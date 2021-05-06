#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

void runLength_encode(int *lengthInBits, uint8_t *bitmap, bool *optimized) {
    int flips = 0;
    int lengthInBytes = (*lengthInBits - 1) / 8 + 1;

    int compressedBMP[240*240+1];

    bool lastBit = 1;

    int sameBits = 0;
    for (int bit = 0; bit < *lengthInBits + 1; bit++) {
        bool currentBit;

        if (bit == *lengthInBits)
            currentBit = !lastBit;
        else
            currentBit = (bitmap[bit / 8] >> (bit % 8)) & 1;

        if (lastBit != currentBit) {
            compressedBMP[flips++] = sameBits;
            sameBits = 0;
        }
        if (sameBits == 0xff) {
            compressedBMP[flips++] = 0xff;
            compressedBMP[flips++] = 0;
            sameBits = 0;
        }

        sameBits++;
        lastBit = currentBit;
    }

    if (flips < lengthInBytes) {

        for (int i = 0; i < flips; i++) {
            bitmap[i] = compressedBMP[i];
        }
        *lengthInBits = flips*8;
        *optimized = 1;
    } 
}
