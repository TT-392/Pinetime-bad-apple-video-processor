#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "struct.h"
#include "utils.h"

#define newBlockCost 12 // the amount of time it costs for a new block to be written in units of time it takes to write a pixel

int blockCost (int width, int height, bool xorPixels[width][height], struct dataBlock block) {
    int cost = newBlockCost;
    for (int y = 0; y < (block.y2 + 1); y++) {
        for (int x = 0; x < (block.x2 + 1); x++) {
            if (!xorPixels[x+block.x1][y+block.y1]) {
                cost++;
            }
        }
    }
    return cost;
}

struct dataBlock combinedBlock (struct dataBlock block1, struct dataBlock block2) {
    struct dataBlock returnBlock;
    if (block1.x1 < block2.x1)
        returnBlock.x1 = block1.x1;
    else 
        returnBlock.x1 = block2.x1;

    if (block1.y1 < block2.y1)
        returnBlock.y1 = block1.y1;
    else 
        returnBlock.y1 = block2.y1;

    if (block1.x1 + block1.x2 > block2.x1 + block2.x2)
        returnBlock.x2 = (block1.x1 + block1.x2) - returnBlock.x1;
    else 
        returnBlock.x2 = (block2.x1 + block2.x2) - returnBlock.x1;

    if (block1.y1 + block1.y2 > block2.y1 + block2.y2)
        returnBlock.y2 = (block1.y1 + block1.y2) - returnBlock.y1;
    else 
        returnBlock.y2 = (block2.y1 + block2.y2) - returnBlock.y1;
    return returnBlock;
}

void mergeBlocks(int block1, int block2, struct dataBlock **blocksArray, int *arrayLength) {
    int firstBlock;
    int secondBlock;
    if (block1 < block2) {
        firstBlock = block1;
        secondBlock = block2;
    } else {
        firstBlock = block2;
        secondBlock = block1;
    }

    (*blocksArray)[firstBlock] = combinedBlock((*blocksArray)[block1], (*blocksArray)[block2]);

    memcpy(&(*blocksArray)[secondBlock], &(*blocksArray)[secondBlock + 1], ((*arrayLength) - secondBlock - 1)*sizeof(struct dataBlock));

    (*arrayLength)--;
}

void optimizeBlocks (int width, int height, bool frameBeingOverwritten[width][height], bool overwritingFrame[width][height], struct dataBlock **blocksArray, int *arrayLength) {
    bool xorPixels[width][height];
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            xorPixels[x][y] = frameBeingOverwritten[x][y] ^ overwritingFrame[x][y];
        }
    }

    for (int i = 0; i < *arrayLength; i++) {
        struct dataBlock block1 = (*blocksArray)[i];
        printf("block: %i\n", i);

        int maxSavedCost = 0;
        int bestBlock = -1;
        for (int j = 1; j < *arrayLength; j ++){//= 1 + (i == j - 1)) {
            struct dataBlock block2 = (*blocksArray)[j];
            int unmergedCost = blockCost(width, height, xorPixels, block1) + blockCost(width, height, xorPixels, block2);
            int mergedCost = blockCost(width, height, xorPixels, combinedBlock(block1, block2));

            int savedCost = unmergedCost - mergedCost;
            printf("saved cost for %i: %i\n", j, savedCost);
            if (savedCost > maxSavedCost) {
                printf("merge candidate %i\n", j);

                // check if there are no better merge candidates for the found block
                bool betterBlock = 0;
                for (int k = 0; k < *arrayLength; k++) {
                    int unmergedCost2 = blockCost(width, height, xorPixels, block1) + blockCost(width, height, xorPixels, block2);
                    int mergedCost2 = blockCost(width, height, xorPixels, combinedBlock(block1, block2));
                    if (unmergedCost2 - mergedCost2 > savedCost) {
                        betterBlock = 1;
                        printf("better block found: %i\n", k);
                    }
                }
                if (!betterBlock) {
                    maxSavedCost = savedCost;
                    bestBlock = j;
                }
            }
        }

        if (bestBlock != -1) {
            printf("merging with %i\n", bestBlock);
            mergeBlocks(i, bestBlock, blocksArray, arrayLength);
            break;
        } else {
            printf("no merge candidate found");
        }

    }
}

void findSimpleBlocks (int width, int height, bool frameBeingOverwritten[width][height], bool overwritingFrame[width][height], struct dataBlock **blocksArray, int *arrayLength) {
    *arrayLength = 0;
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

            *blocksArray = (struct dataBlock*)malloc((*arrayLength + 1) * sizeof(struct dataBlock));

            memcpy(*blocksArray, oldBlocks, (*arrayLength) * sizeof(struct dataBlock));

            (*blocksArray)[*arrayLength] = block;
            if (*arrayLength > 0)
                free(oldBlocks);
            (*arrayLength)++;
        }
    }
}
