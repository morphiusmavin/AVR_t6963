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
static void disp_auxcmd(UCHAR state, char *str);
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
	UINT tempint;
	UINT tempint2;
	UINT tempint3 = 0;
	int i,j;
	char test_str[10];
	int do_states = 1;
	int res;
	char tempx[20];
	char tempnum[NUM_ENTRY_SIZE];
	int error_code = 0;
	UCHAR aux_data[AUX_DATA_SIZE];
	UCHAR aux_data2[AUX_DATA_SIZE];
	UCHAR auxcmd, auxparam;
	UCHAR taux_index = 1;

//	set_win2(win);
	init_list();
	display_labels();
	wrefresh(win);
	memset(aux_data,0,AUX_DATA_SIZE);
	memset(aux_data2,0,AUX_DATA_SIZE);

	for(i = 0;i < AUX_DATA_SIZE;i++)
	{
		aux_data[i]++;
		aux_data2[i]++;
	}
	for(i = 0;i < AUX_DATA_SIZE;i++)
		aux_data[i]++;

	while(TRUE)
	{
		done = 0;
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
#endif

		read(fd,&ch,1);
		// get_key will filter out and process any keypresses in menus.c
		ch = get_key(ch);
		// ignore any chars with highbit set - they are processed by get_key()
//		mvwprintw(win, DISP_OFFSET+28, 2,"%x ",ch);
		if((ch & 0x80) == 0)
		{
			done = parse_P24(win, fd, ch, param_string);

			if(current_param-RT_RPM >= no_rtparams)
			{
				mvwprintw(win, DISP_OFFSET+32, 2+(2*error_code), "%x ",current_param-RT_RPM);
				if(++error_code > 20)
				{
					mvwprintw(win, DISP_OFFSET+32, 2,"                                                    ");
					error_code = 0;
				}
				wrefresh(win);
				done = 0;
				set_defaults();
			}

			if(done)
			{
				if(current_param == RT_AUX1)
				{
					res = 0;
					tempint = atol(param_string);
					auxparam = (UCHAR)tempint;
					tempint >>= 8;
					auxcmd = (UCHAR)tempint;
/*
					mvwprintw(win, DISP_OFFSET+27, 2,"cmd: %x  param: %x  ",auxcmd,auxparam);
					mvwprintw(win, DISP_OFFSET+35, 2,"mod_data_ready: %d  ",mod_data_ready);
					mvwprintw(win, DISP_OFFSET+36, 2,"new_data_ready: %d  ",new_data_ready);
*/
					switch(aaux_state)
					{
						// menu choice sets aux_index to something other than 0
						// when it wants certain data from PIC24
						case IDLE_AUX:
							mod_data_ready = 0;
							if(aux_index != 0)
							{
								aux_data[0] = CMD_GET_DATA;
								aux_data[1] = aux_index-1;
								aaux_state = DATA_REQ;
							}
							else aaux_state = IDLE_AUX;
							break;
						// wait for PIC24 to get data	
						case DATA_REQ:
							aux_index = 0;
							if(auxcmd == CMD_DATA_READY)	// new data should be in tempint2
							{
								aaux_state = VALID_DATA;
								sprintf(cur_global_number,"%-6u",tempint2);
								i = 0;
								j = 0;
								do
								{
									i++;
								}while(cur_global_number[i] != 0x20);
								cur_global_number[i] = 0;
/*
								strncpy(tempnum,cur_global_number+i,NUM_ENTRY_SIZE-i);
								mvwprintw(win, DISP_OFFSET+24,2,"tempnum: %s   %d",tempnum,i);
								memset(cur_global_number,0,NUM_ENTRY_SIZE);
								strcpy(cur_global_number,tempnum);
								cur_global_number[NUM_ENTRY_SIZE-i] = 0;
								mvwprintw(win, DISP_OFFSET+25,2,"cur:     %sx     ",cur_global_number);
								mvwprintw(win, DISP_OFFSET+26,2,"%sx   %d   ",cur_global_number,tempint2);
*/
								new_data_ready = 1;
								loop = 0;
							}	
							else
							{
								aaux_state = DATA_REQ;
//								loop = break_out_loop(loop,aaux_state);
//								mvwprintw(win, DISP_OFFSET+28,2,"loop: %d ",loop);
							}
							break;
						// read data into buffer and waits for user to modify it (or cancel)	
						case VALID_DATA:
//							mvwprintw(win, DISP_OFFSET+28,2,"             ");
//							mvwprintw(win, DISP_OFFSET+35, 2,"mod_data_ready: %d  ",mod_data_ready);
							if(mod_data_ready == 1)
							{
								aaux_state = DATA_READY;
								loop = 0;
							}
							else
							{
//								loop = break_out_loop(loop,aaux_state);
//								mvwprintw(win, DISP_OFFSET+28,2,"loop: %d ",loop);
								aaux_state = VALID_DATA;
							}	
							break;
						// data has been modified by AVR and is ready to send back to PIC24	
						case DATA_READY:
							tempint2 = atol(new_global_number);
							aux_data[0] = CMD_NEW_DATA;
							aaux_state = DATA_READY;
							aux_data2[0] = (UCHAR)(tempint2 >> 8);
							aux_data2[1] = (UCHAR)tempint2;
							limit16++;
//							aux_data2[2] = (UCHAR)(limit16a >> 8);
//							aux_data2[3] = (UCHAR)limit16a;
							aux_data2[2] = aux_data[0];
							aux_data2[3] = aux_data[1];
							limit16a += 2;
							mvwprintw(win, DISP_OFFSET+29, 2,"%d   %d   ",limit16,limit16a);
//							if(++taux_index > no_menu_labels)
//								taux_index = 1;
							mvwprintw(win, DISP_OFFSET+32, 2,"                                                    ");
							aaux_state = IDLE_AUX;
							break;
						default:
							aaux_state = IDLE_AUX;
							break;
					}
					mvwprintw(win, DISP_OFFSET+30, 2,"aux_index: %x  ",taux_index);
					disp_astate(aaux_state,tempx);
					mvwprintw(win, DISP_OFFSET+31, 2,"%s     ",tempx);
					disp_auxcmd(aux_data[0],tempx);
					mvwprintw(win, DISP_OFFSET+31, 20,"%s     ",tempx);

					for(i = 0;i < AUX_DATA_SIZE;i++)
						mvwprintw(win, display_offset+32, 2+(i * 3),"%x  ",aux_data[i]);
//					aux_data[2] = (UCHAR)(tempint3 >> 8);
//					aux_data[3] = (UCHAR)tempint3;
					aux_data[2] = aux_data[3] = 0;
					tempint3++;
					write(fd,aux_data,AUX_DATA_SIZE);

				}
				else if (current_param == RT_AUX2)
				{
					if(aaux_state == DATA_REQ)
					{
						tempint2 = atol(param_string);
						strcpy(param_string,cur_global_number);
						mvwprintw(win, DISP_OFFSET+34, 2,"tempint2: %d  ",tempint2);
					}
					else tempint2 = 0;
//					for(i = 0;i < AUX_DATA_SIZE;i++)
//						aux_data2[i]++;
					for(i = 0;i < AUX_DATA_SIZE;i++)
						mvwprintw(win, display_offset+33, 2+(i * 3),"%x  ",aux_data2[i]);
					write(fd,aux_data2,AUX_DATA_SIZE);

				}
				for(i = 0;i < NUM_ENTRY_SIZE;i++)
					mvwprintw(win, DISP_OFFSET+35, 2+i,"%c",cur_global_number[i]);

				if(rt_params[current_param-RT_RPM].shown == SHOWN_SENT)
				{
					mvwprintw(win, display_offset-1, 2, "current param: %x  %s  ",current_param-RT_RPM,param_string);
					mvwprintw(win, display_offset+current_param-RT_RPM, 15, "     ");
					mvwprintw(win, display_offset+current_param-RT_RPM, 15, param_string);
				}	
				wrefresh(win);
	// this displays the RT params on the screen at their positions according to the rt_params struct
	//			mvwprintw(win, display_offset+17,5,"%d ",no_rtparams);
				for(i = 0;i < no_rtparams;i++)
				{
	//				mvwprintw(win, display_offset+18+i,5,"rt_params: %d  %d  %d  %d  ", \
	//				rt_params[i].row,rt_params[i].col, rt_params[i].shown,rt_params[i].type);
					if(rt_params[i].shown == SHOWN_SENT)
					{
						if(rt_params[i].type == current_param)
						{
							GDispStringAt(rt_params[i].row,rt_params[i].col+9,"      ");
						}

						if(rt_params[i].type == current_param && (rt_params[i].shown == SHOWN_SENT))
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
//************************************** disp_auxcmd ***************************************//
//******************************************************************************************//
static void disp_auxcmd(UCHAR state, char *str)
{
	switch (state)
	{
		case CMD_GET_DATA:
			strcpy(str,"CMD_GET_DATA\0");
			break;
		case CMD_DATA_READY:
			strcpy(str,"CMD_DATA_READY\0");
			break;
		case CMD_NEW_DATA:
			strcpy(str,"CMD_NEW_DATA\0");
			break;
		case CMD_EXTRA:
			strcpy(str,"CMD_EXTRA\0");
			break;
		default:
		strcpy(str,"<bad state>\0");
		break;
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


