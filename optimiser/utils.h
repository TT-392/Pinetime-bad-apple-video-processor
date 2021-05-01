#pragma once
#include "struct.h"

uint64_t getTimeNS();

void fillBlock(int width, int height, bool frame[width][height], struct dataBlock* block);

void readFrame(int width, int height, bool frame[width][height], int frameNr);

void renderChanges(int width, int height, bool startFrame[width][height], bool newFrame[width][height], struct dataBlock* blocks, int arrayLength);

