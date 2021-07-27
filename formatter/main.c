#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "fileHandling.h"

int main() {
    FILE *infile;
    infile = fopen("output_compressed.lz4","rb");


    int i = 0;
    fseek(infile, 0, SEEK_END);
    int fileSize = ftell(infile);
    fseek(infile, 0, SEEK_SET);

    

    int dataSize = 0x1000*120;


    int c = fgetc(infile);
    for (int j = 0; j < fileSize / dataSize; j++) {
        FILE *outfile;
        char fileName[30];
        sprintf(fileName, "video_%i.h", j);
        outfile = fopen(fileName,"w");
        char buffer [100];
        fprintf(outfile, "#include <stdint.h>\n\nint videoLength = %i;\nconst uint8_t video[%i] = {\n", dataSize, dataSize);


        for (int i = 0; i < dataSize; i++) {
            fprintf(outfile, "0x%02x", c);
            c = fgetc(infile);

            if (i != dataSize - 1)
                fprintf(outfile, ",");

            if ((i + 1) % 16 == 0)
                fprintf(outfile, "\n");
        }
        // insert code to write remaining data

        fprintf(outfile, "};");
        fclose(outfile);
    }

    if (fileSize % dataSize > 0) {
        FILE *outfile;
        char fileName[30];
        sprintf(fileName, "video_%i.h", fileSize / dataSize);
        outfile = fopen(fileName,"w");
        char buffer [100];
        int lastFileSize = fileSize % dataSize;
        fprintf(outfile, "#include <stdint.h>\n\nint videoLength = %i;\nconst uint8_t video[%i] = {\n", lastFileSize, lastFileSize);


        for (int i = 0; i < lastFileSize; i++) {
            fprintf(outfile, "0x%02x", c);
            c = fgetc(infile);

            if (i != lastFileSize - 1)
                fprintf(outfile, ",");

            if ((i + 1) % 16 == 0)
                fprintf(outfile, "\n");
        }
        // insert code to write remaining data

        fprintf(outfile, "};");
        fclose(outfile);

    }


    fclose(infile);
}
