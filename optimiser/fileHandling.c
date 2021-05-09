#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "struct.h"
#include "fileHandling.h"

int writeBlock_compressed (struct dataBlock data, FILE *file) {
    char buffer[50];

    bool shortCoords = 0;
    if (data.x2 <= 0xf && data.y2 <= 0xf) {
        shortCoords = 1;
    }

    static bool once = 1;
    static uint8_t byte = 0;
    static int bit = 0;
    static fpos_t pos;

    if (once) {
        fgetpos(file, &pos);
        fputc(0, file); // placeholder 
        once = 0;
    }
    if (bit != 8) {
        byte |= (data.newFrame | shortCoords << 1) << bit;
        bit += 2;
    } else {
        fpos_t tempPos;
        fgetpos(file, &tempPos);
        fsetpos(file, &pos);
        fputc(byte, file);
        fsetpos(file, &tempPos);
        fputc(0, file); //p plaseholder
        tempPos = pos;
        bit = 0;
    }

    fputc(data.x1, file);
    fputc(data.y1, file);

    if (shortCoords) {
        fputc(data.x2 | (data.y2 << 4), file);
    } else {
        fputc(data.x2, file);
        fputc(data.y2, file);
    }

    if (data.runLength_encoded) {
        int sizeInBits = (data.x2+1) * (data.y2+1);

        int i = 0;
        while (sizeInBits > 0) {
            fputc(data.bitmap[i], file);
            sizeInBits -= data.bitmap[i];
            i++;
        }
    }
    else {
        int blockSize = ((data.x2+1) * (data.y2+1) + 7) / 8; // bytes rounded up

        for (int i = 0; i < blockSize; i++) {
            fputc(data.bitmap[i], file);
        }
    }

}

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

struct dataBlock readBlock_compressed(FILE *file) {
    struct dataBlock retval = {};
    retval.eof = 0;

    static uint8_t byte = 0;
    static int bit = 8;

    if (bit == 8) {
        uint8_t byte = fgetc(file);
        retval.newFrame = c & 1;
        bool shortCoords = (c >> 1) & 1;
    }

    //printf("%i\n", (uint8_t)c); 

    c = fgetc(file);

    retval.x1 = (uint8_t)c;
    //printf("%i\n", (uint8_t)c); 

    c = fgetc(file);

    retval.y1 = (uint8_t)c;
    ///printf("%i\n", (uint8_t)c); 

    if (shortCoords) {
        c = fgetc(file);
        retval.x2 = c & 0xf;
        retval.y2 = (c >> 4) & 0xf;
    } else {
        c = fgetc(file);

        retval.x2 = (uint8_t)c;
        //printf("%i\n", (uint8_t)c); 

        c = fgetc(file);

        retval.y2 = (uint8_t)c;
        ///printf("%i\n", (uint8_t)c); 
    }


    
    if (retval.runLength_encoded) {
        int bits = ((retval.x2+1) * (retval.y2+1) + 7) / 8; // bytes rounded up

        retval.bitmap = malloc((bits - 1) / 8 + 1);
        
        int bit = 0;
        while (bits != bit) {
            c = getc(file);
            for (int i = 0; i < c; i++) {
                retval.bitmap[(bit + i)/8] |= 1 << (bit + i) % 8;
            }
            bit += c;
        }
    } else {
        int blockSize = ((retval.x2+1) * (retval.y2+1) + 7) / 8; // bytes rounded up

        retval.bitmap = malloc(blockSize);

        for (int i = 0; i < blockSize + 7 / 8; i++) {
            retval.bitmap[i] = fgetc(file);
            //printf("%i\n", i);
            //printf("%i\n", retval.bitmap[i]);
        }
    }
    
    return retval;
}


struct dataBlock readBlock(FILE *file) {
    struct dataBlock retval = {};
    retval.newFrame = fgetc(file) == '1' ? 1 : 0;
    fgetc(file);

    retval.eof = 0;

    char c = fgetc(file);

    if (c == EOF) {
        retval.eof = 1;
        return retval;
    }

    while (c != '\n') {
        retval.x1 *= 10;
        retval.x1 += c - '0';
        c = fgetc(file);
    }

    c = fgetc(file);
    if (c == EOF) {
        retval.eof = 1;
        return retval;
    }

    while (c != '\n') {
        retval.y1 *= 10;
        retval.y1 += c - '0';
        c = fgetc(file);
    }
    
    c = fgetc(file);
    if (c == EOF) {
        retval.eof = 1;
        return retval;
    }
    while (c != '\n') {
        retval.x2 *= 10;
        retval.x2 += c - '0';
        c = fgetc(file);
    }
    
    c = fgetc(file);
    if (c == EOF) {
        retval.eof = 1;
        return retval;
    }
    while (c != '\n') {
        retval.y2 *= 10;
        retval.y2 += c - '0';
        c = fgetc(file);
    }
    
    int blockSize = ((retval.x2+1) * (retval.y2+1) + 7) / 8; // bytes rounded up

    retval.bitmap = malloc(blockSize);

    for (int i = 0; i < blockSize; i++) {
        retval.bitmap[i] = fgetc(file);
    }

    fgetc(file);
    
    return retval;
}

