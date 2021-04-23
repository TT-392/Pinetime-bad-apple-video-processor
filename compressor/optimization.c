#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "struct.h"
#include "utils.h"

#define newBlockCost 12 // the amount of time it costs for a new block to be written in units of time it takes to write a pixel

static uint64_t blockCostTime = 0;
static uint64_t overlappingCornersTime = 0;
static uint64_t deleteUselessDataTime = 0;
static uint64_t combinedBlockTime = 0;

void printPerformance() {
    printf("blockCost:\t\t%lli\noverlappingCorners:\t%lli\ndeleteUselessData:\t%lli\ncombinedBlock:\t\t%lli\n", blockCostTime, overlappingCornersTime, deleteUselessDataTime, combinedBlockTime);
}

int blockCost (int width, int height, bool xorPixels[width][height], int max, bool coveredPixels[width][height], struct dataBlock block) {
    uint64_t startTime = getTimeNS();

    int cost = newBlockCost;
    for (int y = 0; y < (block.y2 + 1); y++) {
        for (int x = 0; x < (block.x2 + 1); x++) {
            if ((!xorPixels[x + block.x1][y + block.y1]) || coveredPixels[x + block.x1][y + block.y1]) {
                cost++;
                if (cost > max && max != -1) 
                    return -1;
            }
        }
    }

    blockCostTime += getTimeNS() - startTime;
    return cost;
}

int overlappingCorners (struct dataBlock block1, struct dataBlock block2) {
    uint64_t startTime = getTimeNS();
    int retval = 0;
    int corners = 0;
    for (int i = 0; i < 4; i++) {
        int x = block1.x1;
        int y = block1.x1;
        switch (i) {
            case 0: // top left
                x = block1.x1;
                y = block1.y1;
                break;
            case 1: // top right
                x = block1.x1 + block1.x2;
                y = block1.y1;
                break;
            case 2: // bottom right
                x = block1.x1 + block1.x2;
                y = block1.y1 + block1.y2;
                break;
            case 3: // bottom left
                x = block1.x1;
                y = block1.y1 + block1.y2;
                break;
        }

        if (x >= block2.x1 && x <= block2.x1 + block2.x2) {
            if (y >= block2.y1 && y <= block2.y1 + block2.y2) {
                retval |= 1 << i;
                corners++;
            }
        }
    }
    overlappingCornersTime += getTimeNS() - startTime;

    if (corners >= 2)
        return retval;
    else
        return 0;
}

void deleteUselessData (struct dataBlock **blocksArray, int *arrayLength) {
    uint64_t startTime = getTimeNS();

    for (int i = 0; i < *arrayLength; i++) {
        for (int j = 0; j < *arrayLength; j++) {
            if (i != j) {
                int overlap = overlappingCorners((*blocksArray)[i], (*blocksArray)[j]);

                if (overlap != 0 && overlap != 0xf) {
                    struct dataBlock *block1 = &(*blocksArray)[i];
                    struct dataBlock *block2 = &(*blocksArray)[j];
                    int cut = 0;
                    switch (overlap) {
                        case 0x3: // top 2 corners
                            cut = (block2->y1 + block2->y2 + 1) - block1->y1;
                            block1->y1 += cut;
                            block1->y2 -= cut;
                            break;
                        case 0xc: // bottom 2 corners
                            cut = (block1->y1 + block1->y2 + 1) - block2->y1;
                            block1->y2 -= cut;
                            break;
                        case 0x9: // left 2 corners
                            cut = (block2->x1 + block2->x2 + 1) - block1->x1;
                            block1->x1 += cut;
                            block1->x2 -= cut;
                            break;
                        case 0x6: // right 2 corners
                            cut = (block1->x1 + block1->x2 + 1) - block2->x1;
                            block1->x2 -= cut;
                            break;
                    }
                    printf("shrinking block\n");
                } else if (overlap == 0xf) {
                    printf("deleting useless block\n");
                    memcpy(&(*blocksArray)[i], &(*blocksArray)[i + 1], ((*arrayLength) - (i + 1))*sizeof(struct dataBlock));
                    (*arrayLength)--;
                    i--;
                    j--;
                }
            }
        }
    }
    deleteUselessDataTime += getTimeNS() - startTime;
}

struct dataBlock combinedBlock (struct dataBlock block1, struct dataBlock block2) {
    uint64_t startTime = getTimeNS();
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
    combinedBlockTime += getTimeNS() - startTime;

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

void updateCoveredPixels (int width, int height, bool coveredPixels[width][height], struct dataBlock **blocksArray, int *arrayLength) {
    for (int i = 0; i < *arrayLength; i++) {
        struct dataBlock block = (*blocksArray)[i];
        for (int y = 0; y < block.y2 + 1; y++) {
            for (int x = 0; x < block.x2 + 1; x++) {
                coveredPixels[x + block.x1][y + block.y1] = 1;
            }
        }
    }
}

void optimizeBlocks (int width, int height, bool frameBeingOverwritten[width][height], bool overwritingFrame[width][height], struct dataBlock **blocksArray, int *arrayLength) {
    bool coveredPixels[width][height];
    bool xorPixels[width][height];
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            xorPixels[x][y] = frameBeingOverwritten[x][y] ^ overwritingFrame[x][y];
            coveredPixels[x][y] = 0;
        }
    }
    updateCoveredPixels (width, height, coveredPixels, blocksArray, arrayLength);

    bool stuffToOptimize = 1;
    while (stuffToOptimize) {
        printPerformance();
        stuffToOptimize = 0;

        int maxSavedCost = 0;
        int bestBlock1 = -1;
        int bestBlock2 = -1;
        for (int i = 1; i < *arrayLength; i++) {
            struct dataBlock block1 = (*blocksArray)[i];
            int block1Cost = blockCost(width, height, xorPixels, -1, coveredPixels, block1);

            for (int j = 0; j < i; j++){
                struct dataBlock block2 = (*blocksArray)[j];

                int unmergedCost = block1Cost + blockCost(width, height, xorPixels, -1, coveredPixels, block2);
                int mergedCost = blockCost(width, height, xorPixels, unmergedCost, coveredPixels, combinedBlock(block1, block2));

                int savedCost = unmergedCost - mergedCost;
                if (savedCost > maxSavedCost && mergedCost != -1) {
                    stuffToOptimize = 1;

                    maxSavedCost = savedCost;
                    bestBlock1 = j;
                    bestBlock2 = i;
                }
            }
        }
        if (stuffToOptimize) {
            printf("merging %i and %i\n", bestBlock2, bestBlock1);
            mergeBlocks(bestBlock1 , bestBlock2, blocksArray, arrayLength);
            deleteUselessData (blocksArray, arrayLength);

            updateCoveredPixels (width, height, coveredPixels, blocksArray, arrayLength);
        } 

    }
}

void findSimpleBlocks (int width, int height, bool frameBeingOverwritten[width][height], bool overwritingFrame[width][height], struct dataBlock **blocksArray, int *arrayLength) {
    bool xorPixels[width][height];
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            xorPixels[x][y] = frameBeingOverwritten[x][y] ^ overwritingFrame[x][y];
        }
    }
    *arrayLength = 0;

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
                if (xorPixels[x][y]) {
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

        while (xorPixels[block.x1+block.x2+1][block.y1] && block.x1 + block.x2 + 1 < width) {
            block.x2++;
        }


        int unchangedPixels = 0;

        while (unchangedPixels == 0 && block.y1 + block.y2 < height) {
            block.y2++;
            for (int x = 0; x < block.x2+1; x++) {
                if (!xorPixels[block.x1+x][block.y1+block.y2]) {
                    unchangedPixels++;
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
                    xorPixels[x][y] = 0;
                }
            }
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

        assert (block.x2 + block.x1 < width);
        assert (block.y2 + block.y1 < height);

    }
}
