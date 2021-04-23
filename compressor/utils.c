#include "struct.h"
#include <stdbool.h>
#include <stdio.h>
#include <time.h>


uint64_t getTimeNS() {
    uint64_t timens = 0;

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    timens = ts.tv_nsec + 1000000000 * ts.tv_sec;

    return timens;
}


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

void renderChanges(int width, int height, bool startFrame[width][height], bool newFrame[width][height], struct dataBlock* blocks, int arrayLength) {
    bool finalFrame[width][height];
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            finalFrame[x][y] = startFrame[x][y];
        }
    }
    for (int i = 0; i < arrayLength; i++) {
        struct dataBlock block = blocks[i];

        ////// render //////
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                bool selected = 0;
                if (x >= block.x1 && x <= block.x1+block.x2 && y >= block.y1 && y <= block.y1+block.y2)
                    selected = 1;

                if (newFrame[x][y]) {
                    if (!selected) {
                        if (startFrame[x][y])
                            printf("  ");
                        else // changed
                            printf("◍ ");
                    } else {
                        if (startFrame[x][y])
                            printf("□ ");
                        else // changed
                            printf("▥ ");
                    }
                } else {
                    if (!selected) {
                        if (startFrame[x][y]) // changed
                            printf("○ ");
                        else 
                            printf("  ");
                    } else {
                        if (startFrame[x][y]) // changed
                            printf("▤ ");
                        else 
                            printf("□ ");
                    }
                }

                if (selected) {
                    newFrame[x][y] = startFrame[x][y];
                }
            }
            printf("\n");
        }
        int ch = getchar();
    }
}
