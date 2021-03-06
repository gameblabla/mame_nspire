#include <stdlib.h>
#include <libndls.h>
#include "n2DLib.h"

#ifdef __cplusplus
extern "C" {
#endif

/*             *
 *  Buffering  * 
*/
scr_type_t screen_type;

unsigned short *BUFF_BASE_ADDRESS, *ALT_SCREEN_BASE_ADDRESS, *INV_BUFF, *temp;
void *SCREEN_BACKUP;
int swapped = 0;
unsigned char new_rev;

void initBuffering()
{
	screen_type = lcd_type();
	lcd_init(screen_type);
	
	if (screen_type == SCR_240x320_565)
	{
		new_rev = 1;
	}
	else
	{
		new_rev = 0;
	}
	
	BUFF_BASE_ADDRESS = (unsigned short*)malloc(BUFF_BYTES_SIZE);
	if(!BUFF_BASE_ADDRESS) exit(0);
	
	SCREEN_BACKUP = *(void**)0xC0000010;
	
	// Handle monochrome screens-specific shit
	if(is_classic)
		*(int32_t*)(0xC000001C) = (*((int32_t*)0xC000001C) & ~0x0e) | 0x08;
	
	ALT_SCREEN_BASE_ADDRESS = (unsigned short*)malloc(BUFF_BYTES_SIZE + 8);
	if(!ALT_SCREEN_BASE_ADDRESS)
	{
		free(BUFF_BASE_ADDRESS);
		*((int32_t*)0xC000001C) = (*((int32_t*)0xC000001C) & ~0x0e) | 0x04;
		*(void**)0xC0000010 = SCREEN_BACKUP;
		exit(0);
	}
	
	INV_BUFF = (unsigned short*)malloc(BUFF_BYTES_SIZE);
	if(!INV_BUFF)
	{
		free(ALT_SCREEN_BASE_ADDRESS);
		free(BUFF_BASE_ADDRESS);
		*((int32_t*)0xC000001C) = (*((int32_t*)0xC000001C) & ~0x0e) | 0x04;
		*(void**)0xC0000010 = SCREEN_BACKUP;
		exit(0);
	}
	
	*(void**)0xC0000010 = ALT_SCREEN_BASE_ADDRESS;
}

void updateScreen()
{
	unsigned int *dest, *src, i, c;
	// I use different methods for refreshing the screen for GS and color screens because according to my tests, the fastest for one isn't the fastest for the other
		if (has_colors)
		{
			lcd_blit(BUFF_BASE_ADDRESS, screen_type);
			/*dest = (unsigned int*)ALT_SCREEN_BASE_ADDRESS;
			src = (unsigned int*)BUFF_BASE_ADDRESS;
			for(i = 0; i < 160 * 240; i++)
				*dest++ = *src++;*/
		}
		else
		{
			dest = (unsigned int*)INV_BUFF;
			src = (unsigned int*)BUFF_BASE_ADDRESS;
			for(i = 0; i < 38400/*160 * 240*/; i++)
			{
				c = *src++;
				c = ~c;
				// c holds two 16-bits colors, decompose them while keeping them that way
				*dest++ = ((c & 0x1f) + (((c >> 5) & 0x3f) >> 1) + ((c >> 11) & 0x1f)) / 3
					+ ((((c >> 16) & 0x1f) + (((c >> 21) & 0x3f) >> 1) + ((c >> 27) & 0x1f)) / 3 << 16);
				
			}
			
			temp = *(void**)0xC0000010;
			*(void**)0xC0000010 = INV_BUFF;
			INV_BUFF = temp;
			swapped = !swapped;
		}
}

void deinitBuffering()
{
	// Handle monochrome screens-specific shit again
	if(is_classic)
		*((int32_t*)0xC000001C) = (*((int32_t*)0xC000001C) & ~0x0e) | 0x04;
	*(void**)(0xC0000010) = SCREEN_BACKUP;
	if(swapped)
	{
		temp = *(void**)0xC0000010;
		*(void**)0xC0000010 = INV_BUFF;
		INV_BUFF = temp;
	}
	free(INV_BUFF);
	free(ALT_SCREEN_BASE_ADDRESS);
	free(BUFF_BASE_ADDRESS);
}

/*                 *
 * Hardware timers *
 *                 */
// Everything on HW timers is by aeTIos and Streetwalrus from http://www.omnimaga.org/

#define TIMER 0x900D0000

/*         *
 *  Maths  *
 *         */

 /*
Example:
2.5 * 3.5 :
	xdec = 128
	ydec = 128
	xint = 2
	yint = 3
2.5 * 3 = 8.75 :
	rdec = 192
	rint = 8
*/
 

/*            *
 *  Graphics  *
 *            */

void clearBufferB()
{
	int i;
	for(i = 0; i < 38400; i++)
		((unsigned int*)BUFF_BASE_ADDRESS)[i] = 0;
}

inline unsigned short getPixel(const unsigned short *src, unsigned int x, unsigned int y)
{
	if(x < src[0] && y < src[1])
		return src[x + y * src[0] + 3];
	else
		return src[2];
}

inline void setPixelUnsafe(unsigned int x, unsigned int y, unsigned short c)
{
	switch(new_rev)
	{
		case 0:
			*((unsigned short*)BUFF_BASE_ADDRESS + x + y * 320) = c;
		break;
		case 1:
			*((unsigned short*)BUFF_BASE_ADDRESS + y + x * 240) = c;
		break;
	}
}


inline void setPixel(unsigned int x, unsigned int y, unsigned short c)
{
	if(x < 320 && y < 240)
	{
		switch(new_rev)
		{
			case 0:
				*((unsigned short*)BUFF_BASE_ADDRESS + x + y * 320) = c;
			break;
			case 1:
				*((unsigned short*)BUFF_BASE_ADDRESS + y + x * 240) = c;
			break;
		}
	}
}


void fillRect(int x, int y, int w, int h, unsigned short c)
{
	unsigned int _x = max(x, 0), _y = max(y, 0), _w = min(320 - _x, w - _x + x), _h = min(240 - _y, h - _y + y), i, j;
	if(_x < 320 && _y < 240)
	{
		for(j = _y; j < _y + _h; j++)
			for(i = _x; i < _x + _w; i++)
				setPixelUnsafe(i, j, c);
	}
}

void fillRectUnsafe(int x, int y, int w, int h, unsigned short c)
{
	unsigned int _x = max(x, 0), _y = max(y, 0), _w = min(320 - _x, w - _x + x), _h = min(240 - _y, h - _y + y), i, j;
	for(j = _y; j < _y + _h; j++)
		for(i = _x; i < _x + _w; i++)
			setPixelUnsafe(i, j, c);
}


/*        *
 *  Text  *
 *        */

int isOutlinePixel(unsigned char* charfont, int x, int y)
{
	int xis0 = !x, xis7 = x == 7, yis0 = !y, yis7 = y == 7;
	
	if(xis0)
	{
		if(yis0)
		{
			return !(*charfont & 0x80) && ((*charfont & 0x40) || (charfont[1] & 0x80) || (charfont[1] & 0x40));
		}
		else if(yis7)
		{
			return !(charfont[7] & 0x80) && ((charfont[7] & 0x40) || (charfont[6] & 0x80) || (charfont[6] & 0x40));
		}
		else
		{
			return !(charfont[y] & 0x80) && (
				(charfont[y - 1] & 0x80) || (charfont[y - 1] & 0x40) ||
				(charfont[y] & 0x40) ||
				(charfont[y + 1] & 0x80) || (charfont[y + 1] & 0x40));
		}
	}
	else if(xis7)
	{
		if(yis0)
		{
			return !(*charfont & 0x01) && ((*charfont & 0x02) || (charfont[1] & 0x01) || (charfont[1] & 0x02));
		}
		else if(yis7)
		{
			return !(charfont[7] & 0x01) && ((charfont[7] & 0x02) || (charfont[6] & 0x01) || (charfont[6] & 0x02));
		}
		else
		{
			return !(charfont[y] & 0x01) && (
				(charfont[y - 1] & 0x01) || (charfont[y - 1] & 0x02) ||
				(charfont[y] & 0x02) ||
				(charfont[y + 1] & 0x01) || (charfont[y + 1] & 0x02));
		}
	}
	else
	{
		char b = 1 << (7 - x);
		if(yis0)
		{
			return !(*charfont & b) && (
				(*charfont & (b << 1)) || (*charfont & (b >> 1)) ||
				(charfont[1] & (b << 1)) || (charfont[1] & b) || (charfont[1] & (b >> 1)));
		}
		else if(yis7)
		{
			return !(charfont[7] & b) && (
				(charfont[7] & (b << 1)) || (charfont[7] & (b >> 1)) ||
				(charfont[6] & (b << 1)) || (charfont[6] & b) || (charfont[6] & (b >> 1)));
		}
		else
		{
			return !(charfont[y] & b) && (
				(charfont[y] & (b << 1)) || (charfont[y] & (b >> 1)) ||
				(charfont[y - 1] & (b << 1)) || (charfont[y - 1] & b) || (charfont[y - 1] & (b >> 1)) ||
				(charfont[y + 1] & (b << 1)) || (charfont[y + 1] & b) || (charfont[y + 1] & (b >> 1)));
		}
	}
}


/*
void drawString(int *x, int *y, int _x, const char *str, unsigned short fc, unsigned short olc)
{
	int i, max = strlen(str) + 1;
	for(i = 0; i < max; i++)
		drawChar(x, y, _x, str[i], fc, olc);
}
*/

int numberWidth(int n)
{
	// Ints are in [-2147483648, 2147483647]
	int divisor = 10, result = 8;
	
	if(n < 0)
	{
		result += 8;
		n = -n;
	}
	
	while(1)
	{
		if(n < divisor)
			return result;
		result += 8;
		divisor *= 10;
	}
}

int stringWidth(const char* s)
{
	int i, result = 0, size = strlen(s);
	for(i = 0; i < size; i++)
	{
		if(s[i] >= 0x20)
			result += 8;
	}
	return result;
}

/*               *
 * Miscellaneous *
 *               */
/*
int get_key_pressed(t_key* report)
{
	unsigned short rowmap;
	int col, row;
	unsigned short *KEY_DATA = (unsigned short*)0x900E0010;
	int gotKey = 0;
	
	report->row = report->tpad_row = _KEY_DUMMY_ROW;
	report->col = report->tpad_col = _KEY_DUMMY_COL;
	report->tpad_arrow = TPAD_ARROW_NONE;

	// Touchpad and clickpad keyboards have different keymapping
	for(row = 0; row < 8; row++)
	{
		rowmap = has_colors ? KEY_DATA[row] : ~KEY_DATA[row];
		for(col = 1; col <= 0x400; col <<= 1)
		{
			if(rowmap & col)
			{
				gotKey = 1;
				break;
			}
		}
		if(gotKey) break;
	}
	if(gotKey)
	{
		row *= 2;
		row += 0x10;
		if(is_touchpad)
		{
			report->tpad_row = row;
			report->tpad_col = col;
		}
		else
		{
			report->row = row;
			report->col = col;
		}
		return 1;
	}
	else
		return 0;
}

inline int isKey(t_key k1, t_key k2)
{
	return is_touchpad ? (k1.tpad_arrow == k2.tpad_arrow ? k1.tpad_row == k2.tpad_row && k1.tpad_col == k2.tpad_col : 0 )
						: k1.row == k2.row && k1.col == k2.col;
}*/

#ifdef __cplusplus
}
#endif
