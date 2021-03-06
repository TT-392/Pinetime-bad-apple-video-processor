#pragma once
#include "stdbool.h"
#include "stdint.h"

struct dataBlock {
    int x1;
    int y1;
    int x2; // relative coords from x1 and y1
    int y2;

    bool newFrame;
    bool flipped;
    bool runLength_encoded;
    bool eof;
    bool staticFrames;
    int staticAmount;

    uint8_t* bitmap;
};

