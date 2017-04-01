#ifndef NOAVR
#include <avr/io.h>
#include "../avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include <avr/eeprom.h>
#include "macros.h"
#else
#warning "NOAVR"
#include <ncurses.h>
#endif
#include "sfr_helper.h"
#include <stdlib.h>
#include "main.h"
#include "t6963.h"
#include "USART.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
static void clean_disp_num(void);
static void cursor_forward(void);
static void cursor_backward(void);
static void cursor_forward_stuff(char);
static void stuff_num(char);
static void scroll_alnum_list(int dir, int single);
static void show_alnum_list(int show);
static UCHAR main_menu_func(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col);
static UCHAR menu1a(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col);
static UCHAR menu1b(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col);
static UCHAR menu1c(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col);
static UCHAR menu1d(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col);
static UCHAR menu2a(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col);
static UCHAR menu2b(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col);
static UCHAR menu2c(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col);
static UCHAR menu3a(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col);
static UCHAR menu3b(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col);
static UCHAR menu3c(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col);
static UCHAR menu4a(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col);
static UCHAR menu4b(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col);
static UCHAR menu4c(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col);
static UCHAR number_entry(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col);
static UCHAR alnum_entry(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col);
static void show_legend(int override, char *ch0, char *ch1, char *ch2, char *ch3, char *ch4);
static void show_legend(int override, char *ch0, char *ch1, char *ch2, char *ch3, char *ch4)
{
	if(!override && curr_fptr_changed() == 0)
		return;
	GDispStringAt(MENU_START_ROW-1,0,MENU_BLANK);
	GDispStringAt(MENU_START_ROW-1,10,MENU_BLANK);
	GDispStringAt(MENU_START_ROW-1,15,MENU_BLANK);
	GDispStringAt(MENU_START_ROW,0,MENU_BLANK);
	GDispStringAt(MENU_START_ROW,10,MENU_BLANK);
	GDispStringAt(MENU_START_ROW,0,ch0);
	GDispStringAt(MENU_START_ROW+1,0,"* - ");
	GDispStringAt(MENU_START_ROW+1,5,MENU_BLANK);
	GDispStringAt(MENU_START_ROW+1,5,ch1);
	GDispStringAt(MENU_START_ROW+1,MENU_START_COL,"0 - ");
	GDispStringAt(MENU_START_ROW+1,MENU_START_COL+2,"");
	GDispStringAt(MENU_START_ROW+1,MENU_START_COL+2,ch2);
	GDispStringAt(MENU_START_ROW+2,0,"# - ");
	GDispStringAt(MENU_START_ROW+2,5,MENU_BLANK);
	GDispStringAt(MENU_START_ROW+2,5,ch3);
	GDispStringAt(MENU_START_ROW+2,MENU_START_COL,"D - ");
	GDispStringAt(MENU_START_ROW+2,MENU_START_COL+2,MENU_BLANK);
	GDispStringAt(MENU_START_ROW+2,MENU_START_COL+2,ch4);
}
static void show_legend2(int override, char *ch0, char *ch1, char *ch2, char *ch3, char *ch4,char *ch5, char *ch6);
static void show_legend2(int override, char *ch0, char *ch1, char *ch2, char *ch3, char *ch4,char *ch5, char *ch6)
{
	if(!override && curr_fptr_changed() == 0)
		return;
	GDispStringAt(MENU_START_ROW-1,0,MENU_BLANK);
	GDispStringAt(MENU_START_ROW-1,0,ch0);
	GDispStringAt(MENU_START_ROW,0,"A - ");
	GDispStringAt(MENU_START_ROW,5,MENU_BLANK);
	GDispStringAt(MENU_START_ROW,5,ch1);
	GDispStringAt(MENU_START_ROW,MENU_START_COL,"B - ");
	GDispStringAt(MENU_START_ROW,MENU_START_COL+2,MENU_BLANK);
	GDispStringAt(MENU_START_ROW,MENU_START_COL+2,ch2);
	GDispStringAt(MENU_START_ROW+1,0,"C - ");
	GDispStringAt(MENU_START_ROW+1,5,MENU_BLANK);
	GDispStringAt(MENU_START_ROW+1,5,ch3);
	GDispStringAt(MENU_START_ROW+1,MENU_START_COL,"D - ");
	GDispStringAt(MENU_START_ROW+1,MENU_START_COL+2,MENU_BLANK);
	GDispStringAt(MENU_START_ROW+1,MENU_START_COL+2,ch4);
	GDispStringAt(MENU_START_ROW+2,0,"# - ");
	GDispStringAt(MENU_START_ROW+2,5,MENU_BLANK);
	GDispStringAt(MENU_START_ROW+2,5,ch5);
	GDispStringAt(MENU_START_ROW+2,MENU_START_COL,"* - ");
	GDispStringAt(MENU_START_ROW+2,MENU_START_COL+2,MENU_BLANK);
	GDispStringAt(MENU_START_ROW+2,MENU_START_COL+2,ch6);
}
static int first_menu = MAIN_MENU;
static int last_menu = NUM_ENTRY;
static int current_fptr;
static int last_fptr;
static int prev_fptr;
static int list_size;
static int curr_type;
static int scroll_ptr;
static UCHAR cur_row, cur_col;	// used by the current menu/dialog function to keep track of the current row,col
static UCHAR alnum_array[NUM_ALNUM];
static UCHAR choose_alnum;
#define LIST_SIZE 50
static void prev_list(void);
static int menu_list[LIST_SIZE];

static UCHAR (*fptr[NUM_FPTS])(UCHAR, UCHAR, UINT, UCHAR, UCHAR) = { main_menu_func,\
	 menu1a, menu1b,\
	 menu1c, menu1d,\
	 menu2a, menu2b, menu2c,\
	 menu3a, menu3b, menu3c,\
	 menu4a, menu4b, menu4c, number_entry, alnum_entry };

void init_list(void)
{
	int i = 0;
	UCHAR k;
	for(k = 33;k < 58;k++)	// '!' - '9'
		alnum_array[i++] = k;
	for(k = 65;k < 91;k++)	// 'A' - 'Z'
		alnum_array[i++] = k;
	for(k = 97;k < 123;k++)	// 'a' - 'z'
		alnum_array[i++] = k;
	choose_alnum = 0x30;
	scroll_ptr = 25;		// start showing at 'a' (skip '!' - '9')
	list_size = LIST_SIZE;
	current_fptr = first_menu;
	last_fptr = first_menu;
	prev_fptr = first_menu;
	memset(menu_list,0,sizeof(menu_list));
	menu_list[0] = current_fptr;
	curr_type = MENU1;
	show_legend(1,"main","1a","1b","1c","1d");
}

static void prev_list(void)
{
	if(current_fptr == 0)
		return;
	else
	{
		menu_list[current_fptr] = 0;
		current_fptr--;
	}
//	return menu_list[current_fptr];
}

static void set_list(int fptr)
{
	if(current_fptr < list_size)
	{
		current_fptr++;
		menu_list[current_fptr] = fptr;
	}
}

UCHAR get_key(UCHAR ch, UCHAR limit8, UINT limit16)
{
	UCHAR wkey;
	wkey = (*fptr[get_curr_menu()])(ch, limit8, limit16, cur_row, cur_col);
	return ch;
}

int get_curr_fptr(void)
{
	return current_fptr;
}

int get_curr_menu(void)
{
	return menu_list[current_fptr];
}

int curr_fptr_changed(void)
{
	int ret;
	ret = current_fptr != last_fptr;
	last_fptr = current_fptr;
	return ret;
}

int get_type(void)
{
	return curr_type;
}

//******************************************************************************************//
//************************************* main_menu_func *************************************//
//******************************************************************************************//
// for when no menu is shown
static UCHAR main_menu_func(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col)
{
//	UCHAR row1,col1,k;
	UCHAR ret_char = ch;
//	printHexByte(ch);
	curr_type = MENU1;

	show_legend(1,"main","1a","1b","1c","num entry");
//	GDispStringAt(12,0,"back to main menu   ");
//	GDispStringAt(13,0,"                    ");

	switch (ch)
	{
		case KP_AST:	// '*'
			set_list(MENU1A);
			break;
		case KP_0:	// '0'
			set_list(MENU1B);
			break;
		case KP_POUND:	// '#'
			set_list(MENU1C);
			break;
		case KP_D:	// 'D'
			show_legend2(1,"number entry","enter","cancel","forwd","backwd","alnum","exit");
			cur_row = NUM_ENTRY_ROW;
			cur_col = NUM_ENTRY_BEGIN_COL;
			memset((void*)new_global_number,0,sizeof(new_global_number));
			clean_disp_num();
			set_list(NUM_ENTRY);
			break;
		default:
			ret_char = ch;
			break;
	}
	return ret_char;

}
//******************************************************************************************//
//****************************************** menu1a ****************************************//
//******************************************************************************************//
// displays the 1st choice of the main menu
static UCHAR menu1a(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char = ch;
	curr_type = MENU2;
	show_legend(1,"1a","back","2a","2b","2c");
	switch (ch)
	{
		case KP_AST:
			prev_list();
			break;
		case KP_0:
			set_list(MENU2A);
			break;
		case KP_POUND:
			set_list(MENU2B);
			break;
		case KP_D:
			set_list(MENU2C);
			break;
		default:
			ret_char = ch;
			break;
	}
	return ret_char;
}
//******************************************************************************************//
//****************************************** menu1b ****************************************//
//******************************************************************************************//
// displays the 2nd choice of the main menu
static UCHAR menu1b(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char = ch;
	curr_type = MENU3;
	show_legend(1,"1b","back","3a","3b","3c");
	switch (ch)
	{
		case KP_AST:
			prev_list();
			break;
		case KP_0:
			set_list(MENU3A);
			break;
		case KP_POUND:
			set_list(MENU3B);
			break;
		case KP_D:
			set_list(MENU3C);
			break;
		default:
			ret_char = ch;
			break;
	}
	return ret_char;
}
//******************************************************************************************//
//****************************************** menu1c ****************************************//
//******************************************************************************************//
// displays the 3rd choice of the main menu
static UCHAR menu1c(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char = ch;
	curr_type = MENU3;
	show_legend(1,"1c","back","4a","4b","4c");
	switch (ch)
	{
		case KP_AST:
			prev_list();
			break;
		case KP_0:
			set_list(MENU4A);
			break;
		case KP_POUND:
			set_list(MENU4B);
			break;
		case KP_D:
			set_list(MENU4C);
			break;
		default:
			ret_char = ch;
			break;
	}
	return ret_char;
}
//******************************************************************************************//
//****************************************** menu1d ****************************************//
//******************************************************************************************//
// displays the 4th choice of the main menu
static UCHAR menu1d(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char = ch;
	curr_type = MENU4;
	show_legend(1,"1d","back","menu4a","menu4b","menu4c");
	switch (ch)
	{
		case KP_AST:
			prev_list();
			break;
		case KP_0:
			set_list(MENU4A);
			break;
		case KP_POUND:
			set_list(MENU4B);
			break;
		case KP_D:
			set_list(MENU4C);
			break;
		default:
			ret_char = ch;
			break;
	}
	return ret_char;
}
//******************************************************************************************//
//****************************************** menu2a ****************************************//
//******************************************************************************************//
// displays the 1st choice of the 1st choice of the main menu
static UCHAR menu2a(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char = ch;
	curr_type = MENU4;
	show_legend(1,"2a","back","menu1b","num entry","menu1a");
	switch (ch)
	{
		case KP_AST:
			prev_list();
//			current_fptr = 0;
			break;
		case KP_0:
			set_list(MENU1B);
			break;
		case KP_POUND:
			show_legend2(1,"number entry","enter","cancel","forwd","backwd","alnum","exit");
			cur_row = NUM_ENTRY_ROW;
			cur_col = NUM_ENTRY_BEGIN_COL;
			memset((void*)new_global_number,0,sizeof(new_global_number));
			clean_disp_num();
			set_list(NUM_ENTRY);
			break;
		case KP_D:
			set_list(MENU1A);
			break;
		default:
			ret_char = ch;
			break;
	}
	return ret_char;
}
//******************************************************************************************//
//****************************************** menu2b ****************************************//
//******************************************************************************************//
// displays the 2nd choice of the 1st choice of the main menu
static UCHAR menu2b(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char = ch;
	curr_type = MENU4;
	show_legend(1,"2b","back","main","main","main");
	switch (ch)
	{
		case KP_AST:
			prev_list();
//			current_fptr = 0;
			break;
		case KP_0:
			init_list();
			break;
		case KP_POUND:
			init_list();
			break;
		case KP_D:
			init_list();
			break;
		default:
			ret_char = ch;
			break;
	}
	return ret_char;
}
//******************************************************************************************//
//****************************************** menu2c ****************************************//
//******************************************************************************************//
// displays the 3rd choice of the 1st choice of the main menu
static UCHAR menu2c(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char = ch;
	curr_type = MENU4;
	show_legend(1,"2c","back","main","main","main");
	switch (ch)
	{
		case KP_AST:
			prev_list();
			break;
		case KP_0:
			init_list();
			break;
		case KP_POUND:
			init_list();
			break;
		case KP_D:
			init_list();
			break;
		default:
			ret_char = ch;
			break;
	}
	return ret_char;
}
//******************************************************************************************//
//****************************************** menu3a ****************************************//
//******************************************************************************************//
// displays the 1st choice of the 1st choice of the main menu
static UCHAR menu3a(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char = ch;
	curr_type = MENU4;
	show_legend(1,"3a","back","main","num entry","main");
	switch (ch)
	{
		case KP_AST:
			prev_list();
			break;
		case KP_0:
			init_list();
			break;
		case KP_POUND:
			break;
		case KP_D:
			init_list();
			break;
		default:
			ret_char = ch;
			break;
	}
	return ret_char;
}
//******************************************************************************************//
//****************************************** menu3b ****************************************//
//******************************************************************************************//
// displays the 2nd choice of the 1st choice of the main menu
static UCHAR menu3b(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char = ch;
	curr_type = MENU4;
	show_legend(1,"3b","back","main","main","main");
	switch (ch)
	{
		case KP_AST:
			prev_list();
			break;
		case KP_0:
			init_list();
			break;
		case KP_POUND:
			init_list();
			break;
		case KP_D:
			init_list();
			break;
		default:
			ret_char = ch;
			break;
	}
	return ret_char;
}
//******************************************************************************************//
//****************************************** menu3c ****************************************//
//******************************************************************************************//
// displays the 3rd choice of the 1st choice of the main menu
static UCHAR menu3c(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char = ch;
	curr_type = MENU4;
	show_legend(1,"3c","back","main","main","main");
	switch (ch)
	{
		case KP_AST:
			prev_list();
			break;
		case KP_0:
			init_list();
			break;
		case KP_POUND:
			init_list();
			break;
		case KP_D:
			init_list();
			break;
		default:
			ret_char = ch;
			break;
	}
	return ret_char;
}
//******************************************************************************************//
//****************************************** menu4a ****************************************//
//******************************************************************************************//
// displays the 1st choice of the 1st choice of the main menu
static UCHAR menu4a(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char = ch;
	curr_type = MENU4;
	show_legend(1,"4a","back","main","main","main");
	switch (ch)
	{
		case KP_AST:
			prev_list();
			break;
		case KP_0:
			init_list();
			break;
		case KP_POUND:
			init_list();
			break;
		case KP_D:
			init_list();
			break;
		default:
			ret_char = ch;
			break;
	}
	return ret_char;
}
//******************************************************************************************//
//****************************************** menu4b ****************************************//
//******************************************************************************************//
// displays the 2nd choice of the 1st choice of the main menu
static UCHAR menu4b(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char = ch;
	curr_type = MENU4;
	show_legend(1,"4b","back","main","main","main");
	switch (ch)
	{
		case KP_AST:
			prev_list();
			break;
		case KP_0:
			init_list();
			break;
		case KP_POUND:
			init_list();
			break;
		case KP_D:
			init_list();
			break;
		default:
			ret_char = ch;
			break;
	}
	return ret_char;
}
//******************************************************************************************//
//****************************************** menu4c ****************************************//
//******************************************************************************************//
// displays the 3rd choice of the 1st choice of the main menu
static UCHAR menu4c(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char = ch;
	curr_type = MENU4;
	show_legend(1,"4c","back","main","main","main");
	switch (ch)
	{
		case KP_AST:
			prev_list();
			break;
		case KP_0:
			init_list();
			break;
		case KP_POUND:
			init_list();
			break;
		case KP_D:
			init_list();
			break;
		default:
			ret_char = ch;
			break;
	}
	return ret_char;
}
//******************************************************************************************//
//*************************************** number_entry *************************************//
//******************************************************************************************//
// displays the 4th choice of the 1st choice of the main menu
static UCHAR number_entry(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char = ch;
	switch (ch)
	{
		case KP_0:
			cursor_forward_stuff(0);
			break;
		case KP_1:
			cursor_forward_stuff(1);
			break;
		case KP_2:
			cursor_forward_stuff(2);
			break;
		case KP_3:
			cursor_forward_stuff(3);
			break;
		case KP_4:
			cursor_forward_stuff(4);
			break;
		case KP_5:
			cursor_forward_stuff(5);
			break;
		case KP_6:
			cursor_forward_stuff(6);
			break;
		case KP_7:
			cursor_forward_stuff(7);
			break;
		case KP_8:
			cursor_forward_stuff(8);
			break;
		case KP_9:
			cursor_forward_stuff(9);
			break;
		case KP_A:	// copy number from cur_global_number to new_global_number
			memcpy((void*)new_global_number,(void*)cur_global_number,sizeof(cur_global_number));
			prev_list();
			break;
		case KP_B:	// escape - return without changing new_global_number
			memset((void*)cur_global_number,0,sizeof(cur_global_number));
			prev_list();
			break;
		case KP_C:
			cursor_forward();
			break;
		case KP_D:
			cursor_backward();
			dispCharAt(cur_row,cur_col,0x20);
			cur_global_number[cur_col-NUM_ENTRY_BEGIN_COL] = 0x20;
			break;
		case KP_POUND:
			show_legend2(1,"alnum entry","pgup","pgdwn","up","down","choose","exit");
			show_alnum_list(1);
			set_list(ALNUM_ENTRY);
			break;
		case KP_AST:	// cancel changes and leave menu
//			show_legend2(1,"number entry","enter","cancel","forwd","backwd","alnum","exit");
			memset((void*)cur_global_number,0,sizeof(cur_global_number));
			prev_list();
			break;
		default:
			ret_char = ch;
			break;
	}
	return ret_char;
}
//******************************************************************************************//
//*************************************** alnum_entry **************************************//
//******************************************************************************************//
static UCHAR alnum_entry(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char = ch;
	switch (ch)
	{
		case KP_A:
			scroll_alnum_list(1,0);		// scroll page up
			break;
		case KP_B:
			scroll_alnum_list(0,0);		// scroll page down
			break;
		case KP_C:
			scroll_alnum_list(1,1);		// scroll up one
			break;
		case KP_D:
			scroll_alnum_list(0,1);		// scroll down one
			break;
		case KP_AST:
			// show the menu for the previous
			show_legend2(1,"number entry","enter","cancel","forwd","backwd","alnum","exit");
			show_alnum_list(0);
			prev_list();
			break;
		case KP_POUND:
			dispCharAt(cur_row,cur_col,choose_alnum);
			cur_global_number[cur_col-NUM_ENTRY_BEGIN_COL] = choose_alnum;
			cursor_forward();
			break;
		default:
			ret_char = ch;
			break;
	}
	return ret_char;
}
//******************************************************************************************//
//************************************* cursor_forward *************************************//
//******************************************************************************************//
static void cursor_forward(void)
{
	if(++cur_col > NUM_ENTRY_END_COL)
		cur_col = NUM_ENTRY_BEGIN_COL;
    dispSetCursor(TEXT_ON | CURSOR_BLINK_ON,cur_row,cur_col,LINE_8_CURSOR);
}
//******************************************************************************************//
//************************************* cursor_backward ************************************//
//******************************************************************************************//
static void cursor_backward(void)
{
	if(--cur_col < NUM_ENTRY_BEGIN_COL)
		cur_col = NUM_ENTRY_END_COL;
	dispSetCursor(TEXT_ON | CURSOR_BLINK_ON,cur_row,cur_col,LINE_8_CURSOR);
}
//******************************************************************************************//
//********************************** cursor_forward_stuff **********************************//
//******************************************************************************************//
static void cursor_forward_stuff(char x)
{
	stuff_num(x);
	cursor_forward();
}
//******************************************************************************************//
//*************************************** stuff_num ****************************************//
//******************************************************************************************//
static void stuff_num(char num)
{
	num += 0x30;
	dispCharAt(cur_row,cur_col,num);
	cur_global_number[cur_col-NUM_ENTRY_BEGIN_COL] = num;
}
//******************************************************************************************//
//************************************ clean_disp_num **************************************//
//******************************************************************************************//
static void clean_disp_num(void)
{
	int i;
	for(i = 0;i < NUM_ENTRY_SIZE+1;i++)
	{
		dispCharAt(cur_row,cur_col+i,0x20);
	}
}
//******************************************************************************************//
//********************************** scroll_alnum_list *************************************//
//******************************************************************************************//
static void scroll_alnum_list(int dir,int single)
{
	int i;
	UCHAR k;
	UCHAR row, col;
	int len = 1;
	int tscroll_ptr;
	row = NUM_ENTRY_ROW;
	col = NUM_ENTRY_BEGIN_COL - 2;
	if(single)
	{
		if(dir)
		{
			if(--scroll_ptr < 0)
				scroll_ptr = NUM_ALNUM - 1;
		}
		else
		{
			if(++scroll_ptr > NUM_ALNUM - 1)
				scroll_ptr = 0;
		}
		tscroll_ptr = scroll_ptr;
		for(i = 0;i < ALNUM_SCROLL_LIST_LEN;i++)
		{
			if(tscroll_ptr > NUM_ALNUM-1)
				tscroll_ptr = 0;
			k = alnum_array[tscroll_ptr++];
			if(i == 0)
				choose_alnum = k;
			dispCharAt(row,col,k);
			row++;
		}
//		scroll_ptr = tscroll_ptr - ALNUM_SCROLL_LIST_LEN - 1;
	}
	else
	{
		for(i = 0;i < ALNUM_SCROLL_LIST_LEN;i++)
		{
			k = alnum_array[scroll_ptr];
			if(i == 0)
				choose_alnum = k;
			dispCharAt(row,col,k);
			row++;
			if(dir)
			{
				if(--scroll_ptr == 0)
					scroll_ptr = NUM_ALNUM-1;
			}
			else
			{
				if(++scroll_ptr > NUM_ALNUM-1)
					scroll_ptr = 0;
			}
		}
	}
}
//******************************************************************************************//
//*********************************** show_alnum_list **************************************//
//******************************************************************************************//
static void show_alnum_list(int show)
{
	int i;
	UCHAR k;
	UCHAR row, col;
	row = NUM_ENTRY_ROW;
	col = NUM_ENTRY_BEGIN_COL - 2;
	if(show)
	{
		for(i = 0;i < ALNUM_SCROLL_LIST_LEN;i++)
		{
			k = alnum_array[i];
			if(i == 0)
				choose_alnum = k;
			dispCharAt(row,col,k);
			row++;
		}
	}
	else
	{
		k = 0x20;
		for(i = 0;i < ALNUM_SCROLL_LIST_LEN;i++)
		{
			dispCharAt(row,col,k);
			row++;
		}
	}
}

