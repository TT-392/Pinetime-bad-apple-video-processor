#define _XOPEN_SOURCE_EXTENDED
//#include <ncursesw/curses.h> //android
#include <curses.h> //termux android buildable
#include <locale.h>

int variable = 0;
wchar_t currentframe[120][60];

const wchar_t brailread (int row, int bin);

void display_init() {
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    refresh();
}

void wait() {
    int ch = getch();
}

void display_stop() {
    endwin();
}

const wchar_t brailread (int row, int bin) {  //return the braille character with bin represented in binary on row row
    int start = 0x2800;                 //first unicode braille character (all dots empty)
    if (row == 3) {
        return (start + (bin << 6));    //last 2 bits in unicode braille are normal left to right
    } else {
        int x = 0;
        if (bin & 1) x |= 1 << row;
        if (bin & 2) x |= 1 << (row + 3);
        return (x + start);
    }
}

void drawmono(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t* frame) {

    int screensize = (x2-x1+1)*(y2-y1+1);

    int pixelnumber = 0;

    for (int y = y1; y < (y2+1); y++) {
        for (int x = x1; x < (x2+1); x++) {
            if ((frame[pixelnumber / 8] >> (pixelnumber % 8)) & 1) {
                currentframe[x/2][y/4] |= brailread(0,0);
                currentframe[x/2][y/4] &= ~(0xff & brailread(y%4,(x%2+1)));
            } else {
                currentframe[x/2][y/4] |= brailread(y%4,(x%2+1));
            }


            cchar_t pixel = {0,currentframe[x/2][y/4]};

            mvwadd_wch(stdscr,y/4,x/2,&pixel);

            pixelnumber++;
        }
    }
    //std::cout << pixelnumber << std::endl;
    variable = pixelnumber;

    refresh();
}
