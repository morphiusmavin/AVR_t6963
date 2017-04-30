#if 1
// test_write_data.c - used to test the data protocol between the AVR and PIC24 where the
// data string is sent with FF,FE,FD.. and the next 3 bytes are shifted so they don't
// have the high bits set - displays in ncurses window
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

#define BAUDRATE B19200
#define MODEMDEVICE "/dev/ttyS0"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define LEN 200
void set_defaults(void);
// really cranking
#define TIME_DELAY 20000
// readable
//#define TIME_DELAY 300000

int set_interface_attribs (int fd, int speed, int parity);
void set_blocking (int fd, int should_block);
static void disp_pstate(UCHAR state, char *str);
static int break_out_loop(int loop,UCHAR curr_state);
static UCHAR get_keypress(UCHAR ch,WINDOW *win, int display_offset);
#endif
//******************************************************************************************//
//****************************************** main ******************************************//
//******************************************************************************************//
int main(int argc, char *argv[])
{
#if 1
	int fd, res, res2;
	int type = 0;
	int iters, itr, itr2 = 0;
	int i,j;
	UCHAR alnum_strlen = 0;
	int write_alnum = 5;
	UCHAR ch;
	struct termios oldtio,newtio;
	WINDOW *menu_win;
	useconds_t tdelay = TIME_DELAY;
	useconds_t tdelay2 = TIME_DELAY;
    UCHAR data = 2;
    UCHAR data1 = 0;
	UINT data2 = 0;
	UINT data3 = 0;
	UINT rtdata[11];	// this should be no or rt_params
    UCHAR code = RT_RPM;
    UCHAR code2 = code - RT_RPM;
	int done;
	UINT rpm;
	UCHAR key;
	UCHAR wkey;
	char tempx[20];
	int loop = 0;
	int loop2 = 0;
	int do_states = SCALE_DISP_ALL;
	UCHAR gval_done = 1;
	UINT starting_aux = 0;
	char param_string[10];
	int display_offset = 3;
	UCHAR read_buf[NUM_ENTRY_SIZE];
	int y = 0;
	UINT temp_int;
	UINT temp_int2 = 0;

	if(argv[1][0] == 'w')
		type = 1;
	else if(argv[1][0] == 'r')
		type = 0;
	else
	{
		printf("usage: test_data w [no iters][starting rpm][others][delay]\n");
		printf("or test_data r\n");
		return 1;
	}
	memset(new_global_number,0,NUM_ENTRY_SIZE);
	memset(labels,0,NUM_LABELS*MAX_LABEL_LEN);
	memset(menu_structs,0,NUM_MENU_STRUCTS*sizeof(MENU_FUNC_STRUCT));
	memset(rt_params,0,NUM_RT_PARAMS*sizeof(RT_PARAM));
	memset(P24_rt_params,0,NUM_RT_PARAMS*sizeof(RT_PARAM));
#endif
#if 0
	burn_eeprom();
	printf("P24_rt_params\n");
	for(i = 0;i < no_rtparams;i++)
	{
//		printf("%d\t",i);
		printf("%d\t",P24_rt_params[i].row);
		printf("%d\t",P24_rt_params[i].col);
		printf("%d\t",P24_rt_params[i].shown);
		printf("%d\t",P24_rt_params[i].dtype);
		printf("%d\n",P24_rt_params[i].type);
	}

	for(i = 0;i < no_labels;i++)
	{
		printf("%d %s\n",i,labels[i]);
	}

	printf("rt_params:\n");
	for(i = 0;i < no_rtparams;i++)
	{
//		printf("%d\t",i);
		printf("%d\t",rt_params[i].row);
		printf("%d\t",rt_params[i].col);
		printf("%d\t",rt_params[i].shown);
		printf("%d\t",rt_params[i].dtype);
		printf("%d\n",rt_params[i].type);
	}
	printf("menu structs: %d\n",no_menu_structs);
//	for(i = 0;i < no_menu_structs;i++)
	for(i = 0;i < 2;i++)
	{
		printf("%d\t",menu_structs[i].enabled);
		printf("%d\t",menu_structs[i].fptr);
		printf("%d\t",menu_structs[i].menu);
		printf("%x\n",menu_structs[i].label);
	}
	init_list();
	return 0;
#endif
#if 1
	burn_eeprom();
	initscr();			/* Start curses mode 		*/
	clear();
	noecho();
	nodelay(stdscr,TRUE);
	raw();				/* Line buffering disabled	*/
	cbreak();	/* Line buffering disabled. pass on everything */
	menu_win = newwin(55, 65, 0,0);
	keypad(menu_win, TRUE);
	nodelay(menu_win, TRUE);
	box(menu_win,0,0);
	set_win(menu_win);

	if(type == 0)
		display_offset = DISP_OFFSET;

	else if(type == 1)
	{
//		printf("write: ");
		iters = atoi(argv[2])*10;
//		printf("iters: %d\n",iters);
		mvwprintw(menu_win,display_offset+17,4,"interations: %d",iters);
		if(argc > 2)
		{
			data2 = atoi(argv[3]);
//			printf("rpm starting at: %d\n",data2);
			mvwprintw(menu_win,display_offset+18,4,"rpm starting at: %d",data2);
		}
		if(argc > 3)
		{
			data = atoi(argv[4]);
//			printf("others starting at: %d\n",data);
			mvwprintw(menu_win,display_offset+19,4,"others starting at: %d",data);
		}
		if(argc > 4)
		{
//			printf("time delay: %s  ",argv[5]);
			tdelay = atoi(argv[5])*500;
			tdelay2 = atoi(argv[5])*1000;
			mvwprintw(menu_win,display_offset+20,4,"time delays: %d  %d ",tdelay,tdelay2);
		}
	}
	mvwprintw(menu_win, display_offset, 2,"RPM ");
	mvwprintw(menu_win, display_offset+1,2,"ENGT ");
	mvwprintw(menu_win, display_offset+2, 2,"TRIP ");
	mvwprintw(menu_win, display_offset+3, 2,"TIME ");
	mvwprintw(menu_win, display_offset+4, 2,"AIRT ");
	mvwprintw(menu_win, display_offset+5, 2,"MPH ");
	mvwprintw(menu_win, display_offset+6, 2,"OILP ");
	mvwprintw(menu_win, display_offset+7, 2,"MAP ");
	mvwprintw(menu_win, display_offset+8, 2,"OILT ");
	mvwprintw(menu_win, display_offset+9, 2,"O2 ");
	mvwprintw(menu_win, display_offset+10, 2,"AUX1 ");
	mvwprintw(menu_win, display_offset+11, 2,"AUX2 ");
	wrefresh(menu_win);

	memset(&newtio, 0, sizeof newtio);

	fd = open (MODEMDEVICE, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd <0) {perror(MODEMDEVICE);
		exit(-1); }

	if(tcgetattr(fd,&oldtio) != 0) /* save current port settings */
	{
		printf("Error from tcgetattr: %d ",errno);
		close(fd);
		exit(1);
	}

	set_interface_attribs (fd, BAUDRATE, 0);
	set_blocking (fd, 0);

	res2 = 0;
	set_defaults();
	set_state_defaults();

// read	- simulate the AVR
	if(type == 0)
	{
		if(argc > 2)
			starting_aux = atoi(argv[2]);
//		printf("starting aux: %d\n",starting_aux);
		do_read(menu_win, fd,display_offset);
//		printf("do_read: %d\n",ret_code);
	}	// end of else
#endif
// write - simulate the PIC24
	else if(type == 1)
	{
		memset(read_buf,0,sizeof(read_buf));
		for(i = 0;i < 11;i++)
			rtdata[i] = i;
		rtdata[RT_RPM] = 300;
		rtdata[RT_AUX1] = 0;
		rtdata[RT_AUX2] = 100;
/*
		P24_rt_params[RT_TIME].shown = 0;
		P24_rt_params[RT_TRIP].shown = 0;
		P24_rt_params[RT_AIRT].shown = 0;
		P24_rt_params[RT_MAP].shown = 0;
		P24_rt_params[RT_OILT].shown = 0;
		P24_rt_params[RT_O2].shown = 0;
		P24_rt_params[RT_ENGT].shown = 0;
		P24_rt_params[RT_MPH].shown = 0;
		P24_rt_params[RT_OILP].shown = 0;
*/
//		P24_rt_params[RT_AUX].shown = 0;

		for(itr = 0;itr < iters;itr++)
		{
#if 0
			mvwprintw(menu_win, display_offset+22, 4,"do_states: %d ",do_states);
			read(fd,&auxcmd,1);
			read(fd,&auxparam,1);
			mvwprintw(menu_win, display_offset+24, 4,"auxcmd: %d auxparam %d ",auxcmd,auxparam);
			if(auxcmd == SWITCH_SCALE_TYPE)
			if(1)
			{
				switch(auxparam)
				{
					case SCALE_DISP_ALL:
						do_states = SCALE_DISP_ALL;
						for(i = 0;i < no_rtparams;i++)
							P24_rt_params[i].shown = 1;
						break;
					case SCALE_DISP_SOME:
						do_states = SCALE_DISP_SOME;
						for(i = 0;i < no_rtparams;i++)
							if(P24_rt_params[i].dtype == 1)
								P24_rt_params[i].shown = 1;
							else
								P24_rt_params[i].shown = 0;
						break;
					case SCALE_DISP_NONE:
						for(i = 0;i < no_rtparams;i++)
							P24_rt_params[i].shown = 0;
						do_states = SCALE_DISP_NONE;
						break;
					default:
						for(i = 0;i < no_rtparams;i++)
							P24_rt_params[i].shown = 1;
						do_states = SCALE_DISP_ALL;
						break;
				}
			}
#endif
//			for(i = 0;i < 20;i++)
//				mvwprintw(menu_win, display_offset+i+21, 4,"                      ");

//#if 0
			if(code == RT_AUX1)
			{
				switch(paux_state)
				{
					case IDLE_AUX:
						if(aux_data[0] == CMD_GET_DATA)
							paux_state = DATA_REQ;
						else
							paux_state = IDLE_AUX;
						break;
					case DATA_REQ:
						temp_int2++;
						auxcmd = CMD_DATA_READY;
						auxparam = auxcmd + 1;
						paux_state = VALID_DATA;
						break;
					case VALID_DATA:
						if(aux_data[0] == CMD_NEW_DATA)
							paux_state = DATA_READY;
						else
						{
							paux_state = VALID_DATA;
//							loop = break_out_loop(loop, paux_state);
//							mvwprintw(menu_win, display_offset+40, 2,"loop: %d  ",loop);
						}	
						break;
					case DATA_READY:
						for(i = 0;i < AUX_DATA_SIZE;i++)
							mvwprintw(menu_win, display_offset+34, 2+(i * 3),"%x  ",aux_data[i]);
						paux_state = IDLE_AUX;
						break;
					default:
						paux_state = IDLE_AUX;
						break;
				}
				disp_pstate(paux_state,tempx);
//				auxcmd = aux_data[0];
				mvwprintw(menu_win, display_offset+32, 2,"%s        ",tempx);
				mvwprintw(menu_win, display_offset+33, 2,"cmd: %x  param: %x  ",auxcmd,auxparam);
			}
			else if(code == RT_AUX2)
//#endif
//			if(code == RT_AUX2)
			{
//				rtdata[code] = (UINT)(auxcmd << 8) | (UINT)auxparam;
//				auxcmd++;
//				auxparam++;
				res = read(fd,&aux_data,AUX_DATA_SIZE);
				mvwprintw(menu_win, display_offset+28, 2,"res: %d  ",res);
				for(i = 0;i < AUX_DATA_SIZE;i++)
					mvwprintw(menu_win, display_offset+29, 2+(i * 3),"%x  ",aux_data[i]);
//				mvwprintw(menu_win, display_offset+30,2,"%x %x   ",aux_data[0],aux_data[1]);
				mvwprintw(menu_win, display_offset+30,2,"%x %x   ",aux_data[2],aux_data[3]);
			}	
			write(fd,&code,1);
			usleep(tdelay);

//			if(P24_rt_params[code2].shown == 1)
// shown means if it gets displayed - add a field called 'sent' if we don't want to send it
			if(1)
			{
				data3 = rtdata[code2];
				// this is a round-about way to send data back to the AVR
				// since the AUX1 state table can't do a write back
				if(P24_rt_params[code2].dtype == 2)			//AUX1
				{
					rtdata[code2] = temp_int2;
				}
				else if(P24_rt_params[code2].dtype == 3)	// AUX2
//				if(P24_rt_params[code2].dtype == 3)
				{
					temp_int = (UINT)auxcmd;
					temp_int <<= 8;
					temp_int &= 0xff00;
					rtdata[code2] = temp_int;
					temp_int = (UINT)auxparam;
					rtdata[code2] |= temp_int;
					mvwprintw(menu_win, display_offset+31,2,"%x %x %x   ",rtdata[code2],auxcmd,auxparam);
				}
				else
					rtdata[code2] = data3;
					
				if(P24_rt_params[code2].dtype > 0)
				{
//					usleep(tdelay2);
					data2 = rtdata[code2];
					if(data2 & 0x8000)
					{
						ch = RT_HIGH3;
						res = write(fd,&ch,1);
						usleep(tdelay);
						data1 = (UCHAR)(data2);
						write(fd,&data1,1);
						usleep(tdelay);
						data1 = (UCHAR)(data2>>8);
						data1 &= 0x7f;
						ch = data1;
						res = write(fd,&ch,1);
						usleep(tdelay);
					}
					else if(data2 & 0x0080)
					{
						ch = RT_HIGH2;
//						mvwprintw(menu_win, display_offset+2, 18,"RT_HIGH2");
						res = write(fd,&ch,1);
						usleep(tdelay);
						data1 = (UCHAR)data2;
						data1 &= 0x7f;
						ch = data1;
						res = write(fd,&ch,1);
						usleep(tdelay);
						data1 = (UCHAR)(data2>>8);
						ch = data1;
						res = write(fd,&ch,1);
						usleep(tdelay);
					}
					else
					{
						ch = RT_HIGH1;
//						mvwprintw(menu_win, display_offset+2, 18,"RT_HIGH1");
						res = write(fd,&ch,1);
						usleep(tdelay);
						data1 = (UCHAR)(data2);
						ch = data1;
						res = write(fd,&ch,1);
						usleep(tdelay);
						data1 = (UCHAR)(data2>>8);
						ch = data1;
						res = write(fd,&ch,1);
						usleep(tdelay);
					}

					sprintf(param_string,"%4u",data2);
					mvwprintw(menu_win, display_offset+code2, 10,"              ");
					mvwprintw(menu_win, display_offset+code2, 10, param_string);
					wrefresh(menu_win);
					data2 += 10;
					rtdata[code2] = data2;
				}
				// end of if RT_RPM
				else if(P24_rt_params[code2].dtype == 0)
				{
					data = (UCHAR)rtdata[code2];
					if(data > 0x7f)
					{
						ch = RT_HIGH0;
//						mvwprintw(menu_win, display_offset+2, 18,"RT_HIGH0");
						res = write(fd,&ch,1);
						usleep(tdelay);
						data1 = data & 0x7f;
						ch = data1;
						res = write(fd,&ch,1);
						usleep(tdelay);
					}
					else
					{
						ch = RT_LOW;
//						mvwprintw(menu_win, display_offset+2, 18,"RT_LOW  ");
						res = write(fd,&ch,1);
						usleep(tdelay);
						ch = data;
						res = write(fd,&ch,1);
						usleep(tdelay);
					}
					data++;
					rtdata[code2] = (UINT)data;
					sprintf(param_string,"%4u",data);
					mvwprintw(menu_win, display_offset+code2, 10, param_string);
					wrefresh(menu_win);
				}
			}
			// end of if UCHAR (not RT_RPM)

			mvwprintw(menu_win, display_offset+17, 4, "iterations left: %d   ",iters-itr);
			wrefresh(menu_win);

	// see if one of the keys from the "keypad" is pressed
			key = wgetch(menu_win);
			wkey = get_keypress(key,menu_win, display_offset);
			if(wkey != 0xff)
				write(fd,&wkey,1);	// simulates the PIC24 sending a keypress
			if(++code > RT_AUX2)
				code = RT_RPM;
			code2 = code - RT_RPM;
//			mvwprintw(menu_win, display_offset+39, 2, "rt_type: %s        ",get_label(code2));
			wrefresh(menu_win);
		}
	}
	delwin(menu_win);
	clrtoeol();
	refresh();
	endwin();

	tcsetattr(fd,TCSANOW,&oldtio);
	close(fd);
	exit(0);
}
// this is duplicated in main.c
//******************************************************************************************//
//************************************** break_out_loop ************************************//
//******************************************************************************************//
static int break_out_loop(int loop,UCHAR curr_state)
{
	if(++loop > 5)
	{
		paux_state = IDLE_AUX;
		return 0;
	}
	else
	{
		paux_state = curr_state;
		return loop;
	}
}

void set_defaults(void)
{
	temp_UINT = 0;
	parse_state = IDLE;
}
void set_state_defaults(void)
{
	paux_state = IDLE_AUX;
	aaux_state = IDLE_AUX;
}
//******************************************************************************************//
//**************************************** disp_pstate *************************************//
//******************************************************************************************//
// PIC24_aux_states (main.h)
static void disp_pstate(UCHAR state, char *str)
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
		default:
			strcpy(str,"<bad state>\0");
			break;
	}
}
//******************************************************************************************//
//************************************* disp_cmdtype ***************************************//
//******************************************************************************************//
void disp_cmdtype(UCHAR state, char *str)
{
/*
	switch (state)
	{
		case START_AVR:
		strcpy(str,"START_AVR\0");
		break;
		case DATA_READY:
		strcpy(str,"DATA_READY\0");
		break;
		case NEW_DATA_READY:
		strcpy(str,"NEW_DATA_READY\0");
		break;
		case NONE:
		strcpy(str,"NONE\0");
		break;
		strcpy(str,"<bad cmd>\0");
		default:
		break;
	}
*/
}

//******************************************************************************************//
//*************************************** get_keypress *************************************//
//******************************************************************************************//
static UCHAR get_keypress(UCHAR key,WINDOW *win, int display_offset)
{
	UCHAR wkey;
	if(key != 0xff)
	{
		switch(key)
		{
			case '0':
				mvwprintw(win, display_offset+23, 8, "zero  ");
				wkey = KP_0;
				break;
			case '1':
				mvwprintw(win, display_offset+23, 8, "one   ");
				wkey = KP_1;
				break;
			case '2':
				mvwprintw(win, display_offset+23, 8, "two   ");
				wkey = KP_2;
				break;
			case '3':
				mvwprintw(win,display_offset+23, 8, "three ");
				wkey = KP_3;
				break;
			case '4':
				mvwprintw(win, display_offset+23, 8, "four  ");
				wkey = KP_4;
				break;
			case '5':
				mvwprintw(win, display_offset+23, 8, "five  ");
				wkey = KP_5;
				break;
			case '6':
				mvwprintw(win, display_offset+23, 8, "six   ");
				wkey = KP_6;
				break;
			case '7':
				mvwprintw(win, display_offset+23, 8, "seven ");
				wkey = KP_7;
				break;
			case '8':
				mvwprintw(win, display_offset+23, 8, "eight ");
				wkey = KP_8;
				break;
			case '9':
				mvwprintw(win, display_offset+23, 8, "nine  ");
				wkey = KP_9;
				break;
			case '*':
				mvwprintw(win, display_offset+23, 8, "ast   ");
				wkey = KP_AST;
				break;
			case '#':
				mvwprintw(win, display_offset+23, 8, "pound ");
				wkey = KP_POUND;
				break;
			case 'A':
			case 'a':
				mvwprintw(win, display_offset+23, 8, "A     ");
				wkey = KP_A;
				break;
			case 'B':
			case 'b':
				mvwprintw(win, display_offset+23, 8, "B     ");
				wkey = KP_B;
				break;
			case 'C':
			case 'c':
				mvwprintw(win, display_offset+23, 8, "C     ");
				wkey = KP_C;
				break;
			case 'D':
			case 'd':
				mvwprintw(win, display_offset+23, 8, "D     ");
				wkey = KP_D;
				break;
// use 'z' as a shortcut to '*' and 'y' as a shortcut to '#'
			case 'Y':
			case 'y':
				mvwprintw(win, display_offset+23, 8, "pound ");
				wkey = KP_POUND;
				break;
			case 'Z':
			case 'z':
				mvwprintw(win, display_offset+23, 8, "ast   ");
				wkey = KP_AST;
				break;
			default:
				mvwprintw(win, display_offset+23, 8, "?     ");
				wkey = 0xff;
				break;
		}
	}
	return wkey;
}

//******************************************************************************************//
//*********************************** set_interface_attribs ********************************//
//******************************************************************************************//
int set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                printf ("error %d from tcgetattr", errno);
				perror(" ");
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                printf ("error %d from tcsetattr", errno);
				perror(" ");
                return -1;
        }
        return 0;
}

void set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                printf ("error %d from tggetattr", errno);
                perror(" ");
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
//        tty.c_cc[VTIME] = 10;			// 1 second read timeout
//        tty.c_cc[VTIME] = 0.5;
        tty.c_cc[VTIME] = 1;            // 0.1 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                printf ("error %d setting term attributes", errno);
}

