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
static void scale_disp(int amt);
static void init_checkboxes(void);
static void scroll_checkboxes(void);
static void finish_checkboxes(void);
static void stop_rtdisplay(void);
static void blank_choices(void);
static void display_edit_value(void);
static UCHAR generic_menu_function(UCHAR ch, int  index);
static UCHAR backspace(UCHAR ch);
static UCHAR enter(UCHAR ch);
static UCHAR escape(UCHAR ch);
static UCHAR scr_alnum0(UCHAR ch);
static UCHAR scr_alnum1(UCHAR ch);
static UCHAR scr_alnum2(UCHAR ch);
static UCHAR scr_alnum3(UCHAR ch);
static UCHAR alnum_enter(UCHAR ch);
static UCHAR scrollup_checkboxes(UCHAR ch);
static UCHAR scrolldown_checkboxes(UCHAR ch);
static UCHAR toggle_checkboxes(UCHAR ch);
static UCHAR enter_checkboxes(UCHAR ch);
static UCHAR escape_checkboxes(UCHAR ch);
#endif
#if 1
static int first_menu = MAIN;
static int last_menu = testnum2;
static int current_fptr;		// pointer into the menu_list[]
static int prev_fptr;
static int list_size;
static int curr_type;
static int scroll_ptr;
static int cur_alnum_col;
static int dirty_flag;
static UCHAR cur_row, cur_col;	// used by the current menu/dialog function to keep track of the current row,col
static UCHAR alnum_array[NUM_ALNUM+1];
static UCHAR choose_alnum;
static int no_setlist;
#define LIST_SIZE 50
static void prev_list(void);
static int menu_list[LIST_SIZE];	// works like a stack for the how deep into the menus we go
#endif
static UCHAR (*fptr[NUM_FPTS])(UCHAR) = {enter, backspace, escape, scr_alnum0, \
		 scr_alnum1, scr_alnum2, scr_alnum3, cursor_forward, alnum_enter, scrollup_checkboxes, \
			scrolldown_checkboxes, toggle_checkboxes, enter_checkboxes, escape_checkboxes };
#if 1
//#ifndef MAIN_C
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
char *get_rt_label(int index)
{
	return rt_labels[index];
}
char *get_menu_label(int index)
{
	return menu_labels[index];
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
		strcpy(temp,menu_labels[menu_structs[index].label]);
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
	char blank[] = "               ";

	for(i = 0;i < no_rtparams;i++)
	{
		GDispStringAt(rt_params[i].row,rt_params[i].col,blank);
	}

	for(i = 0;i < no_rtparams;i++)
	{
		if(rt_params[i].shown == SHOWN_SENT)
		{
			strcpy(temp,get_rt_label(i));
			GDispStringAt(rt_params[i].row,rt_params[i].col,temp);
		}
	}
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
// reduce output to show just rpm, mph, engine temp and oil pressure at the top
// amt = SCALE_DISP_ALL - display all rt_params
// amt = SCALE_DISP_SOME - display limited rt_params
// amt = SCALE_DISP_NONE - don't display any rt_params
static void scale_disp(int amt)
{
	int i;
	char blank[] = "               ";

	scale_type = amt;
	switch (amt)
	{
		case SCALE_DISP_ALL:
			for(i = 0;i < no_rtparams;i++)
				if(rt_params[i].type == RT_AUX1 || rt_params[i].type == RT_AUX2)
					rt_params[i].shown = NOSHOWN_SENT;
				else	
					rt_params[i].shown = SHOWN_SENT;
			break;
		case SCALE_DISP_SOME:
			for(i = 0;i < no_rtparams;i++)
			{
				if(rt_params[i].type == RT_RPM || rt_params[i].type == RT_ENGT || \
						rt_params[i].type == RT_MPH || rt_params[i].type == RT_OILP)
					rt_params[i].shown = SHOWN_SENT;
				else if(rt_params[i].type == RT_AUX1 || rt_params[i].type == RT_AUX2)
					rt_params[i].shown = NOSHOWN_SENT;
				else	
					rt_params[i].shown = NOSHOWN_NOSENT;
					
				GDispStringAt(rt_params[i].row,rt_params[i].col,blank);
			}
			break;
		case SCALE_DISP_NONE:
			for(i = 0;i < no_rtparams;i++)
			{
				if(rt_params[i].type == RT_AUX1 || rt_params[i].type == RT_AUX2)
					rt_params[i].shown = NOSHOWN_SENT;
				else	
					rt_params[i].shown = NOSHOWN_NOSENT;
				GDispStringAt(rt_params[i].row,rt_params[i].col,blank);
			}
			break;
		default:
			break;
	}
	display_labels();
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

	alnum_array[i++] = 33;		// first one is a '!'
	for(k = 34;k < 48;k++)		// '"' - '/'	14
		alnum_array[i++] = k;
	alnum_array[i++] = 32;		// 'space'		1
	for(k = 58;k < 65;k++)		// ':' - '@'	7
		alnum_array[i++] = k;
	for(k = 91;k < 97;k++)		// '[' - '`'	6
		alnum_array[i++] = k;
	for(k = 123;k < 127;k++)	// '{' - '~'	4
		alnum_array[i++] = k;
	for(k = 65;k < 91;k++)		// 'A' - 'Z'	26
		alnum_array[i++] = k;
	for(k = 97;k < 123;k++)		// 'a' - 'z'	26	total: 85 (NUM_ALNUM)
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
	aux_type = 0;
	curr_checkbox = 0;
	last_checkbox = NUM_CHECKBOXES-1;
	scale_type = 0;
	prev_scale_type = 1;
	ask_data_ready = 0;
	new_data_ready = 0;
	data_entry_mode = 0;
	mod_data_ready = 0;
	cur_row = NUM_ENTRY_ROW;
	cur_col = NUM_ENTRY_BEGIN_COL;
	aux_index = 0;
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
	if(new_data_ready == 1)
	{
		data_entry_mode = 1;
		display_edit_value();
		new_data_ready = 0;
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
static void stop_rtdisplay(void)
{
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
			if(menu_structs[menu_index].menu == num_entry)
			{
				ret_char = escape(ret_char);
			}
			if(menu_structs[menu_index].menu == chkboxes)
			{
				ret_char = escape_checkboxes(ret_char);
			}
			else
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
			if(menu_structs[menu_index].menu == num_entry && menu_structs[menu_index].index > 0)
				aux_index = menu_structs[menu_index].index;

//			mvwprintw(win, DISP_OFFSET+19, 2,"aux_index: %d  ",aux_index);
			
			if(menu_structs[menu_index].menu == num_entry)
			{
				start_numentry();
				stop_rtdisplay();
			}
			if(menu_structs[menu_index].menu == chkboxes)
			{
				init_checkboxes();
				stop_rtdisplay();
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
/*
		mvwprintw(win, 27, 2,"index: %d\n",index);
		mvwprintw(win, 28, 2,"menu_index: %d\n",menu_index);
		mvwprintw(win, 29, 2,"menu in ms: %d\n",menu_structs[menu_index].menu);
		mvwprintw(win, 30, 2,"fptr in ms: %d\n",menu_structs[menu_index].fptr);
		mvwprintw(win, 31, 2,"current_fptr: %d\n",current_fptr);
		mvwprintw(win, 32, 2,"menu_list:  %d\n",menu_list[current_fptr]);

		for(i = 0;i < 10;i++)
			mvwprintw(win, 34+i, 2,"                ");
		for(i = 0;i < current_fptr;i++)
			mvwprintw(win, 34+i, 2," -> %s  ",menu_labels[menu_list[i]]);

		mvwprintw(win, 34+i, 2," -> %s  ",menu_labels[get_curr_menu()]);

		for(i = 0;i < current_fptr;i++)
			mvwprintw(win, DISP_OFFSET+20+i, 2," -> %s  ",menu_labels[menu_list[i]]);

		mvwprintw(win, DISP_OFFSET+20+i, 2," -> %s  ",menu_labels[get_curr_menu()]);
		mvwprintw(win, DISP_OFFSET+20+i+1, 2,"                                  ");
*/
#endif
	}
	no_setlist = 1;
	return ret_char;
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
//	scale_disp(SCALE_DISP_ALL);
	data_entry_mode = 0;
	mod_data_ready = 1;
	return ch;
}
//******************************************************************************************//
//******************************************* enter ****************************************//
//******************************************************************************************//
static UCHAR enter(UCHAR ch)
{
	int i = 0;
	int blank = 0;
	int limit;
	if(data_entry_mode)
	{	
/*
		do
		{
			if(cur_global_number[i] == 0x20)
				blank = i;
			i++;	
		}while(i < NUM_ENTRY_SIZE && blank == 0);
		for(i = blank;i< NUM_ENTRY_SIZE;i++)
			cur_global_number[i] = 0x20;
*/
		limit = atoi(cur_global_number);
		if(limit > 32767)
			strcpy(cur_global_number,"32766\0");
		memcpy((void*)new_global_number,(void*)cur_global_number,NUM_ENTRY_SIZE);
		cur_col = NUM_ENTRY_BEGIN_COL;
		prev_list();
		clean_disp_num();
		mod_data_ready = 1;
	}
#ifdef NOAVR
	mvwprintw(win, DISP_OFFSET+37, 2,"new: %s       %d    ",new_global_number,limit);
#endif
//	scale_disp(SCALE_DISP_ALL);
	return ch;
}
//******************************************************************************************//
//************************************** start_numentry ************************************//
//******************************************************************************************//
static void start_numentry(void)
{
#ifdef NOAVR
//	mvwprintw(win, 41, 3,"                                   ");
#endif
//	scale_disp(SCALE_DISP_SOME);
	cur_row = NUM_ENTRY_ROW;
	cur_col = NUM_ENTRY_BEGIN_COL;
	memset((void*)new_global_number,0,NUM_ENTRY_SIZE);
	memset((void*)cur_global_number,0,NUM_ENTRY_SIZE);
	clean_disp_num();
	dispCharAt(NUM_ENTRY_ROW,cur_col,'/');
	dispCharAt(NUM_ENTRY_ROW,cur_col+NUM_ENTRY_SIZE,'/');
//	set_list(NUM_ENTRY);
}
//******************************************************************************************//
//********************************* display_edit_value *************************************//
//******************************************************************************************//
static void display_edit_value(void)
{
	GDispStringAt(NUM_ENTRY_ROW,NUM_ENTRY_BEGIN_COL,cur_global_number);
	cur_col = strlen(cur_global_number)+NUM_ENTRY_BEGIN_COL;
//	dispCharAt(NUM_ENTRY_ROW,cur_col,'x');
    dispSetCursorX(TEXT_ON | CURSOR_BLINK_ON,NUM_ENTRY_ROW,cur_col,LINE_8_CURSOR);
#ifdef NOAVR
		dispCharAt(NUM_ENTRY_ROW+1,cur_col,95);
#endif
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
			dispCharAt(NUM_ENTRY_ROW,cur_col,choose_alnum);
		break;
		case 1:		// start at 'a'
			scroll_ptr = 0;
			while(alnum_array[++scroll_ptr] != 'a');
			choose_alnum = alnum_array[scroll_ptr];
			dispCharAt(NUM_ENTRY_ROW,cur_col,choose_alnum);
		break;
		case 2:		// start at '!' (very first one)
			scroll_ptr = 0;
//			while(alnum_array[++scroll_ptr] != '!');	// this will crash it
			choose_alnum = alnum_array[scroll_ptr];
			dispCharAt(NUM_ENTRY_ROW,cur_col,choose_alnum);
		break;
		case 3:		// advance the chosen character to next one
			if(++scroll_ptr > NUM_ALNUM-1)
				scroll_ptr = 0;
			choose_alnum = alnum_array[scroll_ptr];
			dispCharAt(NUM_ENTRY_ROW,cur_col,choose_alnum);
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
#ifdef NOAVR
		dispCharAt(NUM_ENTRY_ROW+1,cur_col,95);
#endif
	if(++cur_col > NUM_ENTRY_END_COL)
		cur_col = NUM_ENTRY_BEGIN_COL;
    dispSetCursorX(TEXT_ON | CURSOR_BLINK_ON,NUM_ENTRY_ROW,cur_col,LINE_8_CURSOR);
#ifdef NOAVR
		dispCharAt(NUM_ENTRY_ROW+1,cur_col,95);
#endif
//	mvwprintw(win, 45, 3,"cursor forward   ");
    return ch;
}
//******************************************************************************************//
//***************************************** backspace **************************************//
//******************************************************************************************//
static UCHAR backspace(UCHAR ch)
{
	if(data_entry_mode)
	{	
		cursor_backward();
		dispCharAt(NUM_ENTRY_ROW,cur_col,0x20);
		cur_global_number[cur_col-NUM_ENTRY_BEGIN_COL] = 0x20;
		mvwprintw(win, DISP_OFFSET+39,2,"bs:%s      %d  ",cur_global_number,cur_col);
	}
//	memset((void*)cur_global_number,0,NUM_ENTRY_SIZE);
	return ch;
}
//******************************************************************************************//
//************************************* cursor_backward ************************************//
//******************************************************************************************//
static void cursor_backward(void)
{
#ifdef NOAVR
		dispCharAt(NUM_ENTRY_ROW+1,cur_col,0x20);
#endif
	if(--cur_col < NUM_ENTRY_BEGIN_COL)
		cur_col = NUM_ENTRY_BEGIN_COL;
	dispSetCursorX(TEXT_ON | CURSOR_BLINK_ON,NUM_ENTRY_ROW,cur_col,LINE_8_CURSOR);
#ifdef NOAVR
		dispCharAt(NUM_ENTRY_ROW+1,cur_col,95);
#endif
}
//******************************************************************************************//
//********************************** cursor_forward_stuff **********************************//
//******************************************************************************************//
static void cursor_forward_stuff(char x)
{
	if(data_entry_mode)
	{
		stuff_num(x);
		cursor_forward(x);
#ifdef NOAVR
		mvwprintw(win, DISP_OFFSET+40, 2,"stuff: %d   %d  ",x,cur_col);
#endif
	}
}
//******************************************************************************************//
//*************************************** stuff_num ****************************************//
//******************************************************************************************//
static void stuff_num(char num)
{
	num += 0x30;
	dispCharAt(NUM_ENTRY_ROW,cur_col,num);
	cur_global_number[cur_col-NUM_ENTRY_BEGIN_COL] = num;
#ifdef NOAVR
	mvwprintw(win, DISP_OFFSET+38, 2,"cur: %s           %d   ",cur_global_number,cur_col);
#endif
}
#if 1	// scroll functions
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
	return ch;
}
#endif
//******************************************************************************************//
//************************************ clean_disp_num **************************************//
//******************************************************************************************//
static void clean_disp_num(void)
{
	int i;
	for(i = 0;i < NUM_ENTRY_SIZE+1;i++)
	{
		dispCharAt(NUM_ENTRY_ROW,i+NUM_ENTRY_BEGIN_COL,0x20);
		dispCharAt(NUM_ENTRY_ROW,i+NUM_ENTRY_BEGIN_COL+1,0x20);
	}
}
static void init_checkboxes(void)
{
	int i;
	UCHAR row, col;
	scale_disp(SCALE_DISP_SOME);
	row = 2;
	col = 3;
	curr_checkbox = 0;
	memset(check_boxes,0,sizeof(CHECKBOXES)*NUM_CHECKBOXES);

	strcpy(check_boxes[0].string,"choice 1\0");
	strcpy(check_boxes[1].string,"choice 2\0");
	strcpy(check_boxes[2].string,"choice 3\0");
	strcpy(check_boxes[3].string,"choice 4\0");
	strcpy(check_boxes[4].string,"choice 5\0");
	strcpy(check_boxes[5].string,"choice 6\0");
	strcpy(check_boxes[6].string,"choice 7\0");
	strcpy(check_boxes[7].string,"choice 8\0");
	strcpy(check_boxes[8].string,"choice 9\0");
	strcpy(check_boxes[9].string,"choice 10\0");

	for(i = 0;i < NUM_CHECKBOXES;i++)
	{
		check_boxes[i].index = i;
		GDispStringAt(row,col,check_boxes[i].string);
		row++;
	}
}
static UCHAR scrollup_checkboxes(UCHAR ch)
{
	if(--curr_checkbox < 0)
		curr_checkbox = last_checkbox;
	// move cursor to 3 + check_boxes[curr_checkbox]
//	dispCharAt(1+check_boxes[curr_checkbox].index,0,0x21);
	return ch;
}
static UCHAR scrolldown_checkboxes(UCHAR ch)
{
	if(++curr_checkbox > last_checkbox)
		curr_checkbox = 0;
	// move cursor
//	dispCharAt(1+check_boxes[curr_checkbox].index,0,0x21);
	return ch;
}
static UCHAR toggle_checkboxes(UCHAR ch)
{
	if(check_boxes[curr_checkbox].checked == 1)
	{
		check_boxes[curr_checkbox].checked = 0;
		dispCharAt(1+check_boxes[curr_checkbox].index,0,0x20);	// display 'blank'
	}
	else
	{
		check_boxes[curr_checkbox].checked = 1;
		dispCharAt(1+check_boxes[curr_checkbox].index,0,120);	// display 'x'
	}
	return ch;
}
static UCHAR enter_checkboxes(UCHAR ch)
{
	blank_choices();
	scale_disp(SCALE_DISP_ALL);
	prev_list();
	return ch;
}
static UCHAR escape_checkboxes(UCHAR ch)
{
	blank_choices();
	scale_disp(SCALE_DISP_ALL);
	prev_list();
	return ch;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static void blank_choices(void)
{
	int row,col,i;
	row = 3;
	col = 3;
	char blank[] = "                     ";
	for(i = 0;i < NUM_CHECKBOXES;i++)
	{
		GDispStringAt(row,col,blank);
		row++;
	}
}

