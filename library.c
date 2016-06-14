#include "library.h"
#include "iso_font.h"

void init_graphics(  ) {
	struct fb_var_screeninfo var_info;
	struct fb_fix_screeninfo fix_info;

	/*set up the memory location*/
		//open framebuffer
	fb = open( "/dev/fb0", O_RDWR );
		//get size
	err = ioctl( fb, FBIOGET_VSCREENINFO, &var_info );
	err = ioctl( fb, FBIOGET_FSCREENINFO, &fix_info );
	sheight = var_info.yres_virtual;
	swidth = fix_info.line_length;
	size = sheight * swidth;
		//put buffer in memory
	mem = mmap( NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fb, 0 );


	err = ioctl( 1, TCGETS, &term );
	term.c_lflag &= ~ICANON;
	term.c_lflag &= ~ECHO;
	err = ioctl( 1, TCSETS, &term );

	return; //unnecessary
}

void exit_graphics(  ) {
	/*unmap memory*/
	err = munmap( mem, size );
	/*turn keyboard back on*/
	term.c_lflag |= ICANON;
	term.c_lflag |= ECHO;
	err = ioctl( 1, TCSETS, &term );
	/*close*/
	close( fb );

	return;
}

void clear_screen(  ) {
	write( 1, "\033[2J", 7 );

	return;
}

char getkey(  ) {
	fd_set rfds;
	struct timeval tv;
	char key;

	FD_ZERO( &rfds );
	FD_SET( 0, &rfds );

	tv.tv_sec = 0;
	tv.tv_usec = 500000;

	if( select( 1, &rfds, NULL, NULL, &tv ) > 0 ) {
		read( 0, &key, 1 );
	}

	return key;
}

void sleep_ms( long ms ) {
	struct timespec ts;

	ts.tv_sec = 0;
	ts.tv_nsec = ms * 1000000;

	nanosleep( &ts, NULL );

	return;
}

void draw_pixel( int x, int y, color_t c ) {
	/*find array position*/
	if( ( ( x >= 0 ) && ( x < swidth ) ) && ( ( y >= 0 ) && ( y < sheight ) ) ) {
		int position = ( y * swidth ) + x;
		lseek( fb, position, SEEK_SET );
		write( fb, ( void * )&c, 2 );
	}

	return;
}

void draw_rect( int x1, int y1, int width, int height, color_t c ) {
	int i;
	for( i = 0; i < width; i++ ) {
		draw_pixel( ( x1 + i ), y1, c );
		draw_pixel( ( x1 + i ), ( y1 + height ), c );
	}
	for( i = 0; i < ( height - 1 ); i++ ) {
		draw_pixel( x1, ( y1 + i ), c );
		draw_pixel( ( x1 + width ), ( y1 + i ), c );
	}

	return;
}

void fill_circle( int x, int y, int r, color_t c ) {
	int i = 0, j = r, decisionOver2 = 1 - j;
	while( i <= j ) {
		draw_pixel( ( j + x ), ( i + y ), c );
		draw_pixel( ( i + x ), ( j + y ), c );
		draw_pixel( ( -j + x ), ( i + y ), c );
		draw_pixel( ( -i + x ), ( j + y ), c );
		draw_pixel( ( -j + x ), ( -i + y ), c );
		draw_pixel( ( -i + x ), ( -j + y ), c );
		draw_pixel( ( j + x ), ( -i + y ), c );
		draw_pixel( ( i + x ), ( -j + y ), c );
		i++;
		if( decisionOver2 <= 0 ) {
			decisionOver2 += ( ( 2 * i ) + 1 );
		} else {
			j--;
			decisionOver2 += ( ( 2 * ( i - j ) ) + 1 );
		}
	}
	if( ( r - 1 ) > 0 ) {
		fill_circle( x, y, ( r - 1 ), c );
	}

	return;
}

void draw_char( int x, int y, const char text, color_t c ) {
	int i, j, base = ( ( ( int )text ) * 16 );
	for( i = 0; i < 16; i++ ) {
		for( j = 0; j < 8; j++ ) {
			if( ( ( iso_font[ base + i ] >> j ) & 0x01 ) == 0x01 ) {
				draw_pixel( ( x + j ), ( y + i ), c );
			}
		}
	}

	return;
}

void draw_text( int x, int y, const char * text, color_t c ) {
	int i, x1 = x, y1 = y;
	while( text[ i ] != '\0' ) {
		if( text[ i ] == ( char )( 0x0D ) ) {
			y1 += 16;
		} else if( text[ i ] == ( char )( 0x09 ) ) {
			x1 += 8;
		} else {
			draw_char( x1, y1, text[ i ], c );
		}

		x1 += 8;
		if( x1 >= swidth ) {
			y1 += 16;
		}
		i++;
	}

	return;
}
