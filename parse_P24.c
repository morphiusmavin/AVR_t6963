#ifdef MAIN_C
#include <avr/io.h>
#include "../avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include "sfr_helper.h"
#else
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/time.h>
#include <ncurses.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sfr_helper.h"
#include "main.h"
#include "t6963.h"

//******************************************************************************************//
//************************************** parse_P24 *****************************************//
//******************************************************************************************//
// linked into and called by both main.c and test_write_data.c
#ifdef MAIN_C
int parse_P24(UCHAR inch, char *param_string, UCHAR *pxbyte, UINT *pxword)
#else
int parse_P24(WINDOW *win, int fd, UCHAR inch, char *param_string)
#endif
{
	int done = 0;
	UCHAR xbyte;
	UINT xword;

//	mvwprintw(win, 34, 2,"parse state = %d ch = %d ",parse_state,inch);
	switch(parse_state)
	{
		case IDLE:
			if((inch & 0x70) == 0x70)
			{
				current_param = inch;
				parse_state = CHECK_HIGHBIT;
//				mvwprintw(win, DISP_OFFSET+29,2,"             ");
			}	
			else
			{
				set_defaults();
//				mvwprintw(win, DISP_OFFSET+29,2,"reset: %x  ",inch);
			}
			break;
		case CHECK_HIGHBIT:
			switch(inch)
			{
				case RT_LOW:
					parse_state = SEND_UCHAR0;
					break;
				case RT_HIGH0:		// if just a UCHAR is sent with high bit set
					parse_state = SEND_UCHAR1;
					break;
				case RT_HIGH1:		// if UINT with neither bit 7 or 15 set
					parse_state = GET_CH0;
					break;
				case RT_HIGH2:		// if a UINT is sent with bit 7 set
					parse_state = GET_CH1;
					break;
				case RT_HIGH3:		// if a UINT is sent with bit 15 set
					parse_state = GET_CH2;
					break;
				default:
					set_defaults();
					break;
			}
//			mvwprintw(win, DISP_OFFSET+32,2,"%d  %d",inch,current_param);
			break;
		case GET_CH0:
			parse_state = SEND_UINT0;
			temp_UINT = inch;
			break;
		case GET_CH1:
			temp_UINT = inch;
			parse_state = SEND_UINT1;
			break;
		case GET_CH2:
			temp_UINT = inch;
			parse_state = SEND_UINT2;
//			mvwprintw(win, DISP_OFFSET+33,2,"%d  %d",inch,current_param);
			break;
		case SEND_UCHAR0:
			xbyte = inch;
			sprintf(param_string,"%4d",xbyte);
//				printf("uchar0:%s\n",param_string);
			done = 1;
			break;
		case SEND_UCHAR1:
			xbyte = inch | 0x80;
			sprintf(param_string,"%4d",xbyte);
//				printf("uchar1:%s\n",param_string);
			done = 1;
			break;
		case SEND_UINT0:
			xword = (UINT)temp_UINT;
			temp_UINT = (UINT)inch;
			temp_UINT <<= 8;
			temp_UINT &= 0xff00;
			xword |= temp_UINT;
			sprintf(param_string,"%4u",xword);
//				printf("uint0:%s\n",param_string);

			done = 1;
			break;
		case SEND_UINT1:
			xword = (UINT)temp_UINT;
			temp_UINT = (UINT)inch;
			temp_UINT <<= 8;
			temp_UINT &= 0xff00;
			xword |= temp_UINT;
			xword |= 0x0080;
			sprintf(param_string,"%4u",xword);
			done = 1;
			break;
		case SEND_UINT2:
			xword = (UINT)temp_UINT;
			temp_UINT = (UINT)inch;
//			mvwprintw(win, DISP_OFFSET+34,2,"%d  %d",inch,current_param);
			temp_UINT <<= 8;
			temp_UINT &= 0xff00;
			xword |= temp_UINT;
			xword |= 0x8000;
			sprintf(param_string,"%4u",xword);
			done = 1;
			break;
		default:
//				printf("%s\n","default ");
			set_defaults();
			break;
	}	// end of inner switch
#ifdef MAIN_C
	*pxbyte = &xbyte;
	*pxword = &xword;
#endif
	return done;
}

