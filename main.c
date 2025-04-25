#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "cache.h"

#define FILENAME "traces/qsort_memtrace.txt"
#define ADDRESS_LENGTH 8
#define CACHE_HIT 2
#define VICTIM_CACHE_HIT 1
#define CACHE_MISS 0

int run_instruction(Cache *cache, char* instruction, uint32_t address){
    int output;
    if (strcmp("L", instruction) == 0){
        output = cache_get(cache, address);
        return output;
    } else if (strcmp("S", instruction) == 0){
        cache_put(cache, address);
    }
    return -1; // flag variable to determine if we wrote or not
}

int main(){
    int total_ops = 0;
    int cache_hits = 0;
    int misses = 0;
    int victim_cache_hits = 0;

    Cache* cache = create_cache();
    FILE *trace_file;
    trace_file = fopen(FILENAME, "r");

    if (trace_file == NULL){
        printf("Error opening file!\n");
        return 0;
    }

    char *line = NULL;
    char *original_line = NULL;
    size_t len = 0;
    size_t read;
  
    char* instruction;
    char* address_string;
    int output;
    while ((read = getline(&line, &len, trace_file)) != -1) {
        original_line = line;
        
        // trim all leading whitespace since input file is weirdly formatted
        char *trimmed_line = line;
        while (*trimmed_line && isspace(*trimmed_line)){
            trimmed_line++;
        }
        
        // duplicate line and tokenize
        char *line_copy = strdup(trimmed_line);
        instruction = strtok(line_copy, " ");
        address_string = strtok(NULL, " ,");

        address_string[strcspn(address_string, "\r\n")] = 0; // trim any unneccesary return characters added by windows
        
        // check if address is too long
        /**
         * This happens occasionally in the trace whree 40 bit addresses are given.
         * I'm not sure why this happens, but for the purpose of the simulation,
         * we can just ignore them.
         */
        char *endptr;
        unsigned long addr = strtoul(address_string, &endptr, 16);
        
        if (addr > 0xFFFFFFFF) {
            fprintf(stderr, "Skipping oversized address: %s\n", address_string);
            free(line_copy);
            continue; 
        }
        
        uint32_t address = (uint32_t) addr;
        
        output = run_instruction(cache, instruction, address);
        total_ops++;
        if (output == CACHE_HIT){
            cache_hits++;
        } else if (output == VICTIM_CACHE_HIT){
            victim_cache_hits++;
        } else if (output == CACHE_MISS){
            misses++;
        }
        
        free(line_copy); // Free our duplicate
        line = original_line; // Restore original pointer
    }
    
    if (line)
        free(line);  // Free the buffer allocated by getline
        
    fclose(trace_file);

    printf("Total operations: %d\n", total_ops);
    printf("Total Cache Hits: %d\n", cache_hits);
    printf("Total Victim Cache Hits: %d\n", victim_cache_hits);
    printf("Total Misses: %d\n", misses);

    free_cache(cache);
    return 0;
}