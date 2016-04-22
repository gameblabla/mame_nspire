/***************************************************************************

  osdepend.c

  OS dependant stuff (display handling, keyboard scan...)
  This is the only file which should me modified in order to port the
  emulator to a different system.

***************************************************************************/

#include "driver.h"
#include <signal.h>
#include <time.h>
#include <ctype.h>
#include "stdarg.h"
#include "string.h"
#include "minimal.h"

#ifdef _TINSPIRE
#include <libndls.h>
#endif

int  msdos_init_sound(void);
void msdos_init_input(void);
void msdos_shutdown_sound(void);
void msdos_shutdown_input(void);
int  frontend_help (int argc, char **argv);
void parse_cmdline (int argc, char **argv, int game);
void init_inpdir(void);

static FILE *errorlog;

/* put here anything you need to do when the program is started. Return 0 if */
/* initialization was successful, nonzero otherwise. */
int osd_init(void)
{
	msdos_init_input();
	return 0;
}


/* put here cleanup routines to be executed when the program is terminated. */
void osd_exit(void)
{
	msdos_shutdown_input();
}

int main (int argc, char *argv[])
{
	int res, i, j = 0, game_index;
   	char *playbackname = NULL;
	int use_fame=0;
   	extern int video_scale;
	extern int video_border;
	extern int video_aspect;
	extern int throttle;
	
	char gamename_temp[16];
	unsigned char last_i;
	
	#ifdef _TINSPIRE
	enable_relative_paths(argv);
	#endif

	memset(&options,0,sizeof(options));
	
	/* these two are not available in mame.cfg */
	errorlog = 0;
	last_i = 0;

	game_index = -1;

	//odx_init(1000,16,44100,16,0,60);
	odx_init(1000,16,44100,16,0,60);


	/* If not playing back a new .inp file */
    if (game_index == -1)
    {
			for(i=0;argv[1][i]!='.';i++)
			{
				gamename_temp[i] = argv[1][i];
				last_i = i;
			}
			gamename_temp[last_i+1] = '\0';
            for(i=0;i<drivers[i] && (game_index == -1);i++)
            {
				if  (strcmp(gamename_temp, drivers[i]->name) == 0)
				{
					game_index = i;
					break;
				}
			}

			if (game_index == -1)
			{
				return 0;
			}
    }

	/* parse generic (os-independent) options */
	parse_cmdline (argc, argv, game_index);
	Machine->sample_rate = 0;
	options.samplerate=0;

	/* Replace M68000 by FAME */
#ifdef HAS_FAME
	if (use_fame) 
	{
		for (i=0;i<MAX_CPU;i++)
		{
			int *type=(int*)&(drivers[game_index]->drv->cpu[i].cpu_type);
            #ifdef NEOMAME
			if (((*type)&0xff)==CPU_M68000)
            #else
			if (((*type)&0xff)==CPU_M68000 || ((*type)&0xff)==CPU_M68010 )
            #endif
			{
				*type=((*type)&(~0xff))|CPU_FAME;
			}
		}
	}
#endif

    // Remove the mouse usage for certain games
    if ( (strcasecmp(drivers[game_index]->name,"hbarrel")==0) || (strcasecmp(drivers[game_index]->name,"hbarrelw")==0) ||
         (strcasecmp(drivers[game_index]->name,"midres")==0) || (strcasecmp(drivers[game_index]->name,"midresu")==0) ||
         (strcasecmp(drivers[game_index]->name,"midresj")==0) || (strcasecmp(drivers[game_index]->name,"tnk3")==0) ||
         (strcasecmp(drivers[game_index]->name,"tnk3j")==0) || (strcasecmp(drivers[game_index]->name,"ikari")==0) ||
         (strcasecmp(drivers[game_index]->name,"ikarijp")==0) || (strcasecmp(drivers[game_index]->name,"ikarijpb")==0) ||
         (strcasecmp(drivers[game_index]->name,"victroad")==0) || (strcasecmp(drivers[game_index]->name,"dogosoke")==0) ||
         (strcasecmp(drivers[game_index]->name,"gwar")==0) || (strcasecmp(drivers[game_index]->name,"gwarj")==0) ||
         (strcasecmp(drivers[game_index]->name,"gwara")==0) || (strcasecmp(drivers[game_index]->name,"gwarb")==0) ||
         (strcasecmp(drivers[game_index]->name,"bermudat")==0) || (strcasecmp(drivers[game_index]->name,"bermudaj")==0) ||
         (strcasecmp(drivers[game_index]->name,"bermudaa")==0) || (strcasecmp(drivers[game_index]->name,"mplanets")==0) ||
         (strcasecmp(drivers[game_index]->name,"forgottn")==0) || (strcasecmp(drivers[game_index]->name,"lostwrld")==0) ||
         (strcasecmp(drivers[game_index]->name,"gondo")==0) || (strcasecmp(drivers[game_index]->name,"makyosen")==0) ||
         (strcasecmp(drivers[game_index]->name,"topgunr")==0) || (strcasecmp(drivers[game_index]->name,"topgunbl")==0) ||
         (strcasecmp(drivers[game_index]->name,"tron")==0) || (strcasecmp(drivers[game_index]->name,"tron2")==0) ||
         (strcasecmp(drivers[game_index]->name,"kroozr")==0) ||(strcasecmp(drivers[game_index]->name,"crater")==0) ||
         (strcasecmp(drivers[game_index]->name,"dotron")==0) || (strcasecmp(drivers[game_index]->name,"dotrone")==0) ||
         (strcasecmp(drivers[game_index]->name,"zwackery")==0) || (strcasecmp(drivers[game_index]->name,"ikari3")==0) ||
         (strcasecmp(drivers[game_index]->name,"searchar")==0) || (strcasecmp(drivers[game_index]->name,"sercharu")==0) ||
         (strcasecmp(drivers[game_index]->name,"timesold")==0) || (strcasecmp(drivers[game_index]->name,"timesol1")==0) ||
         (strcasecmp(drivers[game_index]->name,"btlfield")==0) || (strcasecmp(drivers[game_index]->name,"aztarac")==0))
    {
        extern int use_mouse;
        use_mouse=0;
    }

    /* go for it */
    res = run_game (game_index);

	/* close open files */
	if (errorlog) fclose (errorlog);
	if (options.playback) osd_fclose (options.playback);
	if (options.record)   osd_fclose (options.record);
	if (options.language_file) osd_fclose (options.language_file);

	if (res!=0)
	{
		/* wait a key press */
		odx_video_flip_single();
		odx_joystick_press();
	}

   	odx_deinit();

	exit (res);
}
void CLIB_DECL logerror(const char *text,...)
{
	if (errorlog)
	{
		va_list arg;
		va_start(arg,text);
		vfprintf(errorlog,text,arg);
		va_end(arg);
	}
}
