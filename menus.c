// menus.c
#ifdef MAIN_C
#warning "MAIN_C defined"
#else
#warning "MAIN_C not defined in menus.c"
#endif
#ifndef NOAVR
#include <avr/io.h>
#include "../avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include <avr/eeprom.h>
#include "macros.h"
#else
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
#ifdef NOAVR
void dispSetCursorX(UCHAR mode, UCHAR row, UCHAR col, UCHAR type)
{
	cursor_row = row;
	cursor_col = col;
}
WINDOW *win;
void set_win2(WINDOW *win)
{
	win = win;
}
#else
extern char eepromString[STRING_LEN] EEMEM;
#define dispSetCursorX(mode, row, col, type) dispSetCursor(mode, row, col, type)
#endif
#if 1
static void clean_disp_num(void);
static UCHAR cursor_forward(UCHAR ch);
static void cursor_backward(void);
static void cursor_forward_stuff(char);
static void stuff_num(char);
static void scroll_alnum_list(int dir);
static void blank_menu(void);
static void adv_menu_label(int index, UCHAR *row, UCHAR *col);
static void display_menus(int index);
static void start_numentry(void);

static UCHAR generic_menu_function(UCHAR ch, int  index);
static UCHAR backspace(UCHAR ch);
static UCHAR enter(UCHAR ch);
static UCHAR escape(UCHAR ch);
static UCHAR call_alnum(UCHAR ch);
static UCHAR scr_alnum0(UCHAR ch);
static UCHAR scr_alnum1(UCHAR ch);
static UCHAR scr_alnum2(UCHAR ch);
static UCHAR scr_alnum3(UCHAR ch);
static UCHAR alnum_enter(UCHAR ch);
static UCHAR check_box(UCHAR ch);
#endif
#if 1
static int first_menu = MAIN;
static int last_menu = MENU4C;
static int current_fptr;		// pointer into the menu_list[]
static int prev_fptr;
static int list_size;
static int curr_type;
static int scroll_ptr;
static int cur_alnum_col;
static int dirty_flag;
static UCHAR cur_row, cur_col;	// used by the current menu/dialog function to keep track of the current row,col
static UCHAR alnum_array[NUM_ALNUM];
static UCHAR choose_alnum;
static int no_setlist;
#define LIST_SIZE 50
static void prev_list(void);
static int menu_list[LIST_SIZE];	// works like a stack for the how deep into the menus we go
#endif
static UCHAR (*fptr[NUM_FPTS])(UCHAR) = {enter, backspace, escape, scr_alnum0, \
		 scr_alnum1, scr_alnum2, scr_alnum3, cursor_forward, alnum_enter};
#if 1
//#ifndef MAIN_C
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
char *get_label(int index)
{
	return labels[index];
}
UCHAR get_row(int index)
{
//	return menu_structs[index].row;
	return 0;
}
UCHAR get_col(int index)
{
//	return menu_structs[index].col;
	return 0;
}
//#endif
#endif
//******************************************************************************************//
//************************************ adv_menu_label **************************************//
//******************************************************************************************//
void adv_menu_label(int index, UCHAR *row, UCHAR *col)
{
//	int menu_index = index * 6;
	char temp[MAX_LABEL_LEN];
	char temp2[4];
	switch (index % 6)
	{
		case 0: strncpy(temp2," A:\0",4);break;
		case 1: strncpy(temp2," B:\0",4);break;
		case 2: strncpy(temp2," C:\0",4);break;
		case 3: strncpy(temp2," D:\0",4);break;
		case 4: strncpy(temp2," #:\0",4);break;
		case 5: strncpy(temp2," 0:\0",4);break;
		default:strncpy(temp2,"\0",4);break;
	}

	if(menu_structs[index].enabled)
	{
		strcpy(temp,labels[menu_structs[index].label+no_rtparams]);
		GDispStringAt(*row,*col,temp2);
		GDispStringAt(*row,*col+3,temp);

//printf("%d %d %d %s\n",index,*row,*col,temp);

		if(*col > MAX_LABEL_LEN*2-1)
		{
			*col = 0;
			(*row)++;
		}
		else
			*col += MAX_LABEL_LEN;
	}
}
//******************************************************************************************//
//************************************* display_menus **************************************//
//******************************************************************************************//
// display a different menu
// should be able to get the label by indexing into the labels area by:
// (no_rtparams*sizeof(int) + get_type()*sizeof(int)) * menu_struct[i].ch_type
static void display_menus(int index)
{
	int i;
	UCHAR row,col;
	row = MENU_START_ROW;
	col = 0;
	index *= 6;
//	mvwprintw(win, 42, 3,"index: %d",index);
	blank_menu();
	adv_menu_label(index,&row,&col);
	adv_menu_label(index+1,&row,&col);
	adv_menu_label(index+2,&row,&col);
	adv_menu_label(index+3,&row,&col);
	adv_menu_label(index+4,&row,&col);
	adv_menu_label(index+5,&row,&col);
//	mvwprintw(win, 45, 25,"index: %d ",index);
#ifdef NOAVR
//	mvwprintw(win, 45, 3, "current menu: %s  ",labels[get_curr_menu()+no_rtparams]);
#endif
}
//******************************************************************************************//
//**************************************** display_labels **********************************//
//******************************************************************************************//
// displays the rt_labels if shown is set
void display_labels(void)
{
	int i,j;
	char temp[MAX_LABEL_LEN];
	char blank[] = "         ";
	char *ch;

	for(i = 0;i < no_rtparams;i++)
	{
		GDispStringAt(rt_params[i].row,rt_params[i].col,blank);
	}

	for(i = 0;i < no_rtparams;i++)
	{
		if(rt_params[i].shown == 1)
		{
			strcpy(temp,get_label(i));
		}
		GDispStringAt(rt_params[i].row,rt_params[i].col,temp);
	}
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static void blank_menu(void)
{
	int row,col;
	for(row = MENU_START_ROW-1;row < 15;row++)
		for(col = 0;col < COLUMN+1;col++)
			dispCharAt(row,col,0x20);
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
void init_list(void)
{
	int i = 0;
	UCHAR k;

//	GDispStringAt(5,5,"test1");

	alnum_array[i++] = 0x20;		// first one is a space
	for(k = 33;k < 48;k++)		// '!' - '/'	15
		alnum_array[i++] = k;
	for(k = 58;k < 65;k++)		// ':' - '@'	7
		alnum_array[i++] = k;
	for(k = 91;k < 97;k++)		// '[' - '`'	6
		alnum_array[i++] = k;
	for(k = 123;k < 127;k++)	// '{' - '~'	4
		alnum_array[i++] = k;
	for(k = 65;k < 91;k++)		// 'A' - 'Z'	26
		alnum_array[i++] = k;
	for(k = 97;k < 123;k++)		// 'a' - 'z'	26	total: 85
		alnum_array[i++] = k;
	choose_alnum = 0x30;
	scroll_ptr = 25;		// start showing at 'a' (skip '!' - '9')
	cur_alnum_col = NUM_ENTRY_BEGIN_COL;
	list_size = LIST_SIZE;
	current_fptr = first_menu;
	prev_fptr = first_menu;
	memset(menu_list,0,sizeof(menu_list));
	menu_list[0] = current_fptr;
	curr_type = MAIN;
	dirty_flag = 0;
	display_menus(0);
	no_setlist = 1;
//	show_legend(1,"main","1a","1b","1c","1d");
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static void prev_list(void)
{
	if(current_fptr == 0)
		return;
	else
	{
		menu_list[current_fptr] = 0;
		current_fptr--;
		dirty_flag = 1;
	}
	no_setlist = 1;
//	return menu_list[current_fptr];
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static void set_list(int fptr)
{
	if(current_fptr < list_size)
	{
		current_fptr++;
		menu_list[current_fptr] = fptr;
		dirty_flag = 1;
	}
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
UCHAR get_key(UCHAR ch)
{
	UCHAR wkey;
	int i,j;
	UCHAR choice_array[6];
	j = 0;
	UCHAR ret_char = generic_menu_function(ch,get_curr_menu());
	if(curr_fptr_changed())
	{
		display_menus(get_curr_menu());
	}
	return ret_char;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
int get_curr_menu(void)
{
	return menu_list[current_fptr];
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
int curr_fptr_changed(void)
{
	int flag = dirty_flag;
	dirty_flag = 0;
	return flag;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
int get_str_len(void)
{
	return cur_col-NUM_ENTRY_BEGIN_COL;
}
//******************************************************************************************//
//********************************** scroll_alnum_list *************************************//
//******************************************************************************************//
static UCHAR check_box(UCHAR ch)
{
	return ch;
}
//******************************************************************************************//
//********************************* generic_menu_function **********************************//
//******************************************************************************************//
static UCHAR generic_menu_function(UCHAR ch, int  index)
{
	UCHAR ret_char = ch;
	int menu_index = index * 6;
	int i;
	
	switch (ch)
	{
		case KP_0:
			if(menu_structs[menu_index+5].enabled)
			{
				menu_index += 5;
				no_setlist = 0;
			}
			else cursor_forward_stuff(0);
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
		case KP_A:
			no_setlist = 0;
			break;
		case KP_B:
			menu_index++;
			no_setlist = 0;
			break;
		case KP_C:
			menu_index += 2;
			no_setlist = 0;
			break;
		case KP_D:
			menu_index += 3;
			no_setlist = 0;
			break;
		case KP_POUND:
			menu_index += 4;
			no_setlist = 0;
			break;
		case KP_AST:
			prev_list();
//			mvwprintw(win, 41, 3, "ast");
			break;
		default:
			break;
	}
	if(no_setlist == 0 && menu_structs[menu_index].enabled)
	{
		if(menu_structs[menu_index].fptr == 0)
		{
			set_list(menu_structs[menu_index].menu);
			if(menu_structs[menu_index].menu == num_entry)
			{
				start_numentry();
//				mvwprintw(win, 45, 3,"start_num_entry");
			}
		}
		else
		{
			ret_char = (*fptr[menu_structs[menu_index].fptr-last_menu-1])(ch);
//			mvwprintw(win, 39, 3,"               ");
		}
	}
	if(no_setlist == 1)
	{
#ifdef NOAVR

		mvwprintw(win, 27, 2,"index: %d\n",index);
		mvwprintw(win, 28, 2,"menu_index: %d\n",menu_index);
		mvwprintw(win, 29, 2,"menu in ms: %d\n",menu_structs[menu_index].menu);
		mvwprintw(win, 30, 2,"fptr in ms: %d\n",menu_structs[menu_index].fptr);
		mvwprintw(win, 31, 2,"current_fptr: %d\n",current_fptr);
		mvwprintw(win, 32, 2,"menu_list:  %d\n",menu_list[current_fptr]);

		for(i = 0;i < 10;i++)
			mvwprintw(win, 34+i, 2,"                ");
		for(i = 0;i < current_fptr;i++)
			mvwprintw(win, 34+i, 2," -> %s  ",labels[menu_list[i]+no_rtparams]);
	
		mvwprintw(win, 34+i, 2," -> %s  ",labels[get_curr_menu()+no_rtparams]);

#endif
	}
	no_setlist = 1;
	return ret_char;
}
//******************************************************************************************//
//***************************************** backspace **************************************//
//******************************************************************************************//
static UCHAR backspace(UCHAR ch)
{
	cursor_backward();
	dispCharAt(cur_row,cur_col,0x20);
	cur_global_number[cur_col-NUM_ENTRY_BEGIN_COL] = 0x20;
//	memset((void*)cur_global_number,0,NUM_ENTRY_SIZE);
	return ch;
}
//******************************************************************************************//
//******************************************* escape ***************************************//
//******************************************************************************************//
static UCHAR escape(UCHAR ch)
{
	memset((void*)cur_global_number,0,NUM_ENTRY_SIZE);
	cur_col = NUM_ENTRY_BEGIN_COL;
	prev_list();
	clean_disp_num();
	return ch;
}
//******************************************************************************************//
//******************************************* enter ****************************************//
//******************************************************************************************//
static UCHAR enter(UCHAR ch)
{
	memcpy((void*)new_global_number,(void*)cur_global_number,NUM_ENTRY_SIZE);
	cur_col = NUM_ENTRY_BEGIN_COL;
	prev_list();
	clean_disp_num();
#ifdef NOAVR	
	mvwprintw(win, 45, 3,"num entered: %s ",new_global_number);
#endif
	return ch;
}
static UCHAR call_alnum(UCHAR ch)
{
#ifdef NOAVR
	mvwprintw(win, 45, 3, "call_alnum %x  ",ch);
#endif
	return ch;
}
//******************************************************************************************//
//************************************** start_numentry ************************************//
//******************************************************************************************//
static void start_numentry(void)
{
//	show_legend2(1,"number entry","forward","back","alpha","enter","cancel","cancel");
	cur_row = NUM_ENTRY_ROW;
	cur_col = NUM_ENTRY_BEGIN_COL;
	memset((void*)new_global_number,0,NUM_ENTRY_SIZE);
	memset((void*)cur_global_number,0,NUM_ENTRY_SIZE);
	clean_disp_num();
	dispCharAt(cur_row,cur_col+NUM_ENTRY_SIZE,'/');
//	set_list(NUM_ENTRY);
}
//******************************************************************************************//
//********************************** scroll_alnum_list *************************************//
//******************************************************************************************//
static void scroll_alnum_list(int dir)
{
	int i;
	UCHAR k;
	UCHAR row, col;
	int len = 1;
	row = NUM_ENTRY_ROW;
	col = NUM_ENTRY_BEGIN_COL;
	switch (dir)
	{
		case 0:		// start at 'A'
			scroll_ptr = 0;
			while(alnum_array[++scroll_ptr] != 'A');
			choose_alnum = alnum_array[scroll_ptr];
			dispCharAt(cur_row,cur_col,choose_alnum);
		break;
		case 1:		// start at 'a'
			scroll_ptr = 0;
			while(alnum_array[++scroll_ptr] != 'a');
			choose_alnum = alnum_array[scroll_ptr];
			dispCharAt(cur_row,cur_col,choose_alnum);
		break;
		case 2:		// start at '!' (very first one)
			scroll_ptr = 0;
//			while(alnum_array[++scroll_ptr] != '!');	// this will crash it
			choose_alnum = alnum_array[scroll_ptr];
			dispCharAt(cur_row,cur_col,choose_alnum);
		break;
		case 3:		// advance the chosen character to next one
			if(++scroll_ptr > NUM_ALNUM-1)
				scroll_ptr = 0;
			choose_alnum = alnum_array[scroll_ptr];
			dispCharAt(cur_row,cur_col,choose_alnum);
//			mvwprintw(win, 45, 3,"next char in list");
		break;
		default:
		break;
	}
	cur_global_number[cur_col-NUM_ENTRY_BEGIN_COL] = choose_alnum;
}
//******************************************************************************************//
//************************************* cursor_forward *************************************//
//******************************************************************************************//
static UCHAR cursor_forward(UCHAR ch)
{
	if(++cur_col > NUM_ENTRY_END_COL)
		cur_col = NUM_ENTRY_BEGIN_COL;
    dispSetCursorX(TEXT_ON | CURSOR_BLINK_ON,cur_row,cur_col,LINE_8_CURSOR);
//	mvwprintw(win, 45, 3,"cursor forward   ");
    return ch;
}
//******************************************************************************************//
//************************************* cursor_backward ************************************//
//******************************************************************************************//
static void cursor_backward(void)
{
	if(--cur_col < NUM_ENTRY_BEGIN_COL)
		cur_col = NUM_ENTRY_END_COL;
	dispSetCursorX(TEXT_ON | CURSOR_BLINK_ON,cur_row,cur_col,LINE_8_CURSOR);
}
//******************************************************************************************//
//********************************** cursor_forward_stuff **********************************//
//******************************************************************************************//
static void cursor_forward_stuff(char x)
{
	stuff_num(x);
	cursor_forward(x);
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
static UCHAR scr_alnum0(UCHAR ch)
{
	scroll_alnum_list(0);		// CAPS
	return ch;
}
static UCHAR scr_alnum1(UCHAR ch)
{
	scroll_alnum_list(1);		// small
	return ch;
}
static UCHAR scr_alnum2(UCHAR ch)
{
	scroll_alnum_list(2);		// special
	return ch;
}
static UCHAR scr_alnum3(UCHAR ch)
{
	scroll_alnum_list(3);		// next char in list
	return ch;
}
static UCHAR alnum_enter(UCHAR ch)
{
	cur_global_number[cur_col-NUM_ENTRY_BEGIN_COL] = choose_alnum;
	cursor_forward(ch);
	prev_list();
//	mvwprintw(win, 45, 3,"alnum enter    ");
	return ch;
}
#if 1
//******************************************************************************************//
//************************************ clean_disp_num **************************************//
//******************************************************************************************//
static void clean_disp_num(void)
{
	int i;
	for(i = 0;i < NUM_ENTRY_SIZE+1;i++)
	{
		dispCharAt(NUM_ENTRY_ROW,i+NUM_ENTRY_BEGIN_COL,0x20);
	}
}
#endif
#if 0
//******************************************************************************************//
//*************************************** number_entry *************************************//
//******************************************************************************************//
// displays the 4th choice of the 1st choice of the main menu
// A - "forward", B - "back", C - "alpha", D - "enter", # - "cancel", * - "cancel");
static UCHAR number_entry(UCHAR ch)
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
		case KP_A:
			cursor_forward(ch);
			break;
		case KP_B: // replaced by 'backspace'
			cursor_backward();
			dispCharAt(cur_row,cur_col,0x20);
			cur_global_number[cur_col-NUM_ENTRY_BEGIN_COL] = 0x20;
			memset((void*)cur_global_number,0,NUM_ENTRY_SIZE);
			break;
		case KP_C:
//			show_legend2(1,"alpha entry","CAPS","small","special","next","forward","apply");
			set_list(ALNUM_ENTRY);
			break;
		case KP_D:	// replace by 'enter'
			memcpy((void*)new_global_number,(void*)cur_global_number,NUM_ENTRY_SIZE);
			cur_col = NUM_ENTRY_BEGIN_COL;
			prev_list();
			clean_disp_num();
			break;
		case KP_POUND:	// replaced by 'escape'
			memset((void*)cur_global_number,0,NUM_ENTRY_SIZE);
			cur_col = NUM_ENTRY_BEGIN_COL;
			prev_list();
			clean_disp_num();
			break;
		case KP_AST:	// cancel changes and leave menu
			memset((void*)cur_global_number,0,NUM_ENTRY_SIZE);
			cur_col = NUM_ENTRY_BEGIN_COL;
			prev_list();
			clean_disp_num();
			break;
		default:
			ret_char = ch;
			break;
	}
	return ret_char;
}
#endif
#if 0
//******************************************************************************************//
//*************************************** alnum_entry **************************************//
//******************************************************************************************//
//A - "CAPS", B - "small", C - "special", D - "next", # - "forward", * - "apply");
static UCHAR alnum_entry(UCHAR ch)
{
	UCHAR ret_char = ch;
	switch (ch)
	{
		case KP_A:
			scroll_alnum_list(0);		// CAPS
			break;
		case KP_B:
			scroll_alnum_list(1);		// small
			break;
		case KP_C:
			scroll_alnum_list(2);		// special
			break;
		case KP_D:
			scroll_alnum_list(3);		// next char in list
			break;
		case KP_POUND:
			cursor_forward(ch);			// move cursor forward
			break;
		case KP_AST:
			// show the menu for the previous
//			show_legend2(1,"number entry","forward","back","alpha","enter","cancel","cancel");
			cur_global_number[cur_col-NUM_ENTRY_BEGIN_COL] = choose_alnum;
			cursor_forward(ch);
			prev_list();
			break;
		default:
			ret_char = ch;
			break;
	}
	return ret_char;
}
#endif

