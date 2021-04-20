#include "struct.h"
#include <stdbool.h>
#include <stdio.h>

void readFrame(int width, int height, bool frame[width][height], int frameNr) {
    FILE *input;
    char buffer[50];
    sprintf(buffer, "../frame%i.mono", frameNr);

    input = fopen(buffer, "rb");
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width/8; x++) {
            uint8_t byte = fgetc(input);
            for (int b = 0; b < 8; b++) {
                frame[x*8 + b][y] = (byte >> b) & 1;
            }
        }
    }

    fclose(input);
}

void fillBlock(int width, int height, bool frame[width][height], struct dataBlock* block) {

    for (int i = 0; i < ((block->y2+1) * (block->x2+1) - 1) / 8 + 1; i++) {
        block->bitmap[i] = 0x00;
    }

    for (int y = 0; y < block->y2 + 1; y++) {
        for (int x = 0; x < block->x2 + 1; x++) {
            block->bitmap[(x + y*(block->x2 + 1))/8] |= (frame[x + block->x1][y + block->y1] << ((x + y*(block->x2 + 1))%8));
        }
    }
}
