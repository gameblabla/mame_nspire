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
	if (msdos_init_sound())
		return 1;
	msdos_init_input();
	return 0;
}


/* put here cleanup routines to be executed when the program is terminated. */
void osd_exit(void)
{
	msdos_shutdown_sound();
	msdos_shutdown_input();
}

/* fuzzy string compare, compare short string against long string        */
/* e.g. astdel == "Asteroids Deluxe". The return code is the fuzz index, */
/* we simply count the gaps between maching chars.                       */
int fuzzycmp (const char *s, const char *l)
{
	int gaps = 0;
	int match = 0;
	int last = 1;

	for (; *s && *l; l++)
	{
		if (*s == *l)
			match = 1;
		else if (*s >= 'a' && *s <= 'z' && (*s - 'a') == (*l - 'A'))
			match = 1;
		else if (*s >= 'A' && *s <= 'Z' && (*s - 'A') == (*l - 'a'))
			match = 1;
		else
			match = 0;

		if (match)
			s++;

		if (match != last)
		{
			last = match;
			if (!match)
				gaps++;
		}
	}

	/* penalty if short string does not completely fit in */
	for (; *s; s++)
		gaps++;

	return gaps;
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
	Machine->color_depth = 16;
	
	/* these two are not available in mame.cfg */
	errorlog = 0;

	game_index = -1;
	
/*
	for (i = 1;i < argc;i++)
	{
		if (strcasecmp(argv[i],"-log") == 0)
			errorlog = fopen("error.log","wa");
		if (strcasecmp(argv[i],"-fame") == 0)
			use_fame=1;
		if (strcasecmp(argv[i],"-horizscale") == 0)
			video_scale=1;
		if (strcasecmp(argv[i],"-halfscale") == 0)
			video_scale=2;
		if (strcasecmp(argv[i],"-bestscale") == 0)
			video_scale=3;
		if (strcasecmp(argv[i],"-fastscale") == 0)
			video_scale=4;
		if (strcasecmp(argv[i],"-border") == 0)
			video_border=1;
		if (strcasecmp(argv[i],"-aspect") == 0)
			video_aspect=1;
		if (strcasecmp(argv[i],"-nothrottle") == 0)
			throttle=0;
		if ((strcasecmp(argv[i],"-clock") == 0) && (i<argc-1))
			odx_clock=atoi(argv[i+1]);
        	if (strcasecmp(argv[i],"-playback") == 0)
		{
			i++;
			if (i < argc) 
				playbackname = argv[i];
        	}
	}
*/

	//odx_init(1000,16,44100,16,0,60);
	odx_init(1000,16,44100,16,0,60);
	/*
	res = frontend_help (argc, argv);

	if (res != 1234)
	{
		odx_deinit();
		exit (res);
	}

	init_inpdir();

    if (playbackname != NULL)
        options.playback = osd_fopen(playbackname,0,OSD_FILETYPE_INPUTLOG,0);

    if (options.playback)
    {
        INP_HEADER inp_header;

        osd_fread(options.playback, &inp_header, sizeof(INP_HEADER));

        if (!isalnum(inp_header.name[0])) 
            osd_fseek(options.playback, 0, SEEK_SET); 
        else
        {
            for (i = 0; (drivers[i] != 0); i++)
			{
                if (strcmp(drivers[i]->name, inp_header.name) == 0)
                {
                    game_index = i;
                    printf("Playing back previously recorded game %s (%s) [press return]\n",
                        drivers[game_index]->name,drivers[game_index]->description);
                    getchar();
                    break;
                }
            }
        }
    }
*/

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
				printf("Game \"%s\" not supported\n", gamename_temp);
				return 1;
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

    /*
		for (i=0;i<MAX_CPU;i++)
		{
			int *type=(int*)&(drivers[game_index]->drv->cpu[i].cpu_type);
			if (((*type)&0xff)==CPU_V30)
			{
				*type=((*type)&(~0xff))|CPU_ARMV30;
			}
		}
		for (i=0;i<MAX_CPU;i++)
		{
			int *type=(int*)&(drivers[game_index]->drv->cpu[i].cpu_type);
			if (((*type)&0xff)==CPU_V33)
			{
				*type=((*type)&(~0xff))|CPU_ARMV33;
			}
		}
    */
    
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
    printf ("%s (%s)...\n",drivers[game_index]->description,drivers[game_index]->name);
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
