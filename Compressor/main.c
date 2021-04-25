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

        printf("%i\n", i);
        i++;

        writeBlock_compressed (data, outfile);
    }

    fclose(infile);
    fclose(outfile);
}
