#include <stdbool.h>
#include <stdio.h>
#include "struct.h"
#include "fileHandling.h"
#include "utils.h"

#define frameWidth 56
#define frameHeight 49
static uint8_t bitmap[frameHeight*(frameWidth/8)] = {};

void findNextBlock (int width, int height, bool frameBeingOverwritten[width][height], bool overwritingFrame[width][height], FILE* file) {
    bool stuffChanging = 1;
    while (stuffChanging) {
        int ch = getchar();
        struct dataBlock block = {0};
        block.x1 = 0;
        block.y1 = 0;
        block.x2 = 0;
        block.y2 = 0;
        block.bitmap = bitmap;

        stuffChanging = 0;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (overwritingFrame[x][y] != frameBeingOverwritten[x][y]) {
                    block.x1 = x;
                    block.y1 = y;
                    stuffChanging = 1;
                    goto nestedBreak;
                }
            }
        }
        nestedBreak:

        while (overwritingFrame[block.x1+block.x2+1][block.y1] != frameBeingOverwritten[block.x1+block.x2+1][block.y1]) {
            block.x2++;
        }

        int samePixels = 0;

        while (samePixels == 0) {
            block.y2++;
            for (int x = 0; x < block.x2+1; x++) {
                if (overwritingFrame[block.x1+x][block.y1+block.y2] == frameBeingOverwritten[block.x1+x][block.y1+block.y2]) {
                    samePixels++;
                }
            }
        }
        block.y2--;

        fillBlock(width, height, overwritingFrame, &block);

        ////// render //////
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                bool selected = 0;
                if (x >= block.x1 && x <= block.x1+block.x2 && y >= block.y1 && y <= block.y1+block.y2)
                    selected = 1;

                if (overwritingFrame[x][y]) {
                    if (!selected) {
                        if (frameBeingOverwritten[x][y])
                            printf("  ");
                        else // changed
                            printf("◍ ");
                    } else {
                        if (frameBeingOverwritten[x][y])
                            printf("□ ");
                        else // changed
                            printf("▥ ");
                    }
                } else {
                    if (!selected) {
                        if (frameBeingOverwritten[x][y]) // changed
                            printf("○ ");
                        else 
                            printf("  ");
                    } else {
                        if (frameBeingOverwritten[x][y]) // changed
                            printf("▤ ");
                        else 
                            printf("□ ");
                    }
                }
                if (selected) {
                    overwritingFrame[x][y] = frameBeingOverwritten[x][y];
                }
            }
            printf("\n");
        }


        if (stuffChanging)
            writeBlock(block, file);
    }
}
