#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "struct.h"
#include "fileHandling.h"

int writeBlock(struct dataBlock data, FILE *file) {
    char buffer[50];
    sprintf(buffer, "%i",data.newFrame);
    fputs(buffer, file);
    fputs("\n", file);

    sprintf(buffer, "%i",data.x1);
    fputs(buffer, file);

    fputs("\n", file);

    sprintf(buffer, "%i",data.y1);
    fputs(buffer, file);

    fputs("\n", file);

    sprintf(buffer, "%i",data.x2);
    fputs(buffer, file);

    fputs("\n", file);

    sprintf(buffer, "%i",data.y2);
    fputs(buffer, file);

    fputs("\n", file);

    int blockSize = ((data.x2+1) * (data.y2+1) + 7) / 8; // bytes rounded up

    for (int i = 0; i < blockSize; i++) {
        fputc(data.bitmap[i], file);
    }

    fputs("\n", file);
}

struct dataBlock readBlock(FILE *file) {
    struct dataBlock retval = {};
    retval.newFrame = fgetc(file) == '1' ? 1 : 0;
    fgetc(file);

    char c = fgetc(file);
    while (c != '\n') {
        retval.x1 *= 10;
        retval.x1 += c - '0';
        c = fgetc(file);
    }

    c = fgetc(file);
    while (c != '\n') {
        retval.y1 *= 10;
        retval.y1 += c - '0';
        c = fgetc(file);
    }
    
    c = fgetc(file);
    while (c != '\n') {
        retval.x2 *= 10;
        retval.x2 += c - '0';
        c = fgetc(file);
    }
    
    c = fgetc(file);
    while (c != '\n') {
        retval.y2 *= 10;
        retval.y2 += c - '0';
        c = fgetc(file);
    }
    
    int blockSize = ((retval.x2+1) * (retval.y2+1) + 7) / 8; // bytes rounded up

    retval.bitmap = malloc(blockSize);

    for (int i = 0; i < blockSize + 7 / 8; i++) {
        retval.bitmap[i] = fgetc(file);
    }

    fgetc(file);
    
    return retval;
}

