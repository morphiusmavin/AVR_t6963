// do_read.c - called from test_write_data.c
// this simulates the AVR reading from the PIC24
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
void do_read(WINDOW *win, int fd, int display_offset)
{
	int done;
	int res;
	UCHAR ch;
	char param_string[10];
	UCHAR temp;
	UCHAR limit8;
	UINT limit16;
	int i,j;
	char test_str[20];
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
//		mvwprintw(win, display_offset+14, 5, "parse_state = %d  ",parse_state);
//		mvwprintw(win, display_offset+15, 5, "current_param = %d  ",current_param);
//		wrefresh(win);
		done = parse_P24(win, fd, ch, param_string);
//		could just do this:
//		done = parse_P24(fd, get_key(read(fd,&ch,1)));
		if(done)
		{
			mvwprintw(win, display_offset-1, 3, "current param: %d  %s  ",current_param,param_string);
			mvwprintw(win, display_offset+current_param, 15, "        ");
			mvwprintw(win, display_offset+current_param, 15, param_string);
//			mvwprintw(win, display_offset+16, 5, "param_string = %s  ",param_string);
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
// this displays the RT params on the screen at their positions according to the rt_params struct
//			mvwprintw(win, display_offset+17,5,"%d ",no_rtparams);
			for(i = 0;i < no_rtparams;i++)
			{
/*
				mvwprintw(win, display_offset+18+i,5,"rt_params: %d  %d  %d  %d  ", \
				rt_params[i].row,rt_params[i].col, rt_params[i].shown,rt_params[i].type);
*/
				if(rt_params[i].shown == 1)
				{
					if(rt_params[i].type == current_param)
					{
						GDispStringAt(rt_params[i].row,rt_params[i].col+9,"      ");
					}

					if(rt_params[i].shown && rt_params[i].type == current_param)
					{
						GDispStringAt(rt_params[i].row,rt_params[i].col+10,param_string);
					}
				}
//				mvwprintw(win, display_offset+30, 2,"row: %d col: %d  ",rt_params[i].row,rt_params[i].col);
			}

			set_defaults();
		}	// end of done
	}	// end of while(1)
}


