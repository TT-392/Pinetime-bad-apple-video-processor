#pragma once
#include <stdbool.h>
#include <stdio.h>
#include "struct.h"

void findSimpleBlocks(int width, int height, bool frameBeingOverwritten[width][height], bool overwritingFrame[width][height], struct dataBlock **blocksArray, int *arrayLength);

void optimizeBlocks (int width, int height, bool frameBeingOverwritten[width][height], bool overwritingFrame[width][height], struct dataBlock **blocksArray, int *arrayLength);

int flipOptimize(int width, int height, bool frameBeingOverwritten[width][height], bool newFrame[width][height]);

