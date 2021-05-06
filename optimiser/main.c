#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <threads.h>
#include "utils.h"
#include "optimization.h"
#include "threads.h"
#include <sys/prctl.h>

#include "fileHandling.h"

//#define frameWidth 56
//#define frameHeight 49
#define frameWidth 240
#define frameHeight 240

#define thread_count 16

    
struct frameData {
    struct dataBlock *blocks;
    int blocksLength;
    int frameNr;
    int status;
};

void printData (char* string) {
    static int prevStringLen = 0;

    for (int i = 0; i < prevStringLen; i++) {
        printf("\b");
    }

    printf(string);
    fflush(stdout);

    prevStringLen = 0;
    while (string[prevStringLen] != '\0') {
        prevStringLen++;
    }
}

void threadInfo(int *framesBeingProcessed, int start) {
    char string[200] = {'\0'};

    for (int i = 0; i < thread_count; i++) {
        sprintf(string, "%s%i ", string, framesBeingProcessed[i] != -1? framesBeingProcessed[i] + start + 1 : -1);
    }
    printData(string);
}

int thread(void *arg) {
    struct frameData *data = ((struct frameData*)arg);


    bool frameBeingOverwritten[frameWidth][frameHeight] = {};

    bool newFrame[frameWidth][frameHeight] = {};

    readFrame(frameWidth, frameHeight, frameBeingOverwritten, data->frameNr - 1);

    readFrame(frameWidth, frameHeight, newFrame, data->frameNr);

    struct dataBlock *blocksPointer;
    struct dataBlock **blocks = &blocksPointer;

    int blocksLength = 0;

    findSimpleBlocks(frameWidth, frameHeight, frameBeingOverwritten, newFrame, blocks, &blocksLength);

    optimizeBlocks(frameWidth, frameHeight, frameBeingOverwritten, newFrame, blocks, &blocksLength);

    data->blocksLength = blocksLength;

    if (blocksLength > 0)
        data->blocks = *blocks;

    data->status = 1;

    return 0;
}

int main() {
    uint8_t bitmap[frameHeight*(frameWidth/8)] = {};
    uint8_t startFrame[frameHeight*(frameWidth/8)] = {};
    bool frameBeingOverwritten[frameWidth][frameHeight] = {};

    bool newFrame[frameWidth][frameHeight] = {};

    FILE *file;
    file = fopen("output","wb");
    
    int start = 1;
    int end = 500;

    readFrame(frameWidth, frameHeight, newFrame, start);
    struct dataBlock block = {0};
    block.x1 = 0;
    block.y1 = 0;
    block.x2 = frameWidth-1;
    block.y2 = frameHeight-1;
    block.newFrame = 0;
    block.bitmap = startFrame;

    fillBlock(frameWidth, frameHeight, newFrame, &block);

    writeBlock(block, file);

    for (int y = 0; y < frameHeight; y++) {
        for (int x = 0; x < frameWidth; x++) {
            frameBeingOverwritten[x][y] = newFrame[x][y];
        }
    }


    struct frameData frames[end - start - 1];
    int framesBeingProcessed[thread_count];
    for (int i = 0; i < thread_count; i++)
        framesBeingProcessed[i] = -1;
    

    thrd_t t[thread_count];

    for (int i = start + 1; i < end; i++) {
        int arrayIndex = i - start - 1;

        frames[arrayIndex].frameNr = i;
        frames[arrayIndex].status = 0;

        bool startedProcessing = 0;
        while (!startedProcessing) {
            threadInfo(framesBeingProcessed, start);

            for (int thr = 0; thr < thread_count; thr++) {
                int frame = framesBeingProcessed[thr];
                if (frame == -1 || frames[frame].status == 1) { // if not busy
                    if (frame != -1) {
                        int res;
                        thrd_join(t[thr], &res);
                    }
                    thrd_create(&t[thr], thread, &frames[arrayIndex]);

                    framesBeingProcessed[thr] = arrayIndex;
                    startedProcessing = 1;
                    break;
                }
            }
        }
    }

    bool finished = 0;
    while (!finished) {
        finished = 1;
        for (int thr = 0; thr < thread_count; thr++) {
            int frame = framesBeingProcessed[thr];
            if (frame != -1) {
                finished = 0;
            }
            if (frame != -1 && frames[frame].status == 1) {
                int res;
                thrd_join(t[thr], &res);
                framesBeingProcessed[thr] = -1;
            }
            threadInfo(framesBeingProcessed, start);
        }
    }
    printf("\n");

    for (int i = start + 1; i < end; i++) {
        int arrayIndex = i - start - 1;
        readFrame(frameWidth, frameHeight, newFrame, i);

        printf("%i\n", i);//frames[arrayIndex].blocksLength);

        for (int j = 0; j < frames[arrayIndex].blocksLength;  j++) {
            frames[arrayIndex].blocks[j].bitmap = bitmap;

            if (j == 0) {
                frames[arrayIndex].blocks[j].newFrame = 1;
            } else {
                frames[arrayIndex].blocks[j].newFrame = 0;
            }

            fillBlock(frameWidth, frameHeight, newFrame, &frames[arrayIndex].blocks[j]);

            writeBlock(frames[arrayIndex].blocks[j], file);
        }
    }

    fclose(file);
}
