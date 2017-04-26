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

static void disp_astate(UCHAR state, char *str);
static int break_out_loop(int loop,UCHAR curr_state);
static int loop;
static int loop2;

//******************************************************************************************//
//**************************************** do_read *****************************************//
//******************************************************************************************//
// do_read simulates the AVR
void do_read(WINDOW *win, int fd, int display_offset)
{
	int done;
	UCHAR ch;
	char param_string[10];
	UCHAR temp;
	UCHAR limit8 = 0;
	UINT limit16 = 0xf0;
	UINT limit16a = 0x70;
	int i,j;
	char test_str[20];
	int do_states = 1;
	int res;

//	set_win2(win);
	init_list();
	display_labels();
	wrefresh(win);

	for(i = 0;i < AUX_DATA_SIZE;i++)
		aux_data[i] = i;

	while(TRUE)
	{
		done = 0;
//		mvwprintw(win, 43, 5, "parse_state = %d  ",parse_state);
//		mvwprintw(win, 44, 5, "current_param = %d  ",current_param);
#if 0
		if(scale_type != prev_scale_type)
		{
			auxcmd = SWITCH_SCALE_TYPE;
			auxparam = scale_type;
			prev_scale_type = scale_type;
			if(scale_type == 0)
				do_states = 1;
			else
				do_states = 0;
		}
		write(fd,&auxcmd,1);
		write(fd,&auxparam,1);
		auxcmd = NONE;
//		if(do_states)
#endif

		read(fd,&ch,1);
		// get_key will filter out and process any keypresses in menus.c
		ch = get_key(ch);

		done = parse_P24(win, fd, ch, param_string);

//		mvwprintw(win, display_offset-1, 2,"done = %d %s ",done, param_string);
		wrefresh(win);
//		could just do this:
//		done = parse_P24(fd, get_key(read(fd,&ch,1)));s

		if(current_param >= no_rtparams)
		{
			mvwprintw(win, display_offset-2, 2, "error: %x   ",current_param);
			wrefresh(win);
			done = 0;
			set_defaults();
		}

		if(done)
		{
			if(current_param == RT_AUX)
			{
				res = 0;
				write(fd,aux_data,AUX_DATA_SIZE);
				for(i = 0;i < AUX_DATA_SIZE;i++)
					aux_data[i]++;
#if 0	
				limit8 = (UCHAR)(limit16 >> 8);
				res = write(fd,&limit8,1);
				limit8 = (UCHAR)limit16;
				res = write(fd,&limit8,1);
				limit16++;
				limit8 = (UCHAR)(limit16 >> 8);
				res = write(fd,&limit8,1);
				limit8 = (UCHAR)limit16;
				res = write(fd,&limit8,1);
				mvwprintw(win, display_offset+no_rtparams+4, 2,"%d  ",limit16);

				limit8 = (UCHAR)(limit16a >> 8);
				res = write(fd,&limit8,1);
				limit8 = (UCHAR)limit16a;
				res = write(fd,&limit8,1);
				limit16a++;
				limit8 = (UCHAR)(limit16a >> 8);
				res = write(fd,&limit8,1);
				limit8 = (UCHAR)limit16a;
				res = write(fd,&limit8,1);
				mvwprintw(win, display_offset+no_rtparams+5, 2,"%d  ",limit16a);
#endif
			}
			mvwprintw(win, display_offset-1, 2, "current param: %d  %s  ",current_param,param_string);
			mvwprintw(win, display_offset-2, 2, "                      ");
			mvwprintw(win, display_offset+current_param, 15, "        ");
			mvwprintw(win, display_offset+current_param, 15, param_string);
//			mvwprintw(win, display_offset+16, 5, "param_string = %s  ",param_string);
			wrefresh(win);
// this displays the RT params on the screen at their positions according to the rt_params struct
//			mvwprintw(win, display_offset+17,5,"%d ",no_rtparams);
			for(i = 0;i < no_rtparams;i++)
			{

//				mvwprintw(win, display_offset+18+i,5,"rt_params: %d  %d  %d  %d  ", \
//				rt_params[i].row,rt_params[i].col, rt_params[i].shown,rt_params[i].type);

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
//******************************************************************************************//
//************************************* break_out_loop *************************************//
//******************************************************************************************//
static int break_out_loop(int loop,UCHAR curr_state)
{
	if(++loop > 10)
	{
//		aaux_state = AVR_IDLE;
		return 0;
	}
	else
	{
		aaux_state = curr_state;
		return loop;
	}
}
//******************************************************************************************//
//************************************** disp_astate ***************************************//
//******************************************************************************************//
static void disp_astate(UCHAR state, char *str)
{
/*
	switch (state)
	{
		case AVR_START:
		strcpy(str,"AVR_START\0");
		break;
		case AVR_IDLE:
		strcpy(str,"AVR_IDLE\0");
		break;
		case WAIT_DATA:
		strcpy(str,"WAIT_DATA\0");
		break;
		case WAIT_NDREADY:
		strcpy(str,"WAIT_NDREADY\0");
		break;
		default:
		strcpy(str,"<bad state>\0");
		break;
	}
*/
}

