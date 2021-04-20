#pragma once
#include <stdbool.h>
#include <stdio.h>
#include "struct.h"

void findNextBlock (int width, int height, bool frameBeingOverwritten[width][height], bool overwritingFrame[width][height], FILE* file);
