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
	UCHAR xbyte;
	char param_string[10];
	UINT xword;
	UCHAR txword;
	UCHAR temp;
	UCHAR limit8;
	UINT limit16;
	int i,j;
	char test_str[20];

	init_list();
	display_labels();

	while(TRUE)
	{
		done = 0;
		res = read(fd,&ch,1);
		ch = get_key(ch,limit8,limit16);
		if(curr_fptr_changed())
		{
			switch(get_curr_menu())
			{
				case 0:
				strcpy(test_str,"MAIN\0");
				break;
				case 1:
				strcpy(test_str,"MENU1A\0");
				break;
				case 2:
				strcpy(test_str,"MENU1B\0");
				break;
				case 3:
				strcpy(test_str,"MENU1C\0");
				break;
				case 4:
				strcpy(test_str,"MENU1D\0");
				break;
				case 5:
				strcpy(test_str,"MENU2A\0");
				break;
				case 6:
				strcpy(test_str,"MENU2B\0");
				break;
				case 7:
				strcpy(test_str,"MENU2C\0");
				break;
				case 8:
				strcpy(test_str,"MENU3A\0");
				break;
				case 9:
				strcpy(test_str,"MENU3B\0");
				break;
				case 10:
				strcpy(test_str,"MENU3C\0");
				break;
				case 11:
				strcpy(test_str,"MENU4A\0");
				break;
				case 12:
				strcpy(test_str,"MENU4B\0");
				break;
				case 13:
				strcpy(test_str,"MAIN4C\0");
				break;
				case 14:
				strcpy(test_str,"NUM_ENTRY\0");
				break;
				case 15:
				strcpy(test_str,"ALNUM_ENTRY\0");
				break;
				default:
				break;
			}
			mvwprintw(win, display_offset+17, 5, "%d  %d  %s  ",get_curr_fptr(),get_curr_menu(),test_str);
			mvwprintw(win, display_offset+18, 5,"                              ");
			mvwprintw(win, display_offset+19, 5,"                              ");
			mvwprintw(win, display_offset+20, 5,"                              ");
			mvwprintw(win, display_offset+21, 5,"                              ");
			mvwprintw(win, display_offset+18, 5,"current: %s",cur_global_number);
			mvwprintw(win, display_offset+19, 5,"new:     %s",new_global_number);
			for(j = 0;j < NUM_ENTRY_SIZE;j++)
				mvwprintw(win, display_offset+20, 5+j,"%c",cur_global_number[j]);
			for(j = 0;j < NUM_ENTRY_SIZE;j++)
				mvwprintw(win, display_offset+21, 5+j,"%c",new_global_number[j]);
		}
		switch(parse_state)
		{
			case IDLE:
				if(ch <= RT_RPM && ch >= RT_TRIP)
				{
					current_param = ch;
//						printf("current:");
					parse_state = CHECK_HIGHBIT;
				}
				else if(ch == RT_AUX)
				{
					ch = get_str_len();
					write(fd,&ch,1);
					write(fd,new_global_number,NUM_ENTRY_SIZE);
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
//					printf("uchar0:%s\n",param_string);
				done = 1;
				break;
			case SEND_UCHAR1:
				xbyte = ch | 0x80;
				sprintf(param_string,"%4d",xbyte);
//					printf("uchar1:%s\n",param_string);
				done = 1;
				break;
			case SEND_UINT0:
				xword = (UINT)temp_UINT;
				temp_UINT = (UINT)ch;
				temp_UINT <<= 8;
				temp_UINT &= 0xff00;
				xword |= temp_UINT;
				sprintf(param_string,"%4u",xword);
//					printf("uint0:%s\n",param_string);

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
//					printf("uint1:%s\n",param_string);
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
//					printf("uint2:%s\n",param_string);
				done = 1;
				break;
			default:
				printf("%s\n","default ");
				set_defaults();
				break;
		}	// end of inner switch
		if(done)
		{
			mvwprintw(win, display_offset+current_param-0xF4, 15, param_string);
			wrefresh(win);

			temp = ~current_param;
			if(temp == 0)
			{
				txword = (UCHAR)(xword>>8);
				write(fd,&txword,1);
				txword = (UCHAR)(xword);
				write(fd,&txword,1);
			}
			else
				write(fd,&xbyte,1);

// this displays the RT params on the screen at their positions according to the prompts struct
			for(i = 0;i < no_prompts;i++)
			{
//						mvwprintw(win, display_offset+current_param-0xF4, (i*3)+10, param_string);

				if(prompts[i].type == RT_LABEL && temp == prompts[i].pnum)
				{
						GDispStringAt(prompts[i].row,prompts[i].col+10,param_string);
//								mvwprintw(win, display_offset+current_param-0xF4, (i*3)+10, param_string);
				}
			}
			set_defaults();
			mvwprintw(win, display_offset+22, 5,"cursor row: %d cursor col: %d  ",cursor_row,cursor_col);
		}	// end of done
	}	// end of while(1)
}

