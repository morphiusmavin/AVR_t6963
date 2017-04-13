// do_read.c - called from test_write_data.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/time.h>
#include <ncurses.h>
#include "../sfr_helper.h"
#include "../main.h"
#include "../t6963.h"

//******************************************************************************************//
//**************************************** do_read *****************************************//
//******************************************************************************************//
// do_read simulates the AVR
int do_read(WINDOW *win, int fd, int display_offset)
{
	int done;
	int res;
	UCHAR ch;
	UCHAR xbyte;
	char param_string[10];
	UINT xword;
	UCHAR txword;
	UCHAR temp;
	UCHAR limit8;
	UINT limit16;
	int i,j;
	char test_str[20];
	int ret_code = 0;

	init_list();
	display_labels();
	wrefresh(win);

#ifdef NOAVR
	set_win2(win);
#endif

	while(TRUE)
	{
		done = 0;
		res = read(fd,&ch,1);
		ch = get_key(ch);
		mvwprintw(win, display_offset+14, 5, "parse_state = %d  ",parse_state);
		mvwprintw(win, display_offset+15, 5, "current_param = %d  ",current_param);
		wrefresh(win);
		switch(parse_state)
		{
			case IDLE:
				if(ch <= RT_MPH && ch >= RT_RPM)
				{
					current_param = ch;
					parse_state = CHECK_HIGHBIT;
				}
				else if(ch == RT_AUX)
				{
/*
					ch = get_str_len();
					write(fd,&ch,1);
					write(fd,new_global_number,NUM_ENTRY_SIZE);
					parse_state = IDLE;
*/
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
						ret_code = 2;
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
				ret_code = 1;
				break;
		}	// end of inner switch
		if(done)
		{
			mvwprintw(win, display_offset+current_param, 15, "        ");
			mvwprintw(win, display_offset+current_param, 15, param_string);
			mvwprintw(win, display_offset+16, 5, "param_string = %s  ",param_string);
			wrefresh(win);
/*
			if(current_param == RT_RPM)
			{
				txword = (UCHAR)(xword>>8);
				write(fd,&txword,1);
				txword = (UCHAR)(xword);
				write(fd,&txword,1);
			}
			else
				write(fd,&xbyte,1);
*/
// this displays the RT params on the screen at their positions according to the prompts struct
//			mvwprintw(win, display_offset+17,5,"%d ",no_rtparams);
			for(i = 0;i < no_rtparams;i++)
			{
//				mvwprintw(win, display_offset+18+i,5,"rt_params: %d  %d  %d  %d  ",rt_params[i].row,rt_params[i].col, \
						rt_params[i].shown,rt_params[i].type);

				if(rt_params[i].type == current_param)
				{
					GDispStringAt(rt_params[i].row,rt_params[i].col+9,"      ");
				}

				if(rt_params[i].shown && rt_params[i].type == current_param)
				{
					GDispStringAt(rt_params[i].row,rt_params[i].col+10,param_string);
				}

			}

			set_defaults();
//			mvwprintw(win, display_offset+22, 5,"cursor row: %d cursor col: %d  ",cursor_row,cursor_col);
		}	// end of done
	}	// end of while(1)
	return ret_code;
}

