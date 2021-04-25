#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "fileHandling.h"

int main() {
    FILE *infile;
    infile = fopen("output_compressed","rb");

    FILE *outfile;
    outfile = fopen("video.h","w");

    int i = 0;
    fseek(infile, 0, SEEK_END);

    int fileSize = ftell(infile);

    fseek(infile, 0, SEEK_SET);

    char buffer [100];
    fprintf(outfile, "#include <stdint.h>\n\nint videooLength = %i;\nconst uint8_t video[%i] = {\n", fileSize, fileSize);
    

    int c = fgetc(infile);
    for (int i = 0; i < fileSize; i++) {
        fprintf(outfile, "0x%02x", c);
        c = fgetc(infile);

        if (i != fileSize -1)
            fprintf(outfile, ",");

        if ((i + 1) % 16 == 0)
            fprintf(outfile, "\n");
    }

    fprintf(outfile, "};");



    fclose(infile);
    fclose(outfile);
}
