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
static void disp_parse_state(UCHAR state, char *str);
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
	UINT limit16 = 0;
	UINT limit16a = 1;
	int tempint;
	int i,j;
	char test_str[10];
	int do_states = 1;
	int res;
	char tempx[20];
	int error_code = 0;

//	set_win2(win);
	init_list();
	display_labels();
	wrefresh(win);

	for(i = 0;i < AUX_DATA_SIZE;i++)
		aux_data[i] = i+10;

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
		// ignore any chars with highbit set - they are processed by get_key()
		if((ch & 0x80) == 0)
		{
			done = parse_P24(win, fd, ch, param_string);

	//		could just do this:
	//		done = parse_P24(fd, get_key(read(fd,&ch,1)));s

	//		if(current_param >= no_rtparams && parse_state != CHECK_HIGHBIT)
			if(current_param-RT_RPM >= no_rtparams)
			{
				mvwprintw(win, DISP_OFFSET+32, 2+(2*error_code), "%x ",current_param-RT_RPM);
				disp_parse_state(parse_state,tempx);
				mvwprintw(win, DISP_OFFSET+33, 2,"%s       ",tempx);
				if(++error_code > 25)
				{
					mvwprintw(win, DISP_OFFSET+32, 2,"                                                    ");
					error_code = 0;
				}
				wrefresh(win);
//				done = 0;
//				set_defaults();
			}

			if(done)
			{
//#if 0
				if(current_param == RT_AUX1)
				{
					res = 0;

					switch(aaux_state)
					{
						// menu choice sets aux_index to something other than 0
						// when it wants certain data from PIC24
						case IDLE_AUX:
							if(aux_index != 0)
							{
								aux_data[0] = CMD_GET_DATA;
								aaux_state = DATA_REQ;
								aux_index = 0;
							}
							else aaux_state = IDLE_AUX;
							break;
						// wait for PIC24 to get data	
						case DATA_REQ:
							if(auxcmd == CMD_DATA_READY)
							{
								aaux_state = VALID_DATA;
								mvwprintw(win, DISP_OFFSET+34,2,"             ");
							}	
							else
							{
								aaux_state = DATA_REQ;
								loop = break_out_loop(loop,aaux_state);
								mvwprintw(win, DISP_OFFSET+34,2,"loop: %d ",loop);
							}
							break;
						// read data into buffer	
						case VALID_DATA:
							aux_data[0] = CMD_NEW_DATA;
							aaux_state = DATA_READY;
							break;
						// data has been modified by AVR and is ready to send back to PIC24	
						case DATA_READY:
							aux_data[0] = (UCHAR)(limit16 >> 8);
							aux_data[1] = (UCHAR)limit16;
							limit16++;
							aux_data[2] = (UCHAR)(limit16a >> 8);
							aux_data[3] = (UCHAR)limit16a;
							limit16a += 10;
							aaux_state = IDLE_AUX;
							break;
						default:
							aaux_state = IDLE_AUX;
							break;
					}
					disp_astate(aaux_state,tempx);
					mvwprintw(win, DISP_OFFSET+30, 2,"%s     ",tempx);
					mvwprintw(win, DISP_OFFSET+31, 2, \
					"aux_index: %d cmd: %d  param: %d    ",aux_index,auxcmd,auxparam);
				}
				else if (current_param == RT_AUX2)
//#endif
//				if(current_param == RT_AUX2)
				{
					tempint = atoi(param_string);
					auxcmd = (UCHAR)((tempint >> 8) & 0x00ff);
					auxparam = (UCHAR)(tempint & 0x00ff);
//					mvwprintw(win, DISP_OFFSET+31, 2,"cmd: %x  param: %x    ",auxcmd,auxparam);					
//					for(i = 0;i < AUX_DATA_SIZE;i++)
//						aux_data[i]++;
					write(fd,aux_data,AUX_DATA_SIZE);
				}
//				mvwprintw(win, DISP_OFFSET+31, 2,"cmd: %x  param: %x    ",auxcmd,auxparam);
				mvwprintw(win, display_offset-1, 2, "current param: %x  %s  ",  \
					current_param-RT_RPM,param_string);
				mvwprintw(win, display_offset+current_param-RT_RPM, 15, "        ");
				mvwprintw(win, display_offset+current_param-RT_RPM, 15, param_string);
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
			}
		}	// end of done
	}	// end of while(1)
}
//******************************************************************************************//
//************************************* break_out_loop *************************************//
//******************************************************************************************//
static int break_out_loop(int loop,UCHAR curr_state)
{
	if(++loop > 5)
	{
		aaux_state = IDLE_AUX;
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
	switch (state)
	{
		case IDLE_AUX:
			strcpy(str,"IDLE_AUX\0");
			break;
		case DATA_REQ:
			strcpy(str,"DATA_REQ\0");
			break;
		case VALID_DATA:
			strcpy(str,"VALID_DATA\0");
			break;
		case DATA_READY:
			strcpy(str,"DATA_READY\0");
			break;
		case EXTRA:
			strcpy(str,"EXTRA\0");
			break;
		default:
		strcpy(str,"<bad state>\0");
		break;
	}
}
//******************************************************************************************//
//************************************** disp_astate ***************************************//
//******************************************************************************************//
static void disp_parse_state(UCHAR state, char *str)
{
	switch (state)
	{
		case IDLE:
			strcpy(str,"IDLE\0");
			break;
		case CHECK_HIGHBIT:
			strcpy(str,"CHECK_HIGHBIT\0");
			break;
		case GET_CH0:
			strcpy(str,"GET_CH0\0");
			break;
		case GET_CH1:
			strcpy(str,"GET_CH1\0");
			break;
		case GET_CH2:
			strcpy(str,"GET_CH2\0");
			break;
		case SEND_UCHAR0:
			strcpy(str,"SEND_UCHAR0\0");
			break;
		case SEND_UCHAR1:
			strcpy(str,"SEND_UCHAR1\0");
			break;
		case SEND_UINT0:
			strcpy(str,"SEND_UINT0\0");
			break;
		case SEND_UINT1:
			strcpy(str,"SEND_UINT1\0");
			break;
		case SEND_UINT2:
			strcpy(str,"SEND_UINT2\0");
			break;
		default:
		strcpy(str,"<bad state>\0");
		break;
	}
}


