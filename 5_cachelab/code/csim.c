#include "cachelab.h"
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <math.h>

typedef struct{
    char valid;
    unsigned long tag;
    int used;
} line;

void cache_op(unsigned long tag, unsigned long set, int used, int E, int *num_hit, int *num_miss, int *num_evict, line** cache_line);

int main(int argc, char *argv[])
{   
    char *fname; //name of trace file
    int opt;
    int s, E, b;
    int S, used;
    line** cache_line;
    FILE* file;
    int i, j; //variable of for iteration
    char operation;
    unsigned long address, tag, set, block, mask;
    int size;
    int num_hit, num_miss, num_evict;

    while((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1){
        switch(opt){
            case 'h': break;
            case 'v': break;
            case 's':
                s = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                fname = optarg;
                break;
        }
    
    }
    S = pow(2, s);
    file = fopen(fname, "r"); //declare file pointer

    //cache create & initialize
    cache_line = (line **)malloc(sizeof(line*) * S);
    for(i = 0; i < S; i++){
        cache_line[i] = (line *)malloc(sizeof(line) * E);
        for(j = 0; j < E; j++){
            cache_line[i][j].tag = 0;
            cache_line[i][j].valid = 0;
            cache_line[i][j].used = 0;
        }
    }

    //file read
    while(fscanf(file, " %c %lx,%d", &operation, &address, &size) != EOF){
        mask = (0xffffffffffffffff << b);
        block = ~mask & address;
        mask = 0xffffffffffffffff << (b + s);
        tag = mask & address;

        set = (address & (~(tag | block))) >> b;
        tag = tag >> (b + s);

        // printf("set: %ld tag: %ld block: %ld", set, tag, block);

        if(operation == 'I') continue;
        else if(operation == 'L'){
            cache_op(tag, set, used, E, &num_hit, &num_miss, &num_evict, cache_line);
        }
        else if(operation == 'S'){
            cache_op(tag, set, used, E, &num_hit, &num_miss, &num_evict, cache_line);
        }
        else if(operation == 'M'){
            cache_op(tag, set, used, E, &num_hit, &num_miss, &num_evict, cache_line);
            cache_op(tag, set, used, E, &num_hit, &num_miss, &num_evict, cache_line);
        }
        used++;

    }

    
    fclose(file);

    //free
    for(i = 0; i < S; i++){
        free(cache_line[i]);
    }
    free(cache_line);
    
    printSummary(num_hit, num_miss, num_evict); //hit, miss, eviction

    return 0;
}

void cache_op(unsigned long tag, unsigned long set, int used, int E, int *num_hit, int *num_miss, int *num_evict, line** cache_line){
    int i; //for iteration
    line* cache_line_for_index = cache_line[set];
    int min_used = cache_line_for_index[0].used;
    int min_used_idx = 0;
    int is_full = 1; 

    for(i = 0 ; i < E ; i++){
        if(min_used > cache_line_for_index[i].used){
            min_used_idx = i;
            min_used = cache_line_for_index[i].used;
        }
        if(cache_line_for_index[i].valid == 0){
            is_full = 0;
        }
        if(cache_line_for_index[i].valid && (cache_line_for_index[i].tag == tag)){
            *num_hit += 1;
            cache_line_for_index[i].used  = used;
            return;
        }
    }

    if(is_full == 0){
        //cache line is not full; use empty cache block
        *num_miss += 1;
        for(i = 0; i < E; i++){
            if(cache_line_for_index[i].valid == 0){
                cache_line_for_index[i].valid = 1;
                cache_line_for_index[i].tag = tag;
                cache_line_for_index[i].used = used;
                break;
            }
        }
    }else{
        //cache line is full; evict with LRU policy
        *num_miss += 1;
        *num_evict += 1;
        cache_line_for_index[min_used_idx].valid = 1;
        cache_line_for_index[min_used_idx].tag = tag;
        cache_line_for_index[min_used_idx].used = used;
    }

    return; 
}