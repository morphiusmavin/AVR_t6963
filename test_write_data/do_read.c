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
static UCHAR filter_aux_states(UCHAR ch, int fd, WINDOW *win, int display_offset);
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
//		ch = filter_aux_states(ch, fd, win, display_offset);
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
	//			memset(new_global_number,0,NUM_ENTRY_SIZE);
				res = 0;
	//			res = write(fd,new_global_number,NUM_ENTRY_SIZE);
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
//*********************************** filter_aux_states ************************************//
//******************************************************************************************//
static UCHAR filter_aux_states(UCHAR ch, int fd, WINDOW *win, int display_offset)
{
	char tempx[20];
	UCHAR auxcmd = 0;
	UCHAR auxparam = 0;
	int res;
	int i;
	memset(tempx,0,sizeof(tempx));

	switch(ch)
	{
		case PRE_START:
		case START_AVR:
		case DATA_READY:
		case NEW_DATA_READY:
		case NONE:
			auxcmd = ch;
			break;
		default:
//			mvwprintw(win, display_offset+no_rtparams+3, 2,"none  %x  ",ch);
//			mvwprintw(win, display_offset+no_rtparams+4, 2,"            ");
			wrefresh(win);
			return ch;
			break;
	}
	disp_cmdtype(auxcmd, tempx);
	mvwprintw(win, display_offset+no_rtparams, 2,"                                                  ");
	mvwprintw(win, display_offset+no_rtparams, 2,"%s        ",tempx);

	disp_astate(aaux_state,tempx);
//			mvwprintw(win, display_offset+no_rtparams+1, 2, "auxcmd: %d auxparam %d  ",auxcmd,auxparam);
	mvwprintw(win, display_offset+no_rtparams, 25, "%s         ",tempx);
	wrefresh(win);
	switch(aaux_state)
	{
		case AVR_START:
			read(fd,&auxcmd,1);
			if(auxcmd == PRE_START)
			aaux_state = AVR_IDLE;
		case AVR_IDLE:
			// when a dialog is pulled up, ask_data_ready is set 
			// with aux_index pointing to what data to get
			if(ask_data_ready == 1)
			{
				auxcmd = START_AVR;
				write(fd,&auxcmd,1);
				write(fd,&aux_index,1);
				aaux_state = WAIT_DATA;
			}
			else
			{
				auxcmd = NONE;
				write(fd,&auxcmd,1);
				aaux_state = AVR_IDLE;	
			}	
			break;
		case WAIT_DATA:
			read(fd,&auxcmd,1);
			if(auxcmd == DATA_READY)
			{
				res = read(fd,&aux_data,AUX_DATA_SIZE);
				aaux_state = WAIT_NDREADY;
				for(i = 0;i < res;i++)
					mvwprintw(win, display_offset+no_rtparams+8, 2+(i*3),"%x",aux_data[i]);
			}
			else
			{
				aaux_state = WAIT_DATA;
			}
			break;
		case WAIT_NDREADY:
			// now that we have the data, wait for dialog to modify it
			if(new_data_ready == 1)
			{
				auxcmd = NEW_DATA_READY;
				write(fd,&auxcmd,1);
				write(fd,&aux_data,AUX_DATA_SIZE);
				aaux_state = AVR_IDLE;
			}
			else
			{
				auxcmd = NONE;
				write(fd,&auxcmd,1);
				aaux_state = WAIT_NDREADY;
			}
			break;
		default:
			aaux_state = AVR_IDLE;
			break;
	}


#if 0
PIC24_IDLE									AVR_IDLE
while (read(fd,&auxcmd,1) != START_AVR);	while(!ask_data_ready);
											auxcmd = START_AVR;
											write(fd,&auxcmd,1);
											auxparam = index;
											write(fd,&auxparam,1);
											goto <next state>
											WAIT_DATA
read(fd,&auxparam,1);
goto <next state>
P24_WAITGVAL
while(new_values == 0)
	goto <this state>
goto WRITE_VALUES
WRITE_VALUES
auxcmd = DATA_READY;
write(fd,&auxcmd,1);						while(auxcmd != DATA_READY)
												read(fd,&auxcmd,1);
											read(fd,&aux_data,AUX_DATA_SIZE);
											goto <next state>
											WAIT_NDREADY
											while(!new_data_ready);
											auxcmd = NEW_DATA_READY;
											write(fd,&auxcmd,1);			
goto WAIT_NEW_DATA


		case AVR_IDLE:
//			read(fd,&auxcmd,1);
			if(auxcmd == START_AVR)
				aaux_state = REQ_DATA;
			loop2 = 0;
			break;
		case REQ_DATA:
			auxcmd = PIC24_GET_DATA;			// PIC24 goes from P24_IDLE to GET_DATA
			auxparam = TYPE_1;
			res = write(fd, &auxcmd,1);
//			mvwprintw(win, display_offset+no_rtparams+6, 4,"%x res = %d %s ",auxcmd,res,strerror(errno));

			aaux_state = WAIT_DATA_READY;		// send data
//			aaux_state = AVR_IDLE;
			break;
		case WAIT_DATA_READY:
//			res = read(fd, &auxcmd, 1);
			if(auxcmd == PIC24_DATA_READY)		// wait for PIC24 to send PIC24_DATA_READY
			{
//				aaux_state = READ_DATA;
				aaux_state = AVR_WAIT_NEW_DATA;
				loop = 0;
			}
			else
			{
				aaux_state = WAIT_DATA_READY;
				loop = break_out_loop(loop,aaux_state);
			}
			break;
		case READ_DATA:
			usleep(5000);	// have to do a delay here - anything less than 5ms only reads 7 bytes
//			res = read(fd,&aux_data,AUX_DATA_SIZE);
//			mvwprintw(win, display_offset+no_rtparams+7, 2,"res = %d ",res);
//			for(i = 0;i < res;i++)
//				mvwprintw(win, display_offset+no_rtparams+8, 2+(i*3),"%x",aux_data[i]);
			aaux_state = AVR_WAIT_NEW_DATA;
			break;
		case AVR_WAIT_NEW_DATA:
			if(1)
//			if(++loop2 > 5)		// loop here to test the possibility that
			{						// the user takes a while to enter the new data
				loop2 = 0;
				auxcmd = AVR_HAS_NEW_DATA;
				write(fd,&auxcmd,1);
				aaux_state = SEND_NEW_DATA;
				aux_data[0] = ~aux_data[0];
				aux_data[1] = ~aux_data[1];
				aux_data[2] = ~aux_data[2];
				aux_data[3] = ~aux_data[3];
				aux_data[4] = ~aux_data[4];
				aux_data[5] = ~aux_data[5];
				aux_data[6] = ~aux_data[6];
				aux_data[7] = ~aux_data[7];
			}
			else
			{
				aaux_state = AVR_WAIT_NEW_DATA;
				auxcmd = AVR_HAS_NEW_DATA;
				write(fd,&auxcmd,1);	
			}
			break;
		case SEND_NEW_DATA:
//			res = write(fd,&aux_data,AUX_DATA_SIZE);
			memset(aux_data,0,AUX_DATA_SIZE);
			aaux_state = AVR_IDLE;
			break;
		default:
			aaux_state = AVR_IDLE;
			break;
	}	// end of switch
#endif	
//	mvwprintw(win, display_offset+no_rtparams+4, 2,"                  ");
//	wrefresh(win);
	return ch;
}
//******************************************************************************************//
//************************************* break_out_loop *************************************//
//******************************************************************************************//
static int break_out_loop(int loop,UCHAR curr_state)
{
	if(++loop > 10)
	{
		aaux_state = AVR_IDLE;
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
}

