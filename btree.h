
/*
 * B+ Tree implementation for Key Value Store
 * by Antony Gavidia <agd10@hotmail.com>
 */
#ifndef BTREE_H
#define BTREE_H
#include "data_types.h"
#include "query.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/**
 * B+TREE INFO:
 * -------------
 * - Every node contains m entries where d <= m <= 2d.
 * - "d" = order of tree = node capacity.
 * - EXCEPTION: root capacity is 1 <= m <= 2d.
 * - Number of children: 2d + 1.
 */

struct nodeClass {
    /*MAX number of values to store*/
    int capacity;
    /*node or leaf*/
    char nodeType[5];
    /*array of pointers*/
    struct nodeClass** children;
    /*pointer to parent Node*/
    struct nodeClass* parentPtr;
    /*pointer to left and right sisters (leafs)*/
    struct nodeClass* leftSisterPtr;
    struct nodeClass* rightSisterPtr;
    /*key-values pairs to store*/
    int *keys, *values;
};

/*definitions to resemble types/classes*/
typedef struct nodeClass * NodePtr;
typedef struct nodeClass Node;

/**************** Prototypes ****************/

/** Main Functions*/
NodePtr createNode(char type[], int capacity, NodePtr parentPointer);
NodePtr insert(NodePtr nPtr, int k, int v);
NodePtr getNextChild(NodePtr p, int k);
NodePtr splitLeaf(NodePtr nPtr);
NodePtr splitNode(NodePtr node);
NodePtr traverseTreeBottomUp(NodePtr node);
void addKeyAndChildren(int *kPtr, NodePtr *childPtr, NodePtr lPtr, NodePtr rPtr);
void distributeKV(NodePtr sourcePtr, NodePtr lLeaf, NodePtr rLeaf);
void addKV(int *kPtr, int *vPtr, int k, int v, int addValue);
void pointToParent(NodePtr topNode);

/** Range Scan Functions*/
NodePtr findLeaf(NodePtr nodePtr, int k);
int countRangeVals(NodePtr leafPtr, int start, int end, int counter);
void assignRangeValues(int *arrPtr, NodePtr startLeaf, int start, int end);
int* range(NodePtr rootPtr, int start, int end);

/** Helper Functions*/
void clearAll(int *ptr);
void copyArray(int start, int until, int *arrPtr, int *fromArr);
void copyArrayChildren(int start, int until, NodePtr *arrPtr, NodePtr *fromArr);
void freeNode(NodePtr p);
void freeTree(NodePtr p);
int keyExists(int *intPtr, int len, int num);
int isRoot(NodePtr n);
int strEqual(char *l, char *r);
int arrSize(int *ptr);
int keysOverLimit(NodePtr p);

/** Testing Functions*/
int getChildrenNum(NodePtr *childPtr, int counter);
void treeInfo(NodePtr root);
void printNodeKeys(int *ptr);
void printTreeKeys(NodePtr topNode);
void insertValues(NodePtr rootPtr, int min, int max, char mode);
void countStats(NodePtr r, int* cNode, int* cLeaf, int* over, int* add);
void testFind(NodePtr root, int k);
void testRangeScan(NodePtr root, int start, int end);


/***************************************************************/
/************************** FUNCTIONS **************************/
/***************************************************************/

/******************** MAIN FUNCTIONS ********************/

NodePtr createNode(char type[4], int capacity, NodePtr parentPointer) {
/** Creates an internal node or a leaf node. Keys and Values have
  * (CAPACITY + 1) since nodes are splitted only after capacity is
  * surpassed. Child array have (CAPACITY + 2) since the upper limit is
  * (CAPACITY + 1) + 1 extra child inserted when capacity is surpassed.
  * @param type "node" or "leaf".
  * @param capacity Max number of key-values.
  * @param parentPointer pointer to parent node.
  */
    NodePtr newNodePtr = calloc(1, sizeof(Node));
    newNodePtr->keys = calloc(capacity + 2, sizeof(int));
    newNodePtr->capacity = capacity;
    newNodePtr->parentPtr = parentPointer;
    newNodePtr->leftSisterPtr = NULL;
    newNodePtr->rightSisterPtr = NULL;

    if (strEqual(type, "node")) {
        strcpy(newNodePtr->nodeType, "node\0");
        newNodePtr->children = calloc(capacity + 3, sizeof(Node));
        newNodePtr->values = calloc(0, sizeof(int));
    }
    else {
        strcpy(newNodePtr->nodeType, "leaf\0");
        newNodePtr->children = calloc(0, sizeof(Node));
        newNodePtr->values = calloc(capacity + 2, sizeof(int));
    }

    return newNodePtr;
}

int find(NodePtr nodePtr, int k) {
/** Find value in leaf.*/
    /*Helper function to navigate key and value arrays */
    int getValue(int *kPtr, int *vPtr, int k) {
        if (*kPtr == k)
            return *vPtr;
        else
            return getValue(++kPtr, ++vPtr, k);
    }

    if (strEqual(nodePtr->nodeType, "node"))
        return find(getNextChild(nodePtr, k), k);
    // When leaf is reached
    else {
        if (!keyExists(nodePtr->keys, nodePtr->capacity, k)) {
            return 0;
        }
        else
            return getValue(nodePtr->keys, nodePtr->values, k);
    }
}

NodePtr insert(NodePtr nPtr, int k, int v) {
/** Insert (key, value) in tree and rebalance it if needed.
  * @param nPtr the ROOT node of the tree.
  * @param k key.
  * @param v value.
  * Returns: the ROOT of the tree.
  */
    if (strEqual(nPtr->nodeType, "leaf")) {
        addKV(nPtr->keys, nPtr->values, k, v, 1);
        /* IF capacity is exceeded*/
        if (keysOverLimit(nPtr)) {
            NodePtr parentPtr = splitLeaf(nPtr);
            return traverseTreeBottomUp(parentPtr);
        }
        else
            return traverseTreeBottomUp(nPtr);
    }
    /*IF NODE: traverse tree top-down recursively.*/
    else {
        return insert(getNextChild(nPtr, k), k, v);
    }
    return nPtr;
}

NodePtr traverseTreeBottomUp(NodePtr node) {
/** Traverse tree bottom-up splitting nodes if capacity was exceeded.*/
    if (keysOverLimit(node)) {
        return traverseTreeBottomUp(splitNode(node));
    }
    else if (isRoot(node)) {
        return node;
    }
    else {
        return traverseTreeBottomUp(node->parentPtr);
    }
    return node;
}

NodePtr splitLeaf(NodePtr nPtr) {
/** Split leaf node and return the parent.
  * Structure of parent:
  *           [key0   - key1   - key2   ...]
  *  [child0 - child1 - child2 - child3 ...]
  */
    NodePtr p;
    /* IF leaf is root (parent == NULL)*/
    if (isRoot(nPtr)) {
        NodePtr parent = NULL;
        p = createNode("node", nPtr->capacity, parent);
    }
    /*IF leaf is NOT root*/
    else
        p = nPtr->parentPtr;
    // create 2 empty leafs
    NodePtr lLeaf = createNode("leaf", nPtr->capacity, p);
    NodePtr rLeaf = createNode("leaf", nPtr->capacity, p);
    distributeKV(nPtr, lLeaf, rLeaf);
    // insert key and children in parent node
    addKeyAndChildren(p->keys, p->children, lLeaf, rLeaf);
    // assign sister pointers
    lLeaf->rightSisterPtr = rLeaf;
    lLeaf->leftSisterPtr = nPtr->leftSisterPtr;
    rLeaf->leftSisterPtr = lLeaf;
    rLeaf->rightSisterPtr = nPtr->rightSisterPtr;
    if (nPtr->leftSisterPtr != NULL)
        (nPtr->leftSisterPtr)->rightSisterPtr = lLeaf;
    if (nPtr->rightSisterPtr != NULL)
        (nPtr->rightSisterPtr)->leftSisterPtr = rLeaf;
    // original splitted leaf can be destroyed.
    freeNode(nPtr);
    return p;
}

NodePtr splitNode(NodePtr node) {
/** Split node and return the parent.*/
    NodePtr parent;
    if (isRoot(node)) {
        NodePtr p = NULL;
        parent = createNode("node", node->capacity, p);
    }
    else
        parent = node->parentPtr;
    // create 2 empty nodes
    NodePtr leftNode = createNode("node", node->capacity, parent);
    NodePtr rightNode = createNode("node", node->capacity, parent);
    // determine lower and upper bounds (to be used as indexes)
    int lower = node->capacity/2;
    int upper = node->capacity+1;
    // copy keys and child pointers to new left node
    copyArray(0, lower, leftNode->keys, node->keys);
    copyArrayChildren(0, lower + 1, leftNode->children, node->children);
    // copy keys and child pointers to new right node
    copyArray(lower, upper, rightNode->keys, node->keys);
    copyArrayChildren(lower + 1, upper + 2, rightNode->children, node->children);
    // inser key and children to the parent
    addKeyAndChildren(parent->keys, parent->children, leftNode, rightNode);
    // Clear all keys and assign them again starting at index [1] (lower + 1)
    // Index [0] was lifted to the parent. It Can't be duplicated in the child
    clearAll(rightNode->keys);
    copyArray(lower + 1, upper, rightNode->keys, node->keys);
    // point children to parents recursively
    pointToParent(parent);
    // original splitted node can be destroyed
    freeNode(node);
    return parent;
}

void pointToParent(NodePtr topNode) {
/** Recursively navigate the tree top-down changing the parent
  * pointer to "topNode" (the appropriate parent) for every
  * children until reaching the leafs.
  */
    NodePtr *p = topNode->children;
    while (*p) {
        (*p)->parentPtr = topNode;
        if (strEqual((*p)->nodeType, "node"))
            pointToParent(*p);
        ++p;
    }
}

NodePtr getNextChild(NodePtr p, int k) {
/** Return requested node child at correct position.
  * Position = [i] where "k" < keys[i].
  */
    NodePtr getChildHelper(int *kPtr, NodePtr *nPtr) {
        if (!*kPtr || k < *kPtr) {
            return *nPtr;
        }
        else {
            return getChildHelper(++kPtr, ++nPtr);
        }
    }
    return getChildHelper(p->keys, p->children);
}

void addKV(int *kPtr, int *vPtr, int k, int v, int addValue) {
/** Add a (key, value) keeping the KEY array SORTED.
  * @param *kPtr pointer of keys array.
  * @param *vPtr pointer of values array.
  * @param k key to insert.
  * @param v value to insert
  * @param addValue IF NOT 1 values won't be copied.
  *        Used to copy only keys in internal nodes.
  */
    if (!*kPtr || k == *kPtr) {
        *kPtr = k;
        if (addValue)
            *vPtr = v;
    }
    else if (k < *kPtr) {
        int tKey = *kPtr;
        int tVal = *vPtr;
        *kPtr = k;
        if (addValue)
            *vPtr = v;
        return addKV(++kPtr, ++vPtr, tKey, tVal, addValue);
    }
    else
        return addKV(++kPtr, ++vPtr, k, v, addValue);
}

void addKeyAndChildren(
    int *kPtr, NodePtr *childPtr, NodePtr lPtr, NodePtr rPtr) {
/** Add a key and corresponding left and right leaf nodes.
  * @param *kPtr pointer of keys array in parent node.
  * @param *childPtr pointer of children array in parent node.
  * @param lPtr pointer to left tree.
  * @param rPtr pointer to right tree.
  */
    addKV(kPtr, kPtr, *(rPtr->keys), 0, 0);

    void insertChild(NodePtr *nodeArray, NodePtr childPtr) {
        if (!*nodeArray || (*nodeArray)->keys[0] == childPtr->keys[0])
            *nodeArray = childPtr;
        else if (childPtr->keys[0] < (*nodeArray)->keys[0]) {
            NodePtr tNode = *nodeArray;
            *nodeArray = childPtr;
            return insertChild(++nodeArray, tNode);
        }
        else
            return insertChild(++nodeArray, childPtr);
    }
    // Left node will replace the old oversized node
    insertChild(childPtr, lPtr);
    // Right node will be inserted sequentially next to left node
    insertChild(childPtr, rPtr);
}

void distributeKV(NodePtr sourcePtr, NodePtr lLeaf, NodePtr rLeaf) {
/** Copies appropriate keys into left and right leafs.
  * If lLeaf is of type "leaf" then values are copied as well.
  */
    int lower = sourcePtr->capacity/2;
    int upper = sourcePtr->capacity+1;

    copyArray(0, lower, lLeaf->keys, sourcePtr->keys);
    copyArray(lower, upper, rLeaf->keys, sourcePtr->keys);

    if (strEqual(lLeaf->nodeType, "leaf")) {
        copyArray(0, lower, lLeaf->values, sourcePtr->values);
        copyArray(lower, upper, rLeaf->values, sourcePtr->values);
    }
}

/******************** RANGE SCAN ********************/

NodePtr findLeaf(NodePtr nodePtr, int k) {
/** Return the leaf node where key should be found.*/
    if (strEqual(nodePtr->nodeType, "node"))
        return findLeaf(getNextChild(nodePtr, k), k);
    else
        return nodePtr;
}

int countRangeVals(NodePtr leafPtr, int start, int end, int counter) {
/** Return number of keys in the requested range [start: end].*/
    if (leafPtr == NULL)
        return counter;
    else {
        int* ptr = leafPtr->keys;

        while (*ptr) {
            if (*ptr >= end)
                return counter;
            else if (*ptr >= start)
                ++counter;
            ++ptr;
        }
        return countRangeVals(leafPtr->rightSisterPtr, start, end, counter);
    }
}

void assignRangeValues(int *arrPtr, NodePtr startLeaf, int start, int end) {
/** Assign values to pointer array in range [start: end].
  * Assumes the pointer has been allocated the required memory.
  */
    if (startLeaf == NULL)
        return;

    int* kPtr = startLeaf->keys;
    int* vPtr = startLeaf->values;
    while (*kPtr) {
        if (*kPtr == end)
            return;
        else if (*kPtr >= start) {
            *arrPtr = *vPtr;
            ++arrPtr;
        }
        ++kPtr;
        ++vPtr;
    }
    return assignRangeValues(arrPtr, startLeaf->rightSisterPtr, start, end);
}

int* range(NodePtr rootPtr, int start, int end) {
/** Retrieves tree values from key range [start: end].
  * Returns a NULL pointer if start or end key don't exist in tree.
  */
    if (start > end) {
        int temp = end;
        end = start;
        start = temp;
    }

    int* arrPtr = NULL;
    int counter = 0;

    NodePtr startLeaf = findLeaf(rootPtr, start);
    counter = countRangeVals(startLeaf, start, end, 0);
    if (counter > 0) {
        arrPtr = calloc(counter + 1, sizeof(int));
        assignRangeValues(arrPtr, startLeaf, start, end);
    }
    return arrPtr;
}

/******************** HELPER FUNCTIONS ********************/

int isRoot(NodePtr n) {
/** Check if node is root.*/
    if (n->parentPtr == NULL)
        return 1;
    else
        return 0;
}

int strEqual(char *l, char *r) {
/** Compare 2 strings.*/
    if (strcmp(l, r) == 0)
        return 1;
    else
        return 0;
}

int arrSize(int *ptr) {
/** Get size of array of integers.*/
    int c = 0;
    while (*ptr) {
        ++c;
        ++ptr;
    }
    return c;
}

int keysOverLimit(NodePtr p) {
/** Check if size of keys is greater than the limit.*/
    if (arrSize(p->keys) > p->capacity)
        return 1;
    else
        return 0;
}

void copyArray(int start, int until, int *arrPtr, int *fromArr) {
/** Copy values from one integer array to another.
  * @param start starting index to be copied.
  * @param until last index to be copied (not inclusive).
  * @param arrPtr array to copy to (starting at index 0).
  * @param fromArr array to copy from.
  */
    if (start == until)
        return;
    else
        *arrPtr = fromArr[start];

    return copyArray(++start, until, ++arrPtr, fromArr);
}

void clearAll(int *ptr) {
/** Sets all integers in array to 0.*/
    while (*ptr) {
        *ptr = 0;
        ++ptr;
    }
}

void copyArrayChildren(
    int start, int until, NodePtr *arrPtr, NodePtr *fromArr) {
/** Copy values from one NodePtr array to another (as copyArray).*/
    if (start == until)
        return;
    else
        *arrPtr = fromArr[start];

    return copyArrayChildren(++start, until, ++arrPtr, fromArr);
}

int keyExists(int *intPtr, int len, int num) {
/** Check if num exists in array of keys.*/
    if (len == 0)
        return 0;
    else if (*intPtr == num)
        return 1;
    else
        return keyExists(++intPtr, len - 1, num);
}

void freeNode(NodePtr p) {
/** Frees memory for node and all its contents.*/
    if (p) {
        free(p->keys);
        free(p->values);
        free(p->children);
        free(p);
        p = NULL;
    }
}

void freeTree(NodePtr p) {
/** Frees memory for all nodes in tree recursively.*/
    if (strEqual(p->nodeType, "leaf"))
        freeNode(p);
    else {
        NodePtr* ptr = p->children;
        while (*ptr) {
            freeTree(*ptr);
            ++ptr;
        }
        freeNode(p);
    }
}

/******************** TEST FUNCTIONS ********************/

void insertValues(NodePtr rootPtr, int min, int max, char mode) {
/** Insert values in tree from "min" to "max". Random numbers can
  * also be generated.
  * @param rootPtr root of the tree.
  * @param min lower bound.
  * @param max upper bound (not inclusive).
  * @param mode 's': sequential (min to max).
  *             'b': backwards (max to min).
  *             'r': generate random values instead.
  */
    int start = (mode == 's') ? min: max;
    int end = (mode == 's') ? max : min;

    while (start != end) {
        int k = (mode == 'r') ? (rand() - rand()) : start;
        *rootPtr = *(insert(rootPtr, k, k));

        if (mode == 's')
            ++start;
        else
            --start;
    }
    printf("\ninsertValues (mode %c): Values Inserted!\n\n", mode);
}

int getExpectedHight(int capacity, int expectedVals) {
/** Get expected tree hight assuming tree is at full capacity.*/
    int acc = capacity;
    int level = 1;
    while (acc < expectedVals) {
        acc = acc * capacity;
        ++level;
    }
    return level;
}

void printNodeKeys(int *ptr) {
/** Display keys of single node.*/
    printf("Keys: ");
    while (*ptr) {
        printf("%d . ", *ptr);
        ++ptr;
    }
}

void printTreeKeys(NodePtr topNode) {
/** Display keys of entire tree.*/
    if (strEqual(topNode->nodeType, "leaf")) {
        printf("\n  LEAF--->");
        printNodeKeys(topNode->keys);
    }
    else {
        printf("\nNODE--->");
        printNodeKeys(topNode->keys);

        NodePtr *p = topNode->children;
        while (*p) {
            printTreeKeys(*p);
            ++p;
        }
    }
}

int getChildrenNum(NodePtr *childPtr, int counter) {
/** Get number of children in single node.*/
    if (!*childPtr)
        return counter;
    else
        return getChildrenNum(++childPtr, counter + 1);

    return counter;
}

void countStats(NodePtr r, int* cNode, int* cLeaf, int* over, int* add) {
/** Count nodes (by type), oversized nodes, and keys in leafs.*/
    if (strEqual(r->nodeType, "node")) {
        *cNode += 1;

        NodePtr* p = r->children;
        while (*p) {
            countStats(*p, cNode, cLeaf, over, add);
            ++p;
        }
    }
    else if (strEqual(r->nodeType, "leaf")) {
        *add += arrSize(r->keys);
        *cLeaf += 1;
    }

    if (keysOverLimit(r))
        *over += 1;
}

void treeInfo(NodePtr root) {
/** Print tree information.*/
    int getHight(NodePtr n, int counter) {
        if (strEqual(n->nodeType, "leaf"))
            return counter + 1;
        else
            return getHight(n->children[0], counter + 1);
    }

    printf("\n==== TREE INFO: ====\n\n");
    printf("- Is Root: %s\n", (isRoot(root) == 1) ? "true" : "false");
//    printf(isRoot(root) == 1 ? "- Is ROOT: true\n" : "- Is ROOT: false\n");
    printf("- Root type: %s\n", root->nodeType);
    printf("- Hight: %d\n", getHight(root, 0));

    if (strEqual(root->nodeType, "node")) {
        printf("   *Direct children %d\n",
        getChildrenNum(root->children, 0));
    }

    int cNode = 0, cLeaf = 0, over = 0, add = 0;
    countStats(root, &cNode, &cLeaf, &over, &add);

    printf("- Internal nodes: %d\n", cNode);
    printf("- Leaf nodes: %d\n", cLeaf);
    printf("- Incorrect nodes (overcapacity): %d\n", over);
    printf("- Total values: %d\n", add);

    float occ = (float)add/cLeaf;
    printf("- Avg. leaf occupancy: %.3f\n", occ);

    printf("- Max capacity: %d\n", root->capacity);
}

void testFind(NodePtr root, int k) {
/** Doesn't print anything when value found == k.
  * Assumes values = keys inserted.
  */
    int f = find(root, k);

    if (f == 0)
        printf("Key %d doesn't exist!\n", k);
    else if (f != k)
        printf("WRONG value found! key[%d] = %d!\n", k, f);
}

void testRangeScan(NodePtr root, int start, int end) {
/** Print the values found in the scan.*/
    int* ptr = range(root, start, end);

    if (!ptr)
        printf("SCAN: no results found!\n");
    else {
        printf("Scan results:\n");
        while (*ptr) {
            printf("%d\n", *ptr);
            ++ptr;
        }
    }
}

/**
* Valgrind:
* cd /home/antony/Desktop/workSpaceC &&\
* gcc -o m -g main.c &&\
* valgrind --tool=memcheck --leak-check=yes --show-reachable=yes ./m
*/

#endif
