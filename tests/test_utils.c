#include "test_utils.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

uint32_t state;

void initialize(int argc, char** argv) {

    if (argc < 2) {
        puts("A number seed needs to be given as an argument to the test.");

        exit(0);
    }

    unsigned int seed = atoi(argv[1]);
    state = seed;
}

static uint32_t rng() {
    state = (state * 1103515245) + 12345;
    return state;
}

void randomize_data(uint8_t* buffer, size_t length) {    
    for (size_t i = 0; i < length; i++)
        buffer[i] = rng() % 256;
}

void print_data(uint8_t* buffer, size_t length) {
    for (size_t i = 0; i < length; i++)
        printf("%02x ", buffer[i]);
    printf("\n");
}