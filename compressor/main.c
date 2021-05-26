#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "fileHandling.h"

int main() {
    FILE *infile;
    infile = fopen("output","rb");

    FILE *outfile;
    outfile = fopen("output_compressed","wb");

    int i = 0;
    while (1) {
        struct dataBlock data = readBlock(infile);

        if (data.eof)
            break;

        bool optimized = 0;
        int lengthInBits = (data.x2 + 1) * (data.y2 + 1);

//        runLength_encode(&lengthInBits, data.bitmap, &optimized);

        data.runLength_encoded = optimized;

        writeBlock_compressed (data, outfile);
        free(data.bitmap);

        //printf("%i\n", i);
        //if (i == 600000) {
        //    break;
        //}
        i++;
    }

    fclose(infile);
    fclose(outfile);
}
