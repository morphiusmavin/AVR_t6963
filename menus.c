#include <avr/io.h>
#include "../avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include "sfr_helper.h"
#include <avr/eeprom.h>
#include <stdlib.h>
#include "USART.h"
#include "t6963.h"
#include "macros.h"
#include "main.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
//******************************************************************************************//
//*************************************** default_func *************************************//
//******************************************************************************************//
// for when no menu is shown
UCHAR default_func(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col)
{
//	UCHAR row1,col1,k;
	UCHAR ret_char;
//	printHexByte(ch);	
	switch (ch)
	{
		case KP_1:
			GDispStringAt(13,0,"           ");
			GDispStringAt(14,0,"           ");
			break;
		case KP_2:
			GDispStringAt(13,0,"Oh Shit!   ");
			GDispStringAt(14,0,"           ");
			break;
		case KP_3:
			GDispStringAt(13,0,"Hot Dam!   ");
			GDispStringAt(14,0,"It Works!  ");
			break;
		case KP_4:
			GDispStringAt(0,8,"      ");
			break;
		case KP_AST:	// '*'
			GDispStringAt(13,0,"asterik    ");
			break;
		case KP_0:	// '0'
			GDispStringAt(13,0,"0 keypress ");
			break;
		case KP_POUND:	// '#'
			GDispStringAt(13,0,"pound sign ");
			break;
		case KP_D:	// 'D'
			GDispStringAt(13,0,"D keypress ");
			break;
		default:
			ret_char = ch;
			break;
	}
	ret_char = ch;
	return ret_char;
			
}

//******************************************************************************************//
//************************************* main_menu_func *************************************//
//******************************************************************************************//
// displays the main menu at the bottom
UCHAR main_menu_func(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char;
	switch (ch)
	{
		case KP_POUND:
			ret_char = '#';
			break;
		case KP_AST:
			ret_char = '*';
			break;
		case KP_0:
			ret_char = '0';
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
UCHAR menu1a(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char;
	switch (ch)
	{
		case KP_POUND:
			ret_char = '#';
			break;
		case KP_AST:
			ret_char = '*';
			break;
		case KP_0:
			ret_char = '0';
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
UCHAR menu1b(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char;
	switch (ch)
	{
		case KP_POUND:
			ret_char = '#';
			break;
		case KP_AST:
			ret_char = '*';
			break;
		case KP_0:
			ret_char = '0';
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
UCHAR menu1c(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char;
	switch (ch)
	{
		case KP_POUND:
			ret_char = '#';
			break;
		case KP_AST:
			ret_char = '*';
			break;
		case KP_0:
			ret_char = '0';
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
UCHAR menu1d(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char;
	switch (ch)
	{
		case KP_POUND:
			ret_char = '#';
			break;
		case KP_AST:
			ret_char = '*';
			break;
		case KP_0:
			ret_char = '0';
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
UCHAR menu2a(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char;
	switch (ch)
	{
		case KP_POUND:
			ret_char = '#';
			break;
		case KP_AST:
			ret_char = '*';
			break;
		case KP_0:
			ret_char = '0';
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
UCHAR menu2b(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char;
	switch (ch)
	{
		case KP_POUND:
			ret_char = '#';
			break;
		case KP_AST:
			ret_char = '*';
			break;
		case KP_0:
			ret_char = '0';
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
UCHAR menu2c(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char;
	switch (ch)
	{
		case KP_POUND:
			ret_char = '#';
			break;
		case KP_AST:
			ret_char = '*';
			break;
		case KP_0:
			ret_char = '0';
			break;
		default:
			ret_char = ch;
			break;
	}
	return ret_char;
}
//******************************************************************************************//
//****************************************** menu2d ****************************************//
//******************************************************************************************//
// displays the 4th choice of the 1st choice of the main menu
UCHAR menu2d(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char;
	switch (ch)
	{
		case KP_POUND:
			ret_char = '#';
			break;
		case KP_AST:
			ret_char = '*';
			break;
		case KP_0:
			ret_char = '0';
			break;
		default:
			ret_char = ch;
			break;
	}
	return ret_char;
}
