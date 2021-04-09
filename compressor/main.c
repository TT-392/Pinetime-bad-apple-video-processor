#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "fileHandling.h"

uint8_t bitmap[240*30] = {};

int main() {
    struct dataBlock testData = {0, 0, 239, 239, 0, bitmap};
    FILE *file;
    file = fopen("output","wb");



    for (int i = 1; i < 6000; i++) {
        FILE *input;
        char buffer[50];

        sprintf(buffer, "../video/30fps/apple%05d.png.mono", i);

        printf(buffer);
        printf("\n");

        input = fopen(buffer, "rb");
        for (int y = 0; y < 240; y++) {
            for (int x = 0; x < 30; x++) {
                bitmap[x + y*30] = fgetc(input);
            }
        }
        fclose(input);
        writeBlock(testData, file);
    }



    fclose(file);
}
