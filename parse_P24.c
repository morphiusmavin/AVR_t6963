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
                           
static void disp_astate(UCHAR state, char *str);
static int break_out_loop(int loop,UCHAR curr_state);
static int loop;
static int loop2;
			
//******************************************************************************************//
//************************************** parse_P24 *****************************************//
//******************************************************************************************//
#ifdef MAIN_C
int parse_P24(UCHAR inch, char *param_string, UCHAR *pxbyte, UINT *pxword)
#else
int parse_P24(WINDOW *win, int fd, UCHAR inch, char *param_string)
#endif
{
	int done = 0;
	UCHAR xbyte;
	UINT xword;
	int i,j;
	UCHAR tempch;
	char tempx[20];
	UCHAR auxcmd;
	UCHAR auxparam;
	int res;

	switch(parse_state)
	{
		case IDLE:
			if(inch <= RT_O2 && inch >= RT_RPM)
			{
				current_param = inch;
				parse_state = CHECK_HIGHBIT;
			}
/*
1) AVR sends a REQ_DATA command along with param to tell what value it wants (if parse_P24 is done)
2) PIC24 reads the byte - if 0 then doesn't do anything; if > 0 then it goes out and gets the value(s)
3) on the next round, the AVR reads a byte sent by the PIC24 saying the data is ready
4) if its not ready, go another round (or go so many rounds and then send another command)
5) AVR reads the ready flag
6) AVR then reads data
7) AVR sets a flag showing value has been received
8) AVR will put value in checkbox dialog (if its a binary) or in num entry dialog (if UCHAR or UINT) (?word)
9) AVR will send a command telling the PIC24 that it can read the changed value(s) back
10) AVR will send data

each one of the following states is one whole cycle in the rt_params

enum PIC24_aux_states
{
	P24_IDLE = 1,
	GET_DATA,			// when PIC24 gets a REQ_DATA then go out and get it
	SEND_DATA_READY,	// tell AVR data is ready to send
	SEND_DATA,			// send data (how much is predefined)
	WAIT_ACQ,			// wait for AVR to say valid data was received (if NAQ then go to prev state)
	P24_WAIT_NEW_DATA,		// wait for AVR to send the updated value(s)
} PIC24_AUX;

enum AVR_aux_states
{
	AVR_IDLE = 1,
	REQ_DATA,			//  when menu choice is get certain data (this one starts everything)
	WAIT_DATA_READY,	// wait for SEND_DATA_READY from PIC24 (keep making rounds until data is ready to read)
	READ_DATA,			// read data (how much is predefined)
	AVR_WAIT_NEW_DATA,		// wait for stupid user to finish editing new data
	SEND_NEW_DATA
} AVR_AUX;
*/
			else if(inch == RT_AUX)
			{

#ifdef MAIN_C
				transmitByte(aux_type);
#else
//				memset(aux_data,0x20,AUX_DATA_SIZE);
				disp_astate(aaux_state,tempx);
				mvwprintw(win, DISP_OFFSET+24, 2, "state: %d %s          ",aaux_state, tempx);
				switch(aaux_state)
				{
					case AVR_IDLE:
						auxcmd = KEEP_IDLE;
						write(fd, &auxcmd,1);
						aaux_state = REQ_DATA;
						loop2 = 0;
						break;
					case REQ_DATA:
						auxcmd = PIC24_GET_DATA;
						auxparam = TYPE_1;
						write(fd, &auxcmd,1);
//						write(fd, &auxparam,1);	// tell the PIC24 what kind of data we want to modify
						aaux_state = WAIT_DATA_READY;
						break;
					case WAIT_DATA_READY:
						read(fd, &auxcmd, 1);
						if(auxcmd == PIC24_DATA_READY)
						{
							mvwprintw(win, DISP_OFFSET+19, 2, "auxcmd: %d  PIC24_DATA_READY",auxcmd);
//							auxcmd = AVR_RTC;
//							write(fd,&auxcmd,1);
							aaux_state = READ_DATA;
							loop = 0;
						}
						else
						{
							aaux_state = WAIT_DATA_READY;
							mvwprintw(win, DISP_OFFSET+19, 2, "loop = %d                    ",loop);
							loop = break_out_loop(loop,aaux_state);
						}
						break;
					case READ_DATA:
						mvwprintw(win, DISP_OFFSET+19, 2, "auxcmd: %d                  ",auxcmd);
//						usleep(10000);	// this works
						usleep(5000);	// so does this
//						usleep(3000);	// anything less than 5000 causes: res = 7 or 1
										// unless time delay param for write is set to 10
						res = read(fd,&aux_data,AUX_DATA_SIZE);
						mvwprintw(win, DISP_OFFSET+20, 2, "read: %d   ",res);
//						for(i = 0;i < AUX_DATA_SIZE;i++)
						for(i = 0;i < res;i++)
							mvwprintw(win, DISP_OFFSET+21, 2+i*3, "%x",aux_data[i]);
//						aaux_state = SEND_ACQ;
						aaux_state = AVR_WAIT_NEW_DATA;
						break;
					case AVR_WAIT_NEW_DATA:
						if(++loop2 > 5)		// loop here to test the possibility that
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
							mvwprintw(win, DISP_OFFSET+22, 2, "loop2 = %d ",loop2);
							aaux_state = AVR_WAIT_NEW_DATA;
						}
						break;
					case SEND_NEW_DATA:
						res = write(fd,&aux_data,AUX_DATA_SIZE);
						memset(aux_data,0,AUX_DATA_SIZE);
						aaux_state = AVR_IDLE;
   						mvwprintw(win, DISP_OFFSET+23, 2, "             ");
						break;
					default:
						mvwprintw(win, DISP_OFFSET+23, 2, "off the rails");
						aaux_state = AVR_IDLE;
						break;
				}
/*
				for(i = 0;i < aux_type;i++)
				{
					aux_data[i] = i + 0x30;
//					tempx[i] &= 0x7f;
				}
*/
//				usleep(1000);
//				write(fd,aux_data,AUX_DATA_SIZE);
//				mvwprintw(win, DISP_OFFSET+19, 2, "aux_type: %d  ",aux_type);
				wrefresh(win);

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
//				printf("uint1:%s\n",param_string);
			done = 1;
			break;
		case SEND_UINT2:
			xword = (UINT)temp_UINT;
			temp_UINT = (UINT)inch;
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

static void disp_astate(UCHAR state, char *str)
{
	switch (state)
	{
		case AVR_IDLE:
		strcpy(str,"AVR_IDLE\0");
		break;
		case REQ_DATA:
		strcpy(str,"REQ_DATA\0");
		break;
		case WAIT_DATA_READY:
		strcpy(str,"WAIT_DATA_READY\0");
		break;
		case READ_DATA:
		strcpy(str,"READ_DATA\0");
		break;
		case AVR_WAIT_NEW_DATA:
		strcpy(str,"AVR_WAIT_NEW_DATA\0");
		break;
		case SEND_NEW_DATA:
		strcpy(str,"SEND_NEW_DATA\0");
		break;
		default:
		break;
	}
}
