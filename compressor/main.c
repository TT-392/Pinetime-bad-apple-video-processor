#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include "utils.h"
#include "optimization.h"

#include "fileHandling.h"

//#define frameWidth 56
//#define frameHeight 49
#define frameWidth 240
#define frameHeight 240

uint8_t bitmap[frameHeight*(frameWidth/8)] = {};
uint8_t startFrame[frameHeight*(frameWidth/8)] = {};
bool frameBeingOverwritten[frameWidth][frameHeight] = {};
bool newFrame[frameWidth][frameHeight] = {};

int main() {
    FILE *file;
    file = fopen("output","wb");

    for (int i = 1; i < 450; i++) {

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
            struct dataBlock *blocks;
            int blocksLength;
            findSimpleBlocks(frameWidth, frameHeight, frameBeingOverwritten, newFrame, &blocks, &blocksLength);

            optimizeBlocks(frameWidth, frameHeight, frameBeingOverwritten, newFrame, &blocks, &blocksLength);

            printf("%i\n", i);

            for (int i = 0; i < blocksLength;  i++) {
                blocks[i].bitmap = bitmap;

                if (i == 0) {
                    blocks[i].newFrame = 1;
                } else {
                    blocks[i].newFrame = 0;
                }

                fillBlock(frameWidth, frameHeight, newFrame, &blocks[i]);

                writeBlock(blocks[i], file);
            }

            //renderChanges(frameWidth, frameHeight, frameBeingOverwritten, newFrame, blocks, blocksLength);
        }

        for (int y = 0; y < frameHeight; y++) {
            for (int x = 0; x < frameWidth; x++) {
                frameBeingOverwritten[x][y] = newFrame[x][y];
            }
        }

    }



    fclose(file);
}
