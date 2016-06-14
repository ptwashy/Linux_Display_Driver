#ifndef _CS1550_P1_LIB_H
#define _CS1550_P1_LIB_H 1

/*includes*/
#include <linux/unistd.h>
#include <linux/fb.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>


/*definitions*/
/*-types*/
typedef unsigned short color_t;
/*-variables*/
void * mem;
int err, fb, sheight, swidth;
struct termios term;
size_t size;

/*functions*/
/*-basic controls*/
void init_graphics();
void exit_graphics();
void clear_screen();
char getkey();
void sleep_ms( long ms );
/*-drawing utilities*/
void draw_pixel( int x, int y, color_t c );
void draw_rect( int x1, int y1, int width, int height, color_t c );
void fill_circle( int x, int y, int r, color_t c );
void draw_text( int x, int y, const char * text, color_t c );

#endif
