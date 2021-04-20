#pragma once
#include "struct.h"

void fillBlock(int width, int height, bool frame[width][height], struct dataBlock* block);

void readFrame(int width, int height, bool frame[width][height], int frameNr);
