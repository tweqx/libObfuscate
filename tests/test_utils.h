#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <stdint.h>
#include <stddef.h>

void initialize(int argc, char** argv);
void randomize_data(uint8_t* buffer, size_t length);
void print_data(uint8_t* buffer, size_t length);

#endif