#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "fileHandling.h"
#include "pinencurses.h"

int main() {
    display_init();


    FILE *file;
    file = fopen("output_compressed","rb");
    

    while (1) {
        struct dataBlock data = readBlock_compressed(file);

        if (data.eof)
            break;

        if (data.newFrame)
            wait();

        if (!data.staticFrames)
            drawmono(data.x1, data.y1, data.x2+data.x1, data.y2+data.y1, data.bitmap);
        else {
            for (int i = 0; i < data.staticAmount; i++)
                wait();
        }
    }
    fclose(file);

    display_stop();
}
