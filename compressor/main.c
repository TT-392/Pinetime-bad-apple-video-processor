#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "utils.h"
#include "optimization.h"

#include "fileHandling.h"

#define frameWidth 56
#define frameHeight 49

static uint8_t bitmap[frameHeight*(frameWidth/8)] = {};
uint8_t startFrame[frameHeight*(frameWidth/8)] = {};
bool frameBeingOverwritten[frameWidth][frameHeight] = {};
bool newFrame[frameWidth][frameHeight] = {};

int main() {
    FILE *file;
    file = fopen("output","wb");


    for (int i = 1; i < 3; i++) {

        readFrame(frameWidth, frameHeight, newFrame, i);

        if (i == 1) {
            struct dataBlock block = {0};
            block.x1 = 0;
            block.y1 = 0;
            block.x2 = frameWidth-1;
            block.y2 = frameHeight-1;
            block.bitmap = startFrame;

            fillBlock(frameWidth, frameHeight, newFrame, &block);

            writeBlock(block, file);
        } else {
            findNextBlock(frameWidth, frameHeight, frameBeingOverwritten, newFrame, file);
            /*bool stuffChanging = 1;
            while (stuffChanging) {
                int ch = getchar();
                struct dataBlock block = {0};
                block.x1 = 0;
                block.y1 = 0;
                block.x2 = 0;
                block.y2 = 0;
                block.bitmap = bitmap;

                stuffChanging = 0;
                for (int y = 0; y < frameHeight; y++) {
                    for (int x = 0; x < frameWidth; x++) {
                        if (newFrame[x][y] != frameBeingOverwritten[x][y]) {
                            block.x1 = x;
                            block.y1 = y;
                            stuffChanging = 1;
                            goto nestedBreak;
                        }
                    }
                }
                nestedBreak:

                while (newFrame[block.x1+block.x2+1][block.y1] != frameBeingOverwritten[block.x1+block.x2+1][block.y1]) {
                    block.x2++;
                }

                int samePixels = 0;

                while (samePixels == 0) {
                    block.y2++;
                    for (int x = 0; x < block.x2+1; x++) {
                        if (newFrame[block.x1+x][block.y1+block.y2] == frameBeingOverwritten[block.x1+x][block.y1+block.y2]) {
                            samePixels++;
                        }
                    }
                }
                block.y2--;

                fillBlock(frameWidth, frameHeight, newFrame, &block);

                ////// render //////
                for (int y = 0; y < frameHeight; y++) {
                    for (int x = 0; x < frameWidth; x++) {
                        bool selected = 0;
                        if (x >= block.x1 && x <= block.x1+block.x2 && y >= block.y1 && y <= block.y1+block.y2)
                            selected = 1;

                        if (newFrame[x][y]) {
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
                            newFrame[x][y] = frameBeingOverwritten[x][y];
                        }
                    }
                    printf("\n");
                }


                if (stuffChanging)
                    writeBlock(block, file);
            } */
        }

        for (int y = 0; y < frameHeight; y++) {
            for (int x = 0; x < frameWidth; x++) {
                frameBeingOverwritten[x][y] = newFrame[x][y];
            }
        }

    }



    fclose(file);
}
