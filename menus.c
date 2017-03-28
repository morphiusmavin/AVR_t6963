#ifndef NOAVR
#warning "NOAVR"
#include <avr/io.h>
#include "../avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include <avr/eeprom.h>
#include "macros.h"
#endif
#include "sfr_helper.h"
#include <stdlib.h>
#include "main.h"
#include "t6963.h"
#include "USART.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//******************************************************************************************//
//*************************************** default_func *************************************//
//******************************************************************************************//
// for when no menu is shown
UCHAR main_menu_func(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col)
{
//	UCHAR row1,col1,k;
	UCHAR ret_char = ch;
//	printHexByte(ch);	
	switch (ch)
	{
		case KP_AST:	// '*'
			current_fptr = MENU1A;
//			printString("menu1a\r\n");
			break;
		case KP_0:	// '0'
			current_fptr = MENU1B;
//			printString("menu1b\r\n");
			break;
		case KP_POUND:	// '#'
			current_fptr = MENU1C;
//			printString("menu1c\r\n");
			break;
		case KP_D:	// 'D'
			current_fptr = MENU1D;
//			printString("menu1d\r\n");
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
UCHAR menu1a(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char = ch;
	switch (ch)
	{
		case KP_AST:
			GDispStringAt(12,0,"menu1a 1st choice");
			GDispStringAt(13,0,"go back          ");
//			printString("going back to main menu\r\n");
			current_fptr--;
			break;
		case KP_0:
			GDispStringAt(12,0,"menu1a 2nd choice");
			GDispStringAt(13,0,"displaying menu2a");
//			printString("menu2a\r\n");
			current_fptr = MENU2A;
			break;
		case KP_POUND:
			GDispStringAt(12,0,"menu1a 3rd choice");
			GDispStringAt(13,0,"displaying menu2b");
//			printString("menu2b\r\n");
			current_fptr = MENU2B;
			break;
		case KP_D:
			GDispStringAt(12,0,"menu1a 4th choice");
			GDispStringAt(13,0,"displaying menu2c");
//			printString("menu2c\r\n");
			current_fptr = MENU2C;
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
UCHAR menu1b(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char = ch;
	switch (ch)
	{
		case KP_AST:
			GDispStringAt(12,0,"menu1b 1st choice");
			GDispStringAt(13,0,"go back          ");
//			printString("going back to main menu\r\n");
			current_fptr--;
			break;
		case KP_0:
			GDispStringAt(12,0,"menu1b 2nd choice");
			GDispStringAt(13,0,"displaying menu3a");
//			printString("menu3a\r\n");
			current_fptr = MENU3A;
			break;
		case KP_POUND:
			GDispStringAt(12,0,"menu1b 3rd choice");
			GDispStringAt(13,0,"displaying menu3b");
//			printString("menu3b\r\n");
			current_fptr = MENU3B;
			break;
		case KP_D:
			GDispStringAt(12,0,"menu1b 4th choice");
			GDispStringAt(13,0,"displaying menu3c");
//			printString("menu3c\r\n");
			current_fptr = MENU3C;
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
UCHAR menu1c(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char = ch;
	switch (ch)
	{
		case KP_AST:
			GDispStringAt(12,0,"menu1c 1st choice");
			GDispStringAt(13,0,"go back          ");
//			printString("going back to main menu\r\n");
			current_fptr--;
			break;
		case KP_0:
			GDispStringAt(12,0,"menu1c 2nd choice");
			GDispStringAt(13,0,"displaying menu4a");
//			printString("menu4a\r\n");
			current_fptr = MENU4A;
			break;
		case KP_POUND:
			GDispStringAt(12,0,"menu1c 3rd choice");
			GDispStringAt(13,0,"displaying menu4b");
//			printString("menu4b\r\n");
			current_fptr = MENU4B;
			break;
		case KP_D:
			GDispStringAt(12,0,"menu1c 4th choice");
			GDispStringAt(13,0,"displaying menu4c");
//			printString("menu4c\r\n");
			current_fptr = MENU4C;
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
UCHAR menu1d(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char = ch;
	switch (ch)
	{
		case KP_AST:
			GDispStringAt(12,0,"menu1d 1st choice");
			GDispStringAt(13,0,"go back          ");
//			printString("going back to main menu\r\n");
			current_fptr--;
			break;
		case KP_0:
			GDispStringAt(12,0,"menu1d 2nd choice");
			GDispStringAt(13,0,"displaying main menu");
//			printString("main menu\r\n");
			current_fptr = MAIN_MENU;
			break;
		case KP_POUND:
			GDispStringAt(12,0,"menu1d 3rd choice");
			GDispStringAt(13,0,"displaying main menu");
//			printString("main menu\r\n");
			current_fptr = MAIN_MENU;
			break;
		case KP_D:
			GDispStringAt(12,0,"menu1d 4th choice");
			GDispStringAt(13,0,"displaying main menu");
//			printString("main menu\r\n");
			current_fptr = MAIN_MENU;
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
UCHAR menu2a(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char = ch;
	switch (ch)
	{
		case KP_AST:
			GDispStringAt(12,0,"menu2a 1st choice");
			GDispStringAt(13,0,"go back          ");
//			printString("going back to menu1a\r\n");
			current_fptr--;
			break;
		case KP_0:
			GDispStringAt(12,0,"menu2a 2nd choice");
			GDispStringAt(13,0,"displaying main menu");
//			printString("main menu\r\n");
			current_fptr = MAIN_MENU;
			break;
		case KP_POUND:
			GDispStringAt(13,0,"menu2a 3rd choice");
			GDispStringAt(14,0,"number entry     ");
			cur_row = NUM_ENTRY_ROW;
			cur_col = NUM_ENTRY_BEGIN_COL;
			memset((void*)new_global_number,0,sizeof(new_global_number));
//			printString("menu\r\n");
			current_fptr = NUM_ENTRY;
			break;
		case KP_D:
			GDispStringAt(12,0,"menu2a 4th choice");
			GDispStringAt(13,0,"displaying main menu");
//			printString("main menu\r\n");
			current_fptr = MAIN_MENU;
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
UCHAR menu2b(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char = ch;
	switch (ch)
	{
		case KP_AST:
			GDispStringAt(12,0,"menu2b 1st choice");
			GDispStringAt(13,0,"go back          ");
//			printString("going back to menu1a\r\n");
			current_fptr--;
			break;
		case KP_0:
			GDispStringAt(12,0,"menu2b 2nd choice");
			GDispStringAt(13,0,"displaying main menu");
//			printString("main menu\r\n");
			current_fptr = MAIN_MENU;
			break;
		case KP_POUND:
			GDispStringAt(12,0,"menu2b 3rd choice");
			GDispStringAt(13,0,"displaying main menu");
//			printString("main menu\r\n");
			current_fptr = MAIN_MENU;
			break;
		case KP_D:
			GDispStringAt(12,0,"menu2b 4th choice");
			GDispStringAt(13,0,"displaying main menu");
//			printString("main menu\r\n");
			current_fptr = MAIN_MENU;
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
UCHAR menu2c(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char = ch;
	switch (ch)
	{
		case KP_AST:
			GDispStringAt(12,0,"menu2c 1st choice");
			GDispStringAt(13,0,"go back          ");
//			printString("going back to menu1a\r\n");
			current_fptr--;
			break;
		case KP_0:
			GDispStringAt(12,0,"menu2c 2nd choice");
			GDispStringAt(13,0,"displaying main menu");
//			printString("main menu\r\n");
			current_fptr = MAIN_MENU;
			break;
		case KP_POUND:
			GDispStringAt(12,0,"menu2c 3rd choice");
			GDispStringAt(13,0,"displaying main menu");
//			printString("main menu\r\n");
			current_fptr = MAIN_MENU;
			break;
		case KP_D:
			GDispStringAt(12,0,"menu2c 4th choice");
			GDispStringAt(13,0,"displaying main menu");
//			printString("main menu\r\n");
			current_fptr = MAIN_MENU;
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
UCHAR menu3a(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char = ch;
	switch (ch)
	{
		case KP_AST:
			GDispStringAt(12,0,"menu3a 1st choice");
			GDispStringAt(13,0,"go back          ");
//			printString("menu\r\n");
			current_fptr--;
			break;
		case KP_0:
			GDispStringAt(12,0,"menu3a 2nd choice");
			GDispStringAt(13,0,"displaying menu3a");
//			printString("menu\r\n");
			current_fptr = MAIN_MENU;
			break;
		case KP_POUND:
			GDispStringAt(13,0,"menu3a 3rd choice");
			GDispStringAt(14,0,"number entry     ");
			cur_row = NUM_ENTRY_ROW;
			cur_col = NUM_ENTRY_BEGIN_COL;
			memset((void*)new_global_number,0,sizeof(new_global_number));
//			printString("menu\r\n");
			current_fptr = NUM_ENTRY;
			break;
		case KP_D:
			GDispStringAt(12,0,"menu3a 4th choice");
			GDispStringAt(13,0,"displaying menu3a");
//			printString("menu\r\n");
			current_fptr = MAIN_MENU;
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
UCHAR menu3b(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char = ch;
	switch (ch)
	{
		case KP_AST:
			GDispStringAt(12,0,"menu3b 1st choice");
			GDispStringAt(13,0,"go back          ");
//			printString("menu\r\n");
			current_fptr--;
			break;
		case KP_0:
			GDispStringAt(12,0,"menu3b 2nd choice");
			GDispStringAt(13,0,"displaying menu3b");
//			printString("menu\r\n");
			current_fptr = MAIN_MENU;
			break;
		case KP_POUND:
			GDispStringAt(12,0,"menu3b 3rd choice");
			GDispStringAt(13,0,"displaying menu3b");
//			printString("menu\r\n");
			current_fptr = MAIN_MENU;
			break;
		case KP_D:
			GDispStringAt(12,0,"menu3b 4th choice");
			GDispStringAt(13,0,"displaying menu3b");
//			printString("menu\r\n");
			current_fptr = MAIN_MENU;
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
UCHAR menu3c(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char = ch;
	switch (ch)
	{
		case KP_AST:
			GDispStringAt(12,0,"menu3c 1st choice");
			GDispStringAt(13,0,"go back          ");
//			printString("menu\r\n");
			current_fptr--;
			break;
		case KP_0:
			GDispStringAt(12,0,"menu3c 2nd choice");
			GDispStringAt(13,0,"displaying menu3c");
//			printString("menu\r\n");
			current_fptr = MAIN_MENU;
			break;
		case KP_POUND:
			GDispStringAt(12,0,"menu3c 3rd choice");
			GDispStringAt(13,0,"displaying menu3c");
//			printString("menu\r\n");
			current_fptr = MAIN_MENU;
			break;
		case KP_D:
			GDispStringAt(12,0,"menu3c 4th choice");
			GDispStringAt(13,0,"displaying menu3c");
//			printString("menu\r\n");
			current_fptr = MAIN_MENU;
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
UCHAR menu4a(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char = ch;
	switch (ch)
	{
		case KP_AST:
			GDispStringAt(12,0,"menu4a 1st choice");
			GDispStringAt(13,0,"go back          ");
//			printString("menu\r\n");
			current_fptr--;
			break;
		case KP_0:
			GDispStringAt(12,0,"menu4a 2nd choice");
			GDispStringAt(13,0,"displaying menu4a");
//			printString("menu\r\n");
			current_fptr = MAIN_MENU;
			break;
		case KP_POUND:
			GDispStringAt(13,0,"menu4a 3rd choice");
			GDispStringAt(13,0,"displaying menu4a");
//			printString("menu\r\n");
			current_fptr = MAIN_MENU;
			break;
		case KP_D:
			GDispStringAt(12,0,"menu4a 4th choice");
			GDispStringAt(13,0,"displaying menu4a");
//			printString("menu\r\n");
			current_fptr = MAIN_MENU;
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
UCHAR menu4b(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char = ch;
	switch (ch)
	{
		case KP_AST:
			GDispStringAt(12,0,"menu4b 1st choice");
			GDispStringAt(13,0,"go back          ");
//			printString("menu\r\n");
			current_fptr--;
			break;
		case KP_0:
			GDispStringAt(12,0,"menu4b 2nd choice");
			GDispStringAt(13,0,"displaying menu4b");
//			printString("menu\r\n");
			current_fptr = MAIN_MENU;
			break;
		case KP_POUND:
			GDispStringAt(12,0,"menu4b 3rd choice");
			GDispStringAt(13,0,"displaying menu4b");
//			printString("menu\r\n");
			current_fptr = MAIN_MENU;
			break;
		case KP_D:
			GDispStringAt(12,0,"menu4b 4th choice");
			GDispStringAt(13,0,"displaying menu4b");
//			printString("menu\r\n");
			current_fptr = MAIN_MENU;
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
UCHAR menu4c(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char = ch;
	switch (ch)
	{
		case KP_AST:
			GDispStringAt(12,0,"menu4c 1st choice");
			GDispStringAt(13,0,"go back          ");
//			printString("menu\r\n");
			current_fptr--;
			break;
		case KP_0:
			GDispStringAt(12,0,"menu4c 2nd choice");
			GDispStringAt(13,0,"displaying menu4c");
//			printString("menu\r\n");
			current_fptr = MAIN_MENU;
			break;
		case KP_POUND:
			GDispStringAt(12,0,"menu4c 3rd choice");
			GDispStringAt(13,0,"displaying menu4c");
//			printString("menu\r\n");
			current_fptr = MAIN_MENU;
			break;
		case KP_D:
			GDispStringAt(12,0,"menu4c 4th choice");
			GDispStringAt(13,0,"displaying menu4c");
//			printString("menu\r\n");
			current_fptr = MAIN_MENU;
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
UCHAR number_entry(UCHAR ch, UCHAR limit8, UINT limit16, UCHAR row, UCHAR col)
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
			current_fptr--;
			break;
		case KP_B:	// escape - return without changing new_global_number
			memset((void*)cur_global_number,0,sizeof(cur_global_number));
			current_fptr--;
			break;
		case KP_C:
			break;
		case KP_D:
			cursor_forward();
			break;
		case KP_POUND:
			cursor_backward();
			break;
		case KP_AST:	// cancel changes and leave menu
			memset((void*)cur_global_number,0,sizeof(cur_global_number));
			current_fptr--;
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
void cursor_forward(void)
{
	if(++cur_col > NUM_ENTRY_END_COL)
		cur_col = NUM_ENTRY_BEGIN_COL;
    dispSetCursor(TEXT_ON | CURSOR_BLINK_ON,cur_row,cur_col,LINE_8_CURSOR);
}
//******************************************************************************************//
//************************************* cursor_backward ************************************//
//******************************************************************************************//
void cursor_backward(void)
{
	if(--cur_col < NUM_ENTRY_BEGIN_COL)
		cur_col = NUM_ENTRY_END_COL;
	dispSetCursor(TEXT_ON | CURSOR_BLINK_ON,cur_row,cur_col,LINE_8_CURSOR);
}
//******************************************************************************************//
//********************************** cursor_forward_stuff **********************************//
//******************************************************************************************//
void cursor_forward_stuff(char x)
{
	stuff_num(x);
	cursor_forward();
}
//******************************************************************************************//
//*************************************** stuff_num ****************************************//
//******************************************************************************************//
void stuff_num(char num)
{
	num += 0x30;
	dispCharAt(cur_row,cur_col,num);
	cur_global_number[cur_col-NUM_ENTRY_BEGIN_COL] = num;
}		


