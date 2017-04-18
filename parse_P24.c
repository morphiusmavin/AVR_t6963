#ifdef MAIN_C
#include <avr/io.h>
#include "../avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include "sfr_helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#include "sfr_helper.h"
#include "main.h"
#include "t6963.h"

//******************************************************************************************//
//************************************** parse_P24 *****************************************//
//******************************************************************************************//
#ifdef MAIN_C
int parse_P24(UCHAR ch, char *param_string, UCHAR *pxbyte, UINT *pxword)
#else
int parse_P24(int fd, UCHAR ch, char *param_string)
#endif
{
	int done = 0;
	UCHAR xbyte;
	UINT xword;

	switch(parse_state)
	{
		case IDLE:
			if(ch <= RT_O2 && ch >= RT_RPM)
			{
				current_param = ch;
				parse_state = CHECK_HIGHBIT;
			}
// 
// if PIC24 sends an RT_AUX command then
// 
			else if(ch == RT_AUX)
			{

				ch = get_str_len();
#ifndef MAIN_C
				write(fd,&ch,1);
				write(fd,new_global_number,NUM_ENTRY_SIZE);
#endif
				parse_state = IDLE;

			}
			else
			{
//						printf("opps\n");
				set_defaults();
			}
			break;
		case CHECK_HIGHBIT:
			switch(ch)
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
			break;
		case GET_CH0:
			parse_state = SEND_UINT0;
			temp_UINT = ch;
			break;
		case GET_CH1:
			temp_UINT = ch;
			parse_state = SEND_UINT1;
			break;
		case GET_CH2:
			temp_UINT = ch;
			parse_state = SEND_UINT2;
			break;
		case SEND_UCHAR0:
			xbyte = ch;
			sprintf(param_string,"%4d",xbyte);
//				printf("uchar0:%s\n",param_string);
			done = 1;
			break;
		case SEND_UCHAR1:
			xbyte = ch | 0x80;
			sprintf(param_string,"%4d",xbyte);
//				printf("uchar1:%s\n",param_string);
			done = 1;
			break;
		case SEND_UINT0:
			xword = (UINT)temp_UINT;
			temp_UINT = (UINT)ch;
			temp_UINT <<= 8;
			temp_UINT &= 0xff00;
			xword |= temp_UINT;
			sprintf(param_string,"%4u",xword);
//				printf("uint0:%s\n",param_string);

			done = 1;
			break;
		case SEND_UINT1:
			xword = (UINT)temp_UINT;
			temp_UINT = (UINT)ch;
			temp_UINT <<= 8;
			temp_UINT &= 0xff00;
			xword |= temp_UINT;
			xword |= 0x0080;
			sprintf(param_string,"%4u",xword);
//				printf("uint1:%s\n",param_string);
			done = 1;
			break;
		case SEND_UINT2:
			xword = (UINT)temp_UINT;
			temp_UINT = (UINT)ch;
			temp_UINT <<= 8;
			temp_UINT &= 0xff00;
			xword |= temp_UINT;
			xword |= 0x8000;
			sprintf(param_string,"%4u",xword);
//				printf("uint2:%s\n",param_string);
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

