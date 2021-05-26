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

    char filename[30];
    sprintf(filename, "output/frame%i", data->frameNr);
    FILE *file = fopen(filename,"wb");

    bool frameBeingOverwritten[frameWidth][frameHeight] = {};
    bool newFrame[frameWidth][frameHeight] = {};
    readFrame(frameWidth, frameHeight, frameBeingOverwritten, data->frameNr - 1);
    readFrame(frameWidth, frameHeight, newFrame, data->frameNr);

    struct dataBlock *blocksPointer;
    struct dataBlock **blocks = &blocksPointer;
    int blocksLength = 0;

    findSimpleBlocks(frameWidth, frameHeight, frameBeingOverwritten, newFrame, blocks, &blocksLength);
    optimizeBlocks(frameWidth, frameHeight, frameBeingOverwritten, newFrame, blocks, &blocksLength);

    for (int j = 0; j < blocksLength;  j++) {
        (*blocks)[j].bitmap = malloc(sizeof(uint8_t) * ((((*blocks)[j].x2 + 1) * ((*blocks)[j].y2 + 1) - 1) / 8 + 1));

        if (j == 0) {
            (*blocks)[j].newFrame = 1;
        } else {
            (*blocks)[j].newFrame = 0;
        }

        fillBlock(frameWidth, frameHeight, newFrame, &(*blocks)[j]);
        writeBlock((*blocks)[j], file);
        free((*blocks)[j].bitmap);
    }
    free(*blocks);
    fclose(file);

    data->status = 1;
    return 0;
}

int main() {
    uint8_t bitmap[frameHeight*(frameWidth/8)] = {};
    uint8_t startFrame[frameHeight*(frameWidth/8)] = {};
    bool frameBeingOverwritten[frameWidth][frameHeight] = {};

    bool newFrame[frameWidth][frameHeight] = {};

    int start = 1;
//    int end = 6572;
    int end = 1000;
    
    /////////////////
    // first frame //
    /////////////////
    FILE *file;
    file = fopen("output/frame1","wb");
    
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
    fclose(file);
    /////////////////

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

    file = fopen("output/full","wb");
    for (int i = start; i < end; i++) {
        char filename[30];
        sprintf(filename, "output/frame%i", i);
        FILE *infile = fopen(filename,"rb");

        printf(filename);
        printf("\n");
        int c = fgetc(infile);
        while (c != EOF) {
            fputc(c, file);
            c = fgetc(infile);
        }
        printf("\n");
        fclose(infile);
    }
    fclose(file);
    printf("\n");
}
