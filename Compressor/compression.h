#pragma once
#include <stdint.h>
#include <stdbool.h>

void runLength_encode(int *lengthInBits, uint8_t *bitmap, bool *optimized);
