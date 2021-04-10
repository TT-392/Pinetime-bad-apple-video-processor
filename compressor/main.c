#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "fileHandling.h"

uint8_t bitmap[240*30] = {};
bool currentScreen[240][240] = {};
bool newScreen[240][240] = {};

int main() {
    struct dataBlock testData = {0, 0, 239, 239, 0, bitmap};
    FILE *file;
    file = fopen("output","wb");



    for (int i = 1; i < 6572; i++) {
        FILE *input;
        char buffer[50];

        sprintf(buffer, "../video/30fps/apple%05d.png.mono", i);

        printf(buffer);
        printf("\n");

        input = fopen(buffer, "rb");
        for (int y = 0; y < 240; y++) {
            for (int x = 0; x < 30; x++) {
                uint8_t byte = fgetc(input);
                for (int b = 0; b < 8; b++) {
                    newScreen[x*8 + b][y] = (byte >> b) & 1;
                }
            }
        }

        int x1 = 0;
        int y1 = 0;
        int x2 = 0;
        int y2 = 0;
        for (int y = 0; y < 240; y++) {
            for (int x = 0; x < 240; x++) {
                if (currentScreen[x][y] != newScreen[x][y]) {
                    x1 = x;
                    y1 = y;

                    int diffPixels[4] = {0};
                    int samePixels[4] = {0};
                    bool edge[4] = {0};

                    for (int direction = 0; direction < 4; direction++) {

                        switch (direction) {
                            case 0:
                                if (x1 + x2 < 240 - 1) {
                                    for (int i = 0; i < (y2 + 1); i++) {
                                        if (currentScreen[x1 + x2 + 1][y1 + i] != newScreen[x1 + x2 + 1][y1 + i])
                                            diffPixels[direction]++;
                                        else
                                            samePixels[direction]++;
                                    }
                                } else{
                                    edge[direction] = 1;
                                }
                                break;
                            case 1:
                                if (x1 - 1 >= 0) {
                                    for (int i = 0; i < (y2 + 1); i++) {
                                        if (currentScreen[x1 - 1][y1 + i] != newScreen[x1 - 1][y1 + i])
                                            diffPixels[direction]++;
                                        else
                                            samePixels[direction]++;
                                    }
                                } else{
                                    edge[direction] = 1;
                                }
                                break;
                            case 2:
                                if (y1 + y2 < 240 - 1) {
                                    for (int i = 0; i < (x2 + 1); i++) {
                                        if (currentScreen[y1 + y2 + 1][x1 + i] != newScreen[y1 + y2 + 1][x1 + i])
                                            diffPixels[direction]++;
                                        else
                                            samePixels[direction]++;
                                    }
                                } else{
                                    edge[direction] = 1;
                                }
                                break;
                            case 3:
                                if (y1 - 1 >= 0) {
                                    for (int i = 0; i < (x2 + 1); i++) {
                                        if (currentScreen[y1 - 1][x1 + i] != newScreen[y1 - 1][x1 + i])
                                            diffPixels[direction]++;
                                        else
                                            samePixels[direction]++;
                                    }
                                } else{
                                    edge[direction] = 1;
                                }
                                break;
                        }
                    }
                }
            }
        }

        fclose(input);
        writeBlock(testData, file);
    }



    fclose(file);
}
