#pragma once
#include <stdbool.h>
#include <stdio.h>
#include "struct.h"

void findSimpleBlocks(int width, int height, bool frameBeingOverwritten[width][height], bool overwritingFrame[width][height], struct dataBlock **blocksArray, int *uwu);
