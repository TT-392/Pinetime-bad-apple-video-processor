#define _XOPEN_SOURCE_EXTENDED
//#include <ncursesw/curses.h> //android
#include <curses.h> //termux android buildable
#include <locale.h>


void display_init();

void wait();


void drawmono(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t* frame);
