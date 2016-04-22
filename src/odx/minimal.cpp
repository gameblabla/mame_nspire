/*

  GP2X minimal library v0.A by rlyeh, (c) 2005. emulnation.info@rlyeh (swap it!)

  Thanks to Squidge, Robster, snaff, Reesy and NK, for the help & previous work! :-)

  Adapted for OpenDIngux by alekmaul <alekmaul@portabledev.com> August 2012

  License
  =======

  Free for non-commercial projects (it would be nice receiving a mail from you).
  Other cases, ask me first.

  GamePark Holdings is not allowed to use this library and/or use parts from it.

*/

#include <stdio.h>
#include <libndls.h>
#include "n2DLib.h"
#include "minimal.h"

unsigned short			*od_screen16;
volatile unsigned short	odx_palette[512];
unsigned short			odx_palette_rgb[256];
unsigned int			odx_sound_rate=0;
int						odx_sound_stereo=1;
int						rotate_controls=0;
unsigned char			odx_keys[OD_KEY_MAX];

extern int master_volume;

signed int axis_x=0, axis_y=0;

void odx_video_flip(void)
{
	updateScreen();
	od_screen16=(unsigned short *)BUFF_BASE_ADDRESS;
}

void odx_video_flip_single(void)
{
	updateScreen();
	od_screen16=(unsigned short *)BUFF_BASE_ADDRESS;
}

unsigned int odx_joystick_read()
{
  	unsigned int res=0;

	// Key touch management
	if (isKeyPressed(KEY_NSPIRE_UP))  res |=  OD_UP; // UP
	if (isKeyPressed(KEY_NSPIRE_DOWN)) res |=  OD_DOWN; // DOWN
	if (isKeyPressed(KEY_NSPIRE_LEFT)) res |=  OD_LEFT; // LEFT
	if (isKeyPressed(KEY_NSPIRE_RIGHT)) res |=  OD_RIGHT; // RIGHT

	if (isKeyPressed(KEY_NSPIRE_CTRL)) { res |=  OD_A;  }  // BUTTON A
	if (isKeyPressed(KEY_NSPIRE_SHIFT)) { res |=  OD_B; }  // BUTTON B

	if (isKeyPressed(KEY_NSPIRE_VAR)) { res |=  OD_X;  }  // BUTTON X
	if (isKeyPressed(KEY_NSPIRE_DEL))  { res |=  OD_Y;  }   // BUTTON Y

	if (isKeyPressed(KEY_NSPIRE_TAB))  { res |=  OD_R;  }  // BUTTON R
	if (isKeyPressed(KEY_NSPIRE_MENU))  { res |=  OD_L;  }  // BUTTON L

	if ( (isKeyPressed(KEY_NSPIRE_DOC)) )  { res |=  OD_START;  } // START
	if ( (isKeyPressed(KEY_NSPIRE_ESC)) ) { res |=  OD_SELECT; } // SELECT

	return res;
}

unsigned int odx_joystick_press ()
{
}

void odx_timer_delay(unsigned int ticks)
{
}

unsigned long odx_timer_read(void)
{
}

void odx_sound_volume(int vol)
{
}

void odx_sound_play(void *buff, int len)
{
}


void odx_sound_thread_start(void)
{
}

void odx_sound_thread_stop(void)
{
}

void odx_init(int ticks_per_second, int bpp, int rate, int bits, int stereo, int Hz)
{
	int i;

	/* All keys unpressed. */
	for( i = 0 ; i < OD_KEY_MAX ; i++ ) 
	{
		odx_keys[i] = 0;
	}

	/* General video & audio stuff */
	initBuffering();
	clearBufferB();
	updateScreen();

	odx_set_video_mode(bpp,320,240);
	odx_clear_video();
}

void odx_deinit(void)
{
	deinitBuffering();
}


void odx_set_video_mode(int bpp,int width,int height)
{
	odx_clear_video();
	od_screen16=(unsigned short *)BUFF_BASE_ADDRESS;
}

void odx_clear_video() {
	clearBufferB();
	odx_video_flip();
}


static void odx_text(unsigned short *scr, int x, int y, char *text, int color)
{

}

void odx_gamelist_text_out(int x, int y, char *eltexto)
{
}

/* Variadic functions guide found at http://www.unixpapa.com/incnote/variadic.html */
void odx_gamelist_text_out_fmt(int x, int y, char* fmt, ...)
{

}


void odx_printf_init(void)
{

}

static void odx_text_log(char *texto)
{

}

/* Variadic functions guide found at http://www.unixpapa.com/incnote/variadic.html */
void odx_printf(char* fmt, ...)
{

}
