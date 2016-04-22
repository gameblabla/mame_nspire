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

#include <sys/time.h>
#include <stdio.h>
#include <string.h>

#include <SDL/SDL.h>

#include "minimal.h"

SDL_Event				event;
unsigned char 			*keystates;

SDL_Surface 			*video;
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
	//SDL_BlitSurface(layer,0,video,0);
	SDL_Flip(video);
	od_screen16=(unsigned short *) video->pixels;
}

void odx_video_flip_single(void)
{
	//SDL_BlitSurface(layer,0,video,0);
	SDL_Flip(video);
	od_screen16=(unsigned short *) video->pixels;
}

unsigned int odx_joystick_read()
{
  	unsigned int res=0;
	
	SDL_PollEvent(&event);
	keystates = SDL_GetKeyState(NULL);

	// Key touch management
	if (keystates[SDLK_UP] == SDL_PRESSED)  res |=  OD_UP; // UP
	if (keystates[SDLK_DOWN] == SDL_PRESSED) res |=  OD_DOWN; // DOWN
	if (keystates[SDLK_LEFT] == SDL_PRESSED) res |=  OD_LEFT; // LEFT
	if (keystates[SDLK_RIGHT] == SDL_PRESSED) res |=  OD_RIGHT; // RIGHT

	if (keystates[SDLK_LCTRL] == SDL_PRESSED) { res |=  OD_A;  }  // BUTTON A
	if (keystates[SDLK_LALT] == SDL_PRESSED) { res |=  OD_B; }  // BUTTON B

	if (keystates[SDLK_SPACE] == SDL_PRESSED) { res |=  OD_X;  }  // BUTTON X
	if (keystates[SDLK_LSHIFT] == SDL_PRESSED)  { res |=  OD_Y;  }   // BUTTON Y

	if (keystates[SDLK_BACKSPACE] == SDL_PRESSED)  { res |=  OD_R;  }  // BUTTON R
	if (keystates[SDLK_TAB] == SDL_PRESSED)  { res |=  OD_L;  }  // BUTTON L

	if ( (keystates[SDLK_RETURN] == SDL_PRESSED) )  { res |=  OD_START;  } // START
	if ( (keystates[SDLK_ESCAPE] == SDL_PRESSED) ) { res |=  OD_SELECT; } // SELECT

	return res;
}

unsigned int odx_joystick_press ()
{
	/*unsigned int ExKey=0;
	while (odx_joystick_read() == 0 ) { odx_timer_delay(100); }
	while (!(ExKey=odx_joystick_read() ==0 )) { }
	return ExKey;*/
}

void odx_timer_delay(unsigned int ticks)
{
	/*unsigned long ini=odx_timer_read();
	while (odx_timer_read()-ini<(ticks*1000));*/
}

unsigned long odx_timer_read(void)
{
	/*
	struct timeval tval;
	gettimeofday(&tval, 0);
	return ((tval.tv_sec*1000000)+tval.tv_usec);*/
}

void odx_sound_volume(int vol)
{
}

void odx_sound_play(void *buff, int len)
{
}

static void odx_sound_callback(void *data, Uint8 *stream, int len)
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
	for( i = 0 ; i < OD_KEY_MAX ; i++ ) {
		odx_keys[i] = 0;
	}

	/* General video & audio stuff */
	SDL_Init(SDL_INIT_VIDEO);
	video = SDL_SetVideoMode(320, 240, 16, SDL_SWSURFACE );

	/*SDL_EventState(SDL_ACTIVEEVENT,SDL_IGNORE);
	SDL_EventState(SDL_MOUSEMOTION,SDL_IGNORE);
	SDL_EventState(SDL_MOUSEBUTTONDOWN,SDL_IGNORE);
	SDL_EventState(SDL_MOUSEBUTTONUP,SDL_IGNORE);
	SDL_EventState(SDL_SYSWMEVENT,SDL_IGNORE);
	SDL_EventState(SDL_VIDEORESIZE,SDL_IGNORE);
	SDL_EventState(SDL_USEREVENT,SDL_IGNORE);
	SDL_ShowCursor(SDL_DISABLE);*/

	odx_set_video_mode(bpp,320,240);
	odx_clear_video();
}

void odx_deinit(void)
{
	if (video) SDL_FreeSurface(video);
	video=NULL;
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}


void odx_set_video_mode(int bpp,int width,int height)
{
	/*
	if (layer != NULL) {
		SDL_FreeSurface(layer);
	}

    layer = SDL_CreateRGBSurface (video->flags,
                                320,
                                240,
                                16,
                                video->format->Rmask,
                                video->format->Gmask,
                                video->format->Bmask,
                                video->format->Amask);
	if(layer == NULL) {
		fprintf(stderr, "Couldn't create surface: %s\n", SDL_GetError());
		exit(1);
	}
	*/
	odx_clear_video();
	
	//od_screen16=(unsigned short *) layer->pixels;
	od_screen16=(unsigned short *) video->pixels;
}

void odx_clear_video() {
	SDL_LockSurface(video);
	//SDL_FillRect( layer, NULL, 0 );
	SDL_FillRect( video, NULL, 0 );
	SDL_UnlockSurface(video);
	odx_video_flip();
}

/*
// Font: THIN8X8.pf : Exported from PixelFontEdit 2.7.0
static const unsigned char fontdata8x8[2048] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Char 000 (.)
	0x7E, 0x81, 0xA5, 0x81, 0xBD, 0x99, 0x81, 0x7E,	// Char 001 (.)
	0x7E, 0xFF, 0xDB, 0xFF, 0xC3, 0xE7, 0xFF, 0x7E,	// Char 002 (.)
	0x6C, 0xFE, 0xFE, 0xFE, 0x7C, 0x38, 0x10, 0x00,	// Char 003 (.)
	0x10, 0x38, 0x7C, 0xFE, 0x7C, 0x38, 0x10, 0x00,	// Char 004 (.)
	0x38, 0x7C, 0x38, 0xFE, 0xFE, 0x7C, 0x38, 0x7C,	// Char 005 (.)
	0x10, 0x10, 0x38, 0x7C, 0xFE, 0x7C, 0x38, 0x7C,	// Char 006 (.)
	0x00, 0x00, 0x18, 0x3C, 0x3C, 0x18, 0x00, 0x00,	// Char 007 (.)
	0xFF, 0xFF, 0xE7, 0xC3, 0xC3, 0xE7, 0xFF, 0xFF,	// Char 008 (.)
	0x00, 0x3C, 0x66, 0x42, 0x42, 0x66, 0x3C, 0x00,	// Char 009 (.)
	0xFF, 0xC3, 0x99, 0xBD, 0xBD, 0x99, 0xC3, 0xFF,	// Char 010 (.)
	0x0F, 0x07, 0x0F, 0x7D, 0xCC, 0xCC, 0xCC, 0x78,	// Char 011 (.)
	0x3C, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x7E, 0x18,	// Char 012 (.)
	0x3F, 0x33, 0x3F, 0x30, 0x30, 0x70, 0xF0, 0xE0,	// Char 013 (.)
	0x7F, 0x63, 0x7F, 0x63, 0x63, 0x67, 0xE6, 0xC0,	// Char 014 (.)
	0x99, 0x5A, 0x3C, 0xE7, 0xE7, 0x3C, 0x5A, 0x99,	// Char 015 (.)
	0x80, 0xE0, 0xF8, 0xFE, 0xF8, 0xE0, 0x80, 0x00,	// Char 016 (.)
	0x02, 0x0E, 0x3E, 0xFE, 0x3E, 0x0E, 0x02, 0x00,	// Char 017 (.)
	0x18, 0x3C, 0x7E, 0x18, 0x18, 0x7E, 0x3C, 0x18,	// Char 018 (.)
	0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 0x66, 0x00,	// Char 019 (.)
	0x7F, 0xDB, 0xDB, 0x7B, 0x1B, 0x1B, 0x1B, 0x00,	// Char 020 (.)
	0x3E, 0x63, 0x38, 0x6C, 0x6C, 0x38, 0xCC, 0x78,	// Char 021 (.)
	0x00, 0x00, 0x00, 0x00, 0x7E, 0x7E, 0x7E, 0x00,	// Char 022 (.)
	0x18, 0x3C, 0x7E, 0x18, 0x7E, 0x3C, 0x18, 0xFF,	// Char 023 (.)
	0x18, 0x3C, 0x7E, 0x18, 0x18, 0x18, 0x18, 0x00,	// Char 024 (.)
	0x18, 0x18, 0x18, 0x18, 0x7E, 0x3C, 0x18, 0x00,	// Char 025 (.)
	0x00, 0x18, 0x0C, 0xFE, 0x0C, 0x18, 0x00, 0x00,	// Char 026 (.) right arrow
	0x00, 0x30, 0x60, 0xFE, 0x60, 0x30, 0x00, 0x00,	// Char 027 (.)
	0x00, 0x00, 0xC0, 0xC0, 0xC0, 0xFE, 0x00, 0x00,	// Char 028 (.)
	0x00, 0x24, 0x66, 0xFF, 0x66, 0x24, 0x00, 0x00,	// Char 029 (.)
	0x00, 0x18, 0x3C, 0x7E, 0xFF, 0xFF, 0x00, 0x00,	// Char 030 (.)
	0x00, 0xFF, 0xFF, 0x7E, 0x3C, 0x18, 0x00, 0x00,	// Char 031 (.)
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Char 032 ( )
	0x40, 0x40, 0x40, 0x40, 0x40, 0x00, 0x40, 0x00,	// Char 033 (!)
	0x90, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Char 034 (")
	0x50, 0x50, 0xF8, 0x50, 0xF8, 0x50, 0x50, 0x00,	// Char 035 (#)
	0x20, 0x78, 0xA0, 0x70, 0x28, 0xF0, 0x20, 0x00,	// Char 036 ($)
	0xC8, 0xC8, 0x10, 0x20, 0x40, 0x98, 0x98, 0x00,	// Char 037 (%)
	0x70, 0x88, 0x50, 0x20, 0x54, 0x88, 0x74, 0x00,	// Char 038 (&)
	0x60, 0x20, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,	// Char 039 (')
	0x20, 0x40, 0x80, 0x80, 0x80, 0x40, 0x20, 0x00,	// Char 040 (()
	0x20, 0x10, 0x08, 0x08, 0x08, 0x10, 0x20, 0x00,	// Char 041 ())
	0x00, 0x20, 0xA8, 0x70, 0x70, 0xA8, 0x20, 0x00,	// Char 042 (*)
	0x00, 0x00, 0x20, 0x20, 0xF8, 0x20, 0x20, 0x00,	// Char 043 (+)
	0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x20, 0x40,	// Char 044 (,)
	0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x00,	// Char 045 (-)
	0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x60, 0x00,	// Char 046 (.)
	0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x00,	// Char 047 (/)
	0x70, 0x88, 0x98, 0xA8, 0xC8, 0x88, 0x70, 0x00,	// Char 048 (0)
	0x40, 0xC0, 0x40, 0x40, 0x40, 0x40, 0xE0, 0x00,	// Char 049 (1)
	0x70, 0x88, 0x08, 0x10, 0x20, 0x40, 0xF8, 0x00,	// Char 050 (2)
	0x70, 0x88, 0x08, 0x10, 0x08, 0x88, 0x70, 0x00,	// Char 051 (3)
	0x08, 0x18, 0x28, 0x48, 0xFC, 0x08, 0x08, 0x00,	// Char 052 (4)
	0xF8, 0x80, 0x80, 0xF0, 0x08, 0x88, 0x70, 0x00,	// Char 053 (5)
	0x20, 0x40, 0x80, 0xF0, 0x88, 0x88, 0x70, 0x00,	// Char 054 (6)
	0xF8, 0x08, 0x10, 0x20, 0x40, 0x40, 0x40, 0x00,	// Char 055 (7)
	0x70, 0x88, 0x88, 0x70, 0x88, 0x88, 0x70, 0x00,	// Char 056 (8)
	0x70, 0x88, 0x88, 0x78, 0x08, 0x08, 0x70, 0x00,	// Char 057 (9)
	0x00, 0x00, 0x60, 0x60, 0x00, 0x60, 0x60, 0x00,	// Char 058 (:)
	0x00, 0x00, 0x60, 0x60, 0x00, 0x60, 0x60, 0x20,	// Char 059 (;)
	0x10, 0x20, 0x40, 0x80, 0x40, 0x20, 0x10, 0x00,	// Char 060 (<)
	0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00,	// Char 061 (=)
	0x80, 0x40, 0x20, 0x10, 0x20, 0x40, 0x80, 0x00,	// Char 062 (>)
	0x78, 0x84, 0x04, 0x08, 0x10, 0x00, 0x10, 0x00,	// Char 063 (?)
	0x70, 0x88, 0x88, 0xA8, 0xB8, 0x80, 0x78, 0x00,	// Char 064 (@)
	0x20, 0x50, 0x88, 0x88, 0xF8, 0x88, 0x88, 0x00,	// Char 065 (A)
	0xF0, 0x88, 0x88, 0xF0, 0x88, 0x88, 0xF0, 0x00,	// Char 066 (B)
	0x70, 0x88, 0x80, 0x80, 0x80, 0x88, 0x70, 0x00,	// Char 067 (C)
	0xF0, 0x88, 0x88, 0x88, 0x88, 0x88, 0xF0, 0x00,	// Char 068 (D)
	0xF8, 0x80, 0x80, 0xE0, 0x80, 0x80, 0xF8, 0x00,	// Char 069 (E)
	0xF8, 0x80, 0x80, 0xE0, 0x80, 0x80, 0x80, 0x00,	// Char 070 (F)
	0x70, 0x88, 0x80, 0x80, 0x98, 0x88, 0x78, 0x00,	// Char 071 (G)
	0x88, 0x88, 0x88, 0xF8, 0x88, 0x88, 0x88, 0x00,	// Char 072 (H)
	0xE0, 0x40, 0x40, 0x40, 0x40, 0x40, 0xE0, 0x00,	// Char 073 (I)
	0x38, 0x10, 0x10, 0x10, 0x10, 0x90, 0x60, 0x00,	// Char 074 (J)
	0x88, 0x90, 0xA0, 0xC0, 0xA0, 0x90, 0x88, 0x00,	// Char 075 (K)
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xF8, 0x00,	// Char 076 (L)
	0x82, 0xC6, 0xAA, 0x92, 0x82, 0x82, 0x82, 0x00,	// Char 077 (M)
	0x84, 0xC4, 0xA4, 0x94, 0x8C, 0x84, 0x84, 0x00,	// Char 078 (N)
	0x70, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00,	// Char 079 (O)
	0xF0, 0x88, 0x88, 0xF0, 0x80, 0x80, 0x80, 0x00,	// Char 080 (P)
	0x70, 0x88, 0x88, 0x88, 0xA8, 0x90, 0x68, 0x00,	// Char 081 (Q)
	0xF0, 0x88, 0x88, 0xF0, 0xA0, 0x90, 0x88, 0x00,	// Char 082 (R)
	0x70, 0x88, 0x80, 0x70, 0x08, 0x88, 0x70, 0x00,	// Char 083 (S)
	0xF8, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00,	// Char 084 (T)
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00,	// Char 085 (U)
	0x88, 0x88, 0x88, 0x50, 0x50, 0x20, 0x20, 0x00,	// Char 086 (V)
	0x82, 0x82, 0x82, 0x82, 0x92, 0x92, 0x6C, 0x00,	// Char 087 (W)
	0x88, 0x88, 0x50, 0x20, 0x50, 0x88, 0x88, 0x00,	// Char 088 (X)
	0x88, 0x88, 0x88, 0x50, 0x20, 0x20, 0x20, 0x00,	// Char 089 (Y)
	0xF8, 0x08, 0x10, 0x20, 0x40, 0x80, 0xF8, 0x00,	// Char 090 (Z)
	0xE0, 0x80, 0x80, 0x80, 0x80, 0x80, 0xE0, 0x00,	// Char 091 ([)
	0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x00,	// Char 092 (\)
	0xE0, 0x20, 0x20, 0x20, 0x20, 0x20, 0xE0, 0x00,	// Char 093 (])
	0x20, 0x50, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00,	// Char 094 (^)
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00,	// Char 095 (_)
	0x40, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Char 096 (`)
	0x00, 0x00, 0x70, 0x08, 0x78, 0x88, 0x74, 0x00,	// Char 097 (a)
	0x80, 0x80, 0xB0, 0xC8, 0x88, 0xC8, 0xB0, 0x00,	// Char 098 (b)
	0x00, 0x00, 0x70, 0x88, 0x80, 0x88, 0x70, 0x00,	// Char 099 (c)
	0x08, 0x08, 0x68, 0x98, 0x88, 0x98, 0x68, 0x00,	// Char 100 (d)
	0x00, 0x00, 0x70, 0x88, 0xF8, 0x80, 0x70, 0x00,	// Char 101 (e)
	0x30, 0x48, 0x40, 0xE0, 0x40, 0x40, 0x40, 0x00,	// Char 102 (f)
	0x00, 0x00, 0x34, 0x48, 0x48, 0x38, 0x08, 0x30,	// Char 103 (g)
	0x80, 0x80, 0xB0, 0xC8, 0x88, 0x88, 0x88, 0x00,	// Char 104 (h)
	0x20, 0x00, 0x60, 0x20, 0x20, 0x20, 0x70, 0x00,	// Char 105 (i)
	0x10, 0x00, 0x30, 0x10, 0x10, 0x10, 0x90, 0x60,	// Char 106 (j)
	0x80, 0x80, 0x88, 0x90, 0xA0, 0xD0, 0x88, 0x00,	// Char 107 (k)
	0xC0, 0x40, 0x40, 0x40, 0x40, 0x40, 0xE0, 0x00,	// Char 108 (l)
	0x00, 0x00, 0xEC, 0x92, 0x92, 0x92, 0x92, 0x00,	// Char 109 (m)
	0x00, 0x00, 0xB0, 0xC8, 0x88, 0x88, 0x88, 0x00,	// Char 110 (n)
	0x00, 0x00, 0x70, 0x88, 0x88, 0x88, 0x70, 0x00,	// Char 111 (o)
	0x00, 0x00, 0xB0, 0xC8, 0xC8, 0xB0, 0x80, 0x80,	// Char 112 (p)
	0x00, 0x00, 0x68, 0x98, 0x98, 0x68, 0x08, 0x08,	// Char 113 (q)
	0x00, 0x00, 0xB0, 0xC8, 0x80, 0x80, 0x80, 0x00,	// Char 114 (r)
	0x00, 0x00, 0x78, 0x80, 0x70, 0x08, 0xF0, 0x00,	// Char 115 (s)
	0x40, 0x40, 0xE0, 0x40, 0x40, 0x50, 0x20, 0x00,	// Char 116 (t)
	0x00, 0x00, 0x88, 0x88, 0x88, 0x98, 0x68, 0x00,	// Char 117 (u)
	0x00, 0x00, 0x88, 0x88, 0x88, 0x50, 0x20, 0x00,	// Char 118 (v)
	0x00, 0x00, 0x82, 0x82, 0x92, 0x92, 0x6C, 0x00,	// Char 119 (w)
	0x00, 0x00, 0x88, 0x50, 0x20, 0x50, 0x88, 0x00,	// Char 120 (x)
	0x00, 0x00, 0x88, 0x88, 0x98, 0x68, 0x08, 0x70,	// Char 121 (y)
	0x00, 0x00, 0xF8, 0x10, 0x20, 0x40, 0xF8, 0x00,	// Char 122 (z)
	0x10, 0x20, 0x20, 0x40, 0x20, 0x20, 0x10, 0x00,	// Char 123 ({)
	0x40, 0x40, 0x40, 0x00, 0x40, 0x40, 0x40, 0x00,	// Char 124 (|)
	0x40, 0x20, 0x20, 0x10, 0x20, 0x20, 0x40, 0x00,	// Char 125 (})
	0x76, 0xDC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Char 126 (~)
	0x00, 0x10, 0x38, 0x6C, 0xC6, 0xC6, 0xFE, 0x00	// Char 127 (.)
};
*/
static void odx_text(unsigned short *scr, int x, int y, char *text, int color)
{
	/*unsigned int i,l;
	scr=scr+x+y*320;

	for (i=0;i<strlen(text);i++) {
		
		for (l=0;l<8;l++) {
			scr[l*320+0]=(fontdata8x8[((text[i])*8)+l]&0x80)?odx_palette_rgb[color]:scr[l*320+0];
			scr[l*320+1]=(fontdata8x8[((text[i])*8)+l]&0x40)?odx_palette_rgb[color]:scr[l*320+1];
			scr[l*320+2]=(fontdata8x8[((text[i])*8)+l]&0x20)?odx_palette_rgb[color]:scr[l*320+2];
			scr[l*320+3]=(fontdata8x8[((text[i])*8)+l]&0x10)?odx_palette_rgb[color]:scr[l*320+3];
			scr[l*320+4]=(fontdata8x8[((text[i])*8)+l]&0x08)?odx_palette_rgb[color]:scr[l*320+4];
			scr[l*320+5]=(fontdata8x8[((text[i])*8)+l]&0x04)?odx_palette_rgb[color]:scr[l*320+5];
			scr[l*320+6]=(fontdata8x8[((text[i])*8)+l]&0x02)?odx_palette_rgb[color]:scr[l*320+6];
			scr[l*320+7]=(fontdata8x8[((text[i])*8)+l]&0x01)?odx_palette_rgb[color]:scr[l*320+7];
		}
		//scr+=8;
		scr+=6;
	} */
}

void odx_gamelist_text_out(int x, int y, char *eltexto)
{
	/*char texto[53];
	strncpy(texto,eltexto,52);
	texto[52]=0;
	if (texto[0]!='-')
		odx_text(od_screen16,x+1,y+1,texto,0);
	odx_text(od_screen16,x,y,texto,255);*/
}

/* Variadic functions guide found at http://www.unixpapa.com/incnote/variadic.html */
void odx_gamelist_text_out_fmt(int x, int y, char* fmt, ...)
{
	/*char strOut[128];
	va_list marker;
	
	va_start(marker, fmt);
	vsprintf(strOut, fmt, marker);
	va_end(marker);	

	odx_gamelist_text_out(x, y, strOut);*/
}

//static int log=0;

void odx_printf_init(void)
{
	//log=0;
}

static void odx_text_log(char *texto)
{
	/*if (!log) {
		odx_clear_video();
		odx_clear_video(); // do twice to avoid flickering 
	}
	odx_text(od_screen16,0,log,texto,255); 	odx_video_flip();
	odx_text(od_screen16,0,log,texto,255); 	odx_video_flip(); // do twice to avoid flickering 
	log+=8;
	if(log>239) log=0;*/
}

/* Variadic functions guide found at http://www.unixpapa.com/incnote/variadic.html */
void odx_printf(char* fmt, ...)
{
	/*int i,c;
	char strOut[4096];
	char str[41];
	va_list marker;
	
	va_start(marker, fmt);
	vsprintf(strOut, fmt, marker);
	va_end(marker);	

	c=0;
	for (i=0;i<strlen(strOut);i++)
	{
		str[c]=strOut[i];
		if (str[c]=='\n')
		{
			str[c]=0;
			odx_text_log(str);
			c=0;
		}
		else if (c==39)
		{
			str[40]=0;
			odx_text_log(str);
			c=0;
		}		
		else
		{
			c++;
		}
	}*/
}
