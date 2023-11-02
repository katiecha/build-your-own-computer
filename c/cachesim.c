#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// simulator of a single-level cache and the memory underneath it.

/* you may NOT use the modulus (%) operator and you may 
NOT include math.h, java.lang.Math, or otherwise use 
built-in modulus, logarithmic, or power functions.
*/

// implementation of base-2 log using only integer math
int log2(int n){
    int r=0;
    while (n>>=1) r++;
    return r; 
}

int main(int argc, char *argv[]){
    //-------------------------------------gathering input parameters-------------------------------------
    int cache_size = atoi(argv[2]) << 10; // cache size (not including tags or valid bits) converted from kB to bytes
    int associate = atoi(argv[3]); // associativity
    int block_size = atoi(argv[4]); // block size in bytes

    //-------------------------------------prepping input parameters pt. 1-------------------------------------
    int associate_log = log2(associate);
    int block_size_log = log2(block_size);
    int frames = cache_size >> block_size_log; // calculating frames
    int sets = frames >> associate_log; // calculating sets
    int index = log2(sets); // index or set index = log2(sets) --> used to identify the set

    int max_num_blocks = 16777216 >> block_size_log; // 2^24 = 16777216
    int blocks_in_set = max_num_blocks >> index; // possible blocks in the set

    //-------------------------------------main memory arrays-------------------------------------
    /* VISUALIZATION (memory part can be represented as a simple array of 16M bytes)
    main memory array 1:
    value #n

    main memory array 2:
    --> is value #n in main memory?
    --> is value #n in cache?
    --> mru value for value #n -- used by lru algorithm
    */

    char* main_memory = (char*) calloc(16777216, sizeof(char)); // actual values -- callocing to set as 0s at first
    int* main_memory_supplement = (int*) calloc(3 * max_num_blocks, sizeof(int)); // supplementary info -- callocing to set as 0s at first

    //-------------------------------------prepping input parameters pt.2-------------------------------------
    long address = 0; // address as long
    long tag; // tag as long
    long set_index; // set index as long
    long block_offset; // block offset
    long access_size; // access size as long
    int mru = 1;

    char* instruction = (char*) malloc(10*sizeof(char)); // "store" or "load"
    char* temp_address = (char*) malloc(10*sizeof(char)); // address as string B4 translation
    char* temp_access_size = (char*) malloc(8*sizeof(char)); // access size as string B4 translation
    char* store = (char*) malloc(1024*sizeof(char)); // char array for store
    char* miss = (char*) malloc(10*sizeof(char)); // char array for misses

    //-------------------------------------while loop / file reading-------------------------------------
    int end_of_file = -1;
    FILE* trace_file = fopen(argv[1], "r"); // passes in memory location of current char in file
    end_of_file = fscanf(trace_file, "%s", instruction); // for while loop
    
    while(end_of_file != EOF){
            //-------------------------------------STORE-------------------------------------
            if(strcmp(instruction, "store") == 0){ // checks for store instruction
                fscanf(trace_file, "%s %s %s", temp_address, temp_access_size, store); // reads in remaining vals
                temp_address += 2; // removes 0x
                address = strtol(temp_address, NULL, 16); // long int strtol(const char *str, char **endptr, int base)
                //-------------------------------------mapping addresses to sets-------------------------------------
                // VISUALIZATION: [tag                      index         block offset   ]
                block_offset = address & ((1<<block_size_log)-1); // gets block offset through masking
                set_index = (address >> block_size_log) & ((1<<index)-1); // gets set index through shiftting and masking
                tag = address >> (block_size_log + index); // gets tag

                //-------------------------------------actual storing in main memory array-------------------------------------
                access_size = atoi(temp_access_size);
                char str[2];
                for(int i = 0; i < access_size; i++){
                    str[0] = store[2*i];
                    str[1] = store[2*i+1];
                    main_memory[address+i] = strtol(str, NULL, 16);
                }

                //-------------------------------------main memory checking-------------------------------------
                if ((main_memory_supplement[(3*tag) + (set_index*blocks_in_set*3) + 1]) == 0){ // miss :(
                    strcpy(miss, "miss");  
                }
                else {
                    strcpy(miss, "hit");
                }
                //-------------------------------------main memory adjustment-------------------------------------
                main_memory_supplement[(3*tag) + (set_index*blocks_in_set*3)] = 1; // defines block as in cache
                main_memory_supplement[(3*tag) + (set_index*blocks_in_set*3) + 2] = mru; // for lru algorithm
                mru += 1;

                //-------------------------------------printing-------------------------------------
                printf("store 0x%s %s\n", temp_address, miss);
                temp_address -= 2;
            } 
            //-------------------------------------LOAD-------------------------------------
            else {
                fscanf(trace_file, "%s %s", temp_address, temp_access_size); // reads in other vals
                temp_address += 2; // removes 0x
                address = strtol(temp_address, NULL, 16);
                //-------------------------------------mapping addresses to sets-------------------------------------
                // VISUALIZATION: [tag                      index         block offset   ]
                block_offset = address & ((1<<block_size_log)-1);
                set_index = (address >> block_size_log) & ((1<<index)-1);
                tag = address >> (block_size_log + index);

                //-------------------------------------actual loading from main memory array-------------------------------------
                access_size = atoi(temp_access_size);
                char* load = (char*) malloc(1024*sizeof(char));
                for(int i = 0; i < access_size; i++){
                    load[i] = main_memory[address + i];
                }

                //-------------------------------------main memory checking-------------------------------------
                if(main_memory_supplement[(3*tag) + (set_index*blocks_in_set*3)] == 0){ // checks to see if there is a value in main memory aka COLD
                    strcpy(miss, "miss"); 

                    //-------------------------------------main memory adjustment-------------------------------------
                    main_memory_supplement[(3*tag) + (set_index*blocks_in_set*3)] = 1; // value is in main memory
                    main_memory_supplement[(3*tag) + (set_index*blocks_in_set*3) + 1] = 1; // load into cache
                    main_memory_supplement[(3*tag) + (set_index*blocks_in_set*3) + 2] = mru; // for lru algorithm
                    mru++;

                    //-------------------------------------eviction-------------------------------------
                    int min = 2147483647; // max integer size
                    int evict_index = 0; // index for eventual eviction candidate
                    int counter = 0; // counter for associativity check

                    for(int i = 0; i < blocks_in_set; i++){
                        if (main_memory_supplement[(3*i) + (set_index*blocks_in_set*3) + 1] == 1){
                            counter++;
                            if (main_memory_supplement[(3*i) + (set_index*blocks_in_set*3) + 2] < min){ // less than previously lru value
                                min = main_memory_supplement[(set_index*blocks_in_set*3) + (3*i) + 2]; // becomes new min
                                evict_index = (set_index*blocks_in_set*3) + (3*i); // evict the least recently used below
                            }
                        }
                    }
                    if (counter > associate){ // need to evict?
                        main_memory_supplement[evict_index + 1] = 0; // no longer in cache 
                    }

                } 
                //-------------------------------------main memory checking-------------------------------------
                else if(main_memory_supplement[(3*tag) + (set_index*blocks_in_set*3) + 1] == 0){ // not in cache, but there is a value in main memory
                    strcpy(miss, "miss");

                    //-------------------------------------main memory adjustment-------------------------------------
                    main_memory_supplement[(3*tag) + (set_index*blocks_in_set*3) + 1] = 1; // load into cache
                    main_memory_supplement[(3*tag) + (set_index*blocks_in_set*3) + 2] = mru; // for lru algorithm
                    mru++;

                    //-------------------------------------eviction-------------------------------------
                    int min = 2147483647; // max integer size
                    int evict_index = 0; // index for eventual eviction candidate
                    int counter = 0; // counter for associativity check

                    for(int i = 0; i < blocks_in_set; i++){
                        if (main_memory_supplement[(3*i) + (set_index*blocks_in_set*3) + 1] == 1){
                            counter++;
                            if (main_memory_supplement[(3*i) + (set_index*blocks_in_set*3) + 2] < min){ // less than previously lru value
                                min = main_memory_supplement[(set_index*blocks_in_set*3) + (3*i) + 2]; // becomes new min
                                evict_index = (set_index*blocks_in_set*3) + (3*i); // evict the least recently used below
                            }
                        }
                    }
                    if (counter > associate){ // need to evict?
                        main_memory_supplement[evict_index + 1] = 0; // no longer in cache 
                    }

                } 
                //-------------------------------------main memory checking-------------------------------------
                else { // in cache
                    strcpy(miss, "hit"); 
                    //-------------------------------------main memory adjustment-------------------------------------
                    main_memory_supplement[(3*tag) + (set_index*blocks_in_set*3) + 2] = mru; // for lru algorithm
                    mru++;
                    // no eviction
                }

                //-------------------------------------printing-------------------------------------
                printf("load 0x%s %s ", temp_address, miss);
                temp_address -= 2;
                for(int i = 0; i < access_size; i++){
                    if(load[i] == 0){
                        printf("00"); // for miss
                    } else {
                        printf("%02hhx", load[i]);
                    }
                }
                printf("\n");
            }
            end_of_file = fscanf(trace_file, "%s", instruction); // done!
    }
    fclose(trace_file);
    return EXIT_SUCCESS; // Additionally, your program should exit with a status of 0 (EXIT_SUCCESS). 
}