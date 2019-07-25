/*
 * CS s165 A1
 * Embedded Key Value Store Client
 * Starter Code
 *
 * By Wilson Qin <wilsonqin@seas.harvard.edu>
 * Modified by Antony Gavidia on 07/23/2019
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "data_types.h"

#include "btree.h"

/*
 * parses a query command (one line), and routes it to the corresponding
 * storage engine methods
 */
int parseRouteQuery(char queryLine[], NodePtr nodePtr){
  if(strlen(queryLine) <= 0){
     perror("parseQuery: queryLine length is empty or malspecified.");
     return -1;
  }
  else if(strlen(queryLine) < 2){
      perror("parseQuery: queryLine may be missing additional arguments.");
      return -1;
  }

  KEY_t key, lowKey, highKey;
  VAL_t val;

  // finish loadPath
  // read a binary file
  // parse it into an array of keys and array of values, and a total length
  // pass those as args to a load function
  char *loadPath = NULL;
  (void) loadPath;

  if ( sscanf(queryLine, PUT_PATTERN, &key, &val) >= 1) {
    *nodePtr = *(insert(nodePtr, key, val));

    // printf(PUT_PATTERN, key, val);
  }
  else if( sscanf(queryLine, GET_PATTERN, &key) >= 1 ) {
    int value = find(nodePtr, key);
    if (value == 0)
      printf("\n");
    else
      printf("%d\n", value);

    // printf(GET_PATTERN, key);
  }
  else if( sscanf(queryLine, RANGE_PATTERN, &lowKey, &highKey) >= 1 ) {
    int* ptr = range(nodePtr, lowKey, highKey);
    if (ptr) {
      while (*ptr) {
        printf("%d\n", *ptr);
        ++ptr;
      }
    }

    // printf(RANGE_PATTERN, lowKey, highKey);
  }
  else {
    // query not parsed. handle the query as unknown
    return -1;
  }
  // fflush(stdin);
  return 0;
}

int main(int argc, char *argv[])
{

  // Tree parameters
  const int d_PARAMETER = 124;
  // Max capacity = 248 keys ~ 4kb (page size)
  // Allocation (in leaf nodes):
  // keys = (248 + 2) * 4B = 1000B
  // values = (248 + 2) * 4B = 1000B
  // Node = 2064B
  const int NODE_CAPACITY = 2 * d_PARAMETER;
  // Initial TREE ROOT
  NodePtr parent = NULL;
  NodePtr rootPtr = createNode("leaf", NODE_CAPACITY, parent);

  /**********************************************************/
  /**********************************************************/
  /* For command line & file reads */

	int opt;
  // initial command line argument parsing
  int queriesSourcedFromFile = 0;
  char fileReadBuffer[1023];
	// parse any filepath option for queries input file
	while((opt = getopt(argc, argv, ":if:lrx")) != -1) {

		switch(opt) {
			case 'f':
				printf("filepath: %s\n", optarg);
				queriesSourcedFromFile = 1;

          FILE *fp = fopen(optarg, "r");
          while(fgets(fileReadBuffer, 1023, fp)){
              parseRouteQuery(fileReadBuffer, rootPtr);
          }

          fclose(fp);
          break;
		}
	}
  // should there be any remaining arguments not parsed by the client,
  // then these cases will handle control flow:
  if(optind < argc){
      perror("Warning: extra arguments were not parsed by s165 client.");
  }

	for(; optind < argc; optind++){
		printf("\textra arguments: %s\n", argv[optind]);
	}

  (void) queriesSourcedFromFile;

  /**********************************************************/
  /**********************************************************/
  /* ~~~ TEST ~~~ */

  // 3 modes: sequential ('s'), backwards ('b'), and random ('r')
  // insertValues(rootPtr, 1, 20000000, 'r');

  // testFind(rootPtr, -999);
  // testFind(rootPtr, 56);
  // testFind(rootPtr, 1500);
  //
  // testRangeScan(rootPtr, -500, 150);
  // testRangeScan(rootPtr, 900, 1050);

  // treeInfo(rootPtr);
  // printTreeKeys(rootPtr);

  // to free all memory in tree.
  // freeTree(rootPtr);

  /**********************************************************/
  return 0;
}

// to run navigate to directory and:
// "make && ./main -f txtSamples/<workloadFileName>.txt" or
// "make && ./main"
