#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "fileHandling.h"

uint8_t bitmap[] = {'t','t'};

int main() {
    struct dataBlock testData = {93,3,2,3,3,bitmap};
    FILE *file;
    file = fopen("output","wb");

    writeBlock(testData, file);

    fclose(file);

    file = fopen("output","rb");

    printf(readBlock(file).bitmap);

    fclose(file);
}
