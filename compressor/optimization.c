#include <stdbool.h>
//#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "struct.h"
#include "utils.h"

void findSimpleBlocks (int width, int height, bool frameBeingOverwritten[width][height], bool overwritingFrame[width][height], struct dataBlock **blocksArray, int *uwu) {
    *uwu = 0;
    bool tempFrame[width][height];
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            tempFrame[x][y] = overwritingFrame[x][y];
        }
    }

    bool stuffChanging = 1;
    while (stuffChanging) {
        struct dataBlock block = {0};
        block.x1 = 0;
        block.y1 = 0;
        block.x2 = 0;
        block.y2 = 0;

        stuffChanging = 0;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (tempFrame[x][y] != frameBeingOverwritten[x][y]) {
                    block.x1 = x;
                    block.y1 = y;
                    stuffChanging = 1;
                    goto nestedBreak;
                }
            }
        }
        nestedBreak:

        if (!stuffChanging)
            break;

        while (tempFrame[block.x1+block.x2+1][block.y1] != frameBeingOverwritten[block.x1+block.x2+1][block.y1]) {
            block.x2++;
        }

        int samePixels = 0;

        while (samePixels == 0) {
            block.y2++;
            for (int x = 0; x < block.x2+1; x++) {
                if (tempFrame[block.x1+x][block.y1+block.y2] == frameBeingOverwritten[block.x1+x][block.y1+block.y2]) {
                    samePixels++;
                }
            }
        }
        block.y2--;


        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                bool selected = 0;
                if (x >= block.x1 && x <= block.x1+block.x2 && y >= block.y1 && y <= block.y1+block.y2)
                    selected = 1;

                if (selected) {
                    tempFrame[x][y] = frameBeingOverwritten[x][y];
                }
            }
            //printf("\n");
        }


        if (stuffChanging) {
            struct dataBlock *oldBlocks = *blocksArray;
            *blocksArray = (struct dataBlock*)malloc((*uwu + 1) * sizeof(struct dataBlock));
            for (int i = 0; i < *uwu; i++) {
                (*blocksArray)[i] = oldBlocks[i];
            }

            //memcpy(blocksArray, oldBlocks, (*uwu) * sizeof(struct dataBlock));
            (*blocksArray)[*uwu] = block;
            if (*uwu > 0)
                free(oldBlocks);
            (*uwu)++;
        }
    }
}
