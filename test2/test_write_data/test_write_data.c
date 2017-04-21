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
#define DISP_OFFSET 15
void set_defaults(void);
// really cranking
#define TIME_DELAY 2000
// readable
//#define TIME_DELAY 300000

//******************************************************************************************//
//****************************************** main ******************************************//
//******************************************************************************************//
int main(int argc, char *argv[])
{
	int fd, res, res2;
	int type = 0;
	int iters;
	int i,j;
	UCHAR alnum_strlen = 0;
	int write_alnum = 10;
	UCHAR ch;
	useconds_t tdelay = TIME_DELAY;
    UCHAR data = 2;
    UCHAR data1 = 0;
	UINT data2 = 0;
    UCHAR code = RT_RPM-1;
    UCHAR read_buff[10];
	int done;
	UINT rpm;
	char key;
	UCHAR wkey;
	int ret_code;
	char param_string[10];
	int display_offset = 0;
	char aux_array[NUM_ENTRY_SIZE];

	memset(aux_array,0,NUM_ENTRY_SIZE);
	memset(labels,0,NUM_LABELS*MAX_LABEL_LEN);
	memset(menu_structs,0,NUM_MENU_STRUCTS*sizeof(MENU_FUNC_STRUCT));
	memset(rt_params,0,NUM_RT_PARAMS*sizeof(RT_PARAM));

	burn_eeprom();
	char temp_label[MAX_LABEL_LEN];
#if 0
	for(i = 0;i < no_labels;i++)
	{
		printf("%d %s\n",i,labels[i]);
	}

	printf("rt_params:\n");
	for(i = 0;i < no_rtparams;i++)
	{
		printf("%d\t",i);
		printf("%d\t",rt_params[i].row);
		printf("%d\t",rt_params[i].col);
		printf("%d\t",rt_params[i].shown);
		printf("%d\n",rt_params[i].type);
	}
	printf("menu structs: %d\n",no_menu_structs);
	for(i = 0;i < 24;i++)
//	for(i = 0;i < no_menu_structs;i++)
	{
		if(i % 6 == 0)
			printf("\n");
		printf("%d\t",menu_structs[i].enabled);
		printf("%d\t",menu_structs[i].fptr);
		printf("%d\t",menu_structs[i].menu);
		printf("%x\n",menu_structs[i].label);
	}
	printf("\n");
#endif
	init_list();
//	return 0;
	memset(read_buff,0,10);
	res2 = 0;
	set_defaults();

// read	- simulate the AVR
// see if one of the keys from the "keypad" is pressed
	while(1)
	{
		key = 0;

		fflush(stdin);
		key = getc(stdin);

//		printf("key=%c **************************\n",key);
		if(key != 0xff)
		{
			switch(key)
			{
				case '0':
//					mvwprintw(menu_win, display_offset+22, 8, "zero  ");
					printf("zero\n");
					wkey = KP_0;
					break;
				case '1':
//					mvwprintw(menu_win, display_offset+22, 8, "one   ");
					printf("one\n");
					wkey = KP_1;
					break;
				case '2':
//					mvwprintw(menu_win, display_offset+22, 8, "two   ");
					printf("two\n");
					wkey = KP_2;
					break;
				case '3':
//					mvwprintw(menu_win,display_offset+22, 8, "three ");
					printf("three\n");
					wkey = KP_3;
					break;
				case '4':
//					mvwprintw(menu_win, display_offset+22, 8, "four  ");
					printf("four\n");
					wkey = KP_4;
					break;
				case '5':
//					mvwprintw(menu_win, display_offset+22, 8, "five  ");
					printf("five\n");
					wkey = KP_5;
					break;
				case '6':
//					mvwprintw(menu_win, display_offset+22, 8, "six   ");
					printf("six\n");
					wkey = KP_6;
					break;
				case '7':
//					mvwprintw(menu_win, display_offset+22, 8, "seven ");
					printf("seven\n");
					wkey = KP_7;
					break;
				case '8':
//					mvwprintw(menu_win, display_offset+22, 8, "eight ");
					wkey = KP_8;
					printf("eight\n");
					break;
				case '9':
//					mvwprintw(menu_win, display_offset+22, 8, "nine  ");
					printf("nine\n");
					wkey = KP_9;
					break;
				case '*':
//					mvwprintw(menu_win, display_offset+22, 8, "ast   ");
					printf("ast\n");
					wkey = KP_AST;
					break;
				case '#':
//					mvwprintw(menu_win, display_offset+22, 8, "pound ");
					printf("pound\n");
					wkey = KP_POUND;
					break;
				case 'A':
				case 'a':
//					mvwprintw(menu_win, display_offset+22, 8, "A     ");
					printf("A\n");
					wkey = KP_A;
					break;
				case 'B':
				case 'b':
//					mvwprintw(menu_win, display_offset+22, 8, "B     ");
					printf("B\n");
					wkey = KP_B;
					break;
				case 'C':
				case 'c':
//					mvwprintw(menu_win, display_offset+22, 8, "C     ");
					printf("C\n");
					wkey = KP_C;
					break;
				case 'D':
				case 'd':
//					mvwprintw(menu_win, display_offset+22, 8, "D     ");
					printf("D\n");
					wkey = KP_D;
					break;
// use 'z' as a shortcut to '#' and 'y' as a shcortcut to '#'
				case 'Y':
				case 'y':
//					mvwprintw(menu_win, display_offset+22, 8, "pound ");
					printf("pound\n");
					wkey = KP_POUND;
					break;
				case 'Z':
				case 'z':
//					mvwprintw(menu_win, display_offset+22, 8, "ast   ");
					printf("ast\n");
					wkey = KP_AST;
					break;
				case 'X':
				case 'x':
					return 0;
				default:
//					mvwprintw(menu_win, display_offset+22, 8, "?     ");
					printf("?\n");
					wkey = 0xff;
					break;
			}
			if(wkey != 0xff)
				get_key(wkey);
		}
	}	// end of else

	exit(0);
}

void set_defaults(void)
{
	temp_UINT = 0;
	parse_state = IDLE;
}

