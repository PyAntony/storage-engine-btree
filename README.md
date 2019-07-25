# Embedded Key-Value Database
Implementation of an in-memory storage engine using a B+ Tree data structure in C (CS s165 A1).

## Usage
There are 2 ways to test the engine:  
**a) Test functions:** they are provided in `main` function (`main.c` file) under the TEST headline.
- *insertValues:* function to insert key/values in tree. Modes include sequential, backwards, and randomly generated integers.
- *testFind:* prints an error message if searched key doesn't match the value (assumes the inserted keys and values match).
- *testRangeScan:* prints all values found in range scan.
- *treeInfo:* prints tree information. Useful for debugging.
- *printTreeKeys:* prints all keys in tree node by node. Useful for debugging with small number of insertions and small fanout.
- *freeTree:* frees all memory allocated to build the tree. Specially useful with tools like Valgrind where you need to find if there is indirect or "unreachable" leaked memory after freeing all memory allocated for the tree.   

You can uncomment the functions provided, enter your own parameters and run tests simply running (in root directory):  
```console
make && time ./main
```
Output examples are found in the write-up/design.pdf document.  

**b) Txt files:** generate your own txt files using the DSL shown in `data_types.h`:
```C
// PUT. Example: 'p 100 24'
#define PUT_PATTERN "p %d %d\n"
// GET. Example: 'g 100'
#define GET_PATTERN "g %d\n"
// RANGE SCAN. Example: 'r 10 150'
#define RANGE_PATTERN "r %d %d\n"
```
Each command/query must be its own line. Put the file in the `txtSamples` folder and run (in root directory):
```console
make && ./main -f txtSamples/<workloadFileName>.txt
```
You can run queries through txt files an still uncomment functions like `treeInfo` and `printTreeKeys` to check the state of the tree. Some txt files are included as examples.  

## Tests
Storage engine is tested by inserting 5M, 10M, and 20M random key-value pairs. Times in seconds are 4.4, 9.7, and 21.3 respectively. Output: 
```console
antony@discordia:~/Desktop/S165-A1$ make && time ./main
gcc -std=c99 -D_GNU_SOURCE -ggdb3 -W -Wall -Wextra -Werror -O3   -c -o main.o main.c
gcc -std=c99 -D_GNU_SOURCE -ggdb3 -W -Wall -Wextra -Werror -O3 -o main main.o  

insertValues (mode r): Values Inserted!


==== TREE INFO: ====

- Is Root: true
- Root type: node
- Hight: 3
   *Direct children 141
- Internal nodes: 142
- Leaf nodes: 29897
- Incorrect nodes (overcapacity): 0
- Total values: 4996066
- Avg. leaf occupancy: 167.109
- Max capacity: 248

real	0m4.402s
user	0m4.382s
sys	0m0.020s

antony@discordia:~/Desktop/S165-A1$ make && time ./main
gcc -std=c99 -D_GNU_SOURCE -ggdb3 -W -Wall -Wextra -Werror -O3   -c -o main.o main.c
gcc -std=c99 -D_GNU_SOURCE -ggdb3 -W -Wall -Wextra -Werror -O3 -o main main.o  

insertValues (mode r): Values Inserted!


==== TREE INFO: ====

- Is Root: true
- Root type: node
- Hight: 4
   *Direct children 2
- Internal nodes: 293
- Leaf nodes: 59572
- Incorrect nodes (overcapacity): 0
- Total values: 9984410
- Avg. leaf occupancy: 167.602
- Max capacity: 248

real	0m9.735s
user	0m9.637s
sys	0m0.096s

antony@discordia:~/Desktop/S165-A1$ make && time ./main
gcc -std=c99 -D_GNU_SOURCE -ggdb3 -W -Wall -Wextra -Werror -O3   -c -o main.o main.c
gcc -std=c99 -D_GNU_SOURCE -ggdb3 -W -Wall -Wextra -Werror -O3 -o main main.o  

insertValues (mode r): Values Inserted!


==== TREE INFO: ====

- Is Root: true
- Root type: node
- Hight: 4
   *Direct children 4
- Internal nodes: 586
- Leaf nodes: 118274
- Incorrect nodes (overcapacity): 0
- Total values: 19938115
- Avg. leaf occupancy: 168.576
- Max capacity: 248

real	0m21.322s
user	0m21.205s
sys	0m0.112s

```
