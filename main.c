#if 1
//******************************************************************************************//
//*************************************** main.c  ******************************************//
//******************************************************************************************//
// main program that drives the t6963 LCD (32 col x 15 row) display - text only
// see t6963_notes.txt for more details
#include <avr/io.h>
#include "../avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include "sfr_helper.h"
#include <avr/eeprom.h>
#include <stdlib.h>
#include "USART.h"
#include "t6963.h"
#include "macros.h"
#include <string.h>
#include "main.h"
#include <stdio.h>
#include <stdlib.h>

char eepromString[STRING_LEN] EEMEM;

//PROMPT_STRUCT prompts[26];	// this must be large enough for no_prompts
#endif
int main(void)
{
	int i;
	uint16_t temp2;
	UCHAR ret_char, temp;
	int done;
	UCHAR xbyte;
	UINT xword;
	int do_states = 1;
//    size_t str_size = sizeof(PROMPT_STRUCT);

    initUSART();

	GDispInit();
	_delay_us(10);
	GDispSetMode(XOR_MODE);
	_delay_us(10);
	GDispSetMode(TEXT_ON);
	_delay_us(10);
	GDispClrTxt();
	GDispStringAt(1,1,"LCD is on!");

//******************************************************************************************//
//******************* read all the data from eeprom into memory  ***************************//
//******************************************************************************************//
	no_rt_labels  = eeprom_read_byte((UCHAR*)NO_RT_LABELS_EEPROM_LOCATION);

	if(no_rt_labels != 0xff)
	{
#ifdef TTY_DISPLAY
#warning "TTY_DISPLAY defined"
		printString("reading prompt data into prompt structs\r\n");
#endif
		no_rt_labels  = eeprom_read_byte((UCHAR*)NO_RT_LABELS_EEPROM_LOCATION);
		no_menu_labels  = eeprom_read_byte((UCHAR*)NO_MENU_LABELS_EEPROM_LOCATION);
		no_rtparams  = eeprom_read_byte((UCHAR*)NO_RTPARAMS_EEPROM_LOCATION);
		no_menu_structs  = eeprom_read_byte((UCHAR*)NO_MENUS_EEPROM_LOCATION);
#ifdef TTY_DISPLAY
		printString("no_labels: ");
		printHexByte(no_labels);
		printString("\r\n");
		printString("no_rtparams: ");
		printHexByte(no_rtparams);
		printString("\r\n");
		printString("no_menu_structs: ");
		printHexByte(no_menu_structs);
		printString("\r\n");
#endif
		rt_params_offset = (UINT)eeprom_read_byte((UCHAR*)RTPARAMS_OFFSET_EEPROM_LOCATION_LSB);
		temp = eeprom_read_byte((UCHAR*)RTPARAMS_OFFSET_EEPROM_LOCATION_MSB);
		temp2 = (UINT)temp;
		rt_params_offset |= (temp2 << 8);		// shift msb over and bit-or with lsb

		menu_struct_offset = (UCHAR)eeprom_read_byte((UCHAR*)MENUSTRUCT_OFFSET_EEPROM_LOCATION_LSB);
		temp = eeprom_read_byte((UCHAR*)MENUSTRUCT_OFFSET_EEPROM_LOCATION_MSB);
		temp2 = (UINT)temp;
		menu_struct_offset |= (temp2 << 8);		// shift msb over and bit-or with lsb

#ifdef TTY_DISPLAY
/*
		printString("label_info_offset: ");
		printHexByte((UCHAR)label_info_offset);
		printHexByte((UCHAR)(label_info_offset>>8));
		printString("\r\n");
*/
		printString("rt_params_offset: ");
		printHexByte((UCHAR)rt_params_offset);
		printHexByte((UCHAR)(rt_params_offset>>8));
		printString("\r\n");
		printString("menu_struct_offset: ");
		printHexByte((UCHAR)menu_struct_offset);
		printHexByte((UCHAR)(menu_struct_offset>>8));
		printString("\r\n");
#endif
#if 0
		for(i = 0;i < no_prompts;i++)
		{
			eeprom_read_block((void*)&prompts[i], eepromString+(prompt_info_offset+(i*sizeof(PROMPT_STRUCT))),sizeof(PROMPT_STRUCT));
		}

#ifdef TTY_DISPLAY
		for(i = 0;i < no_prompts;i++)
		{
			printHexByte((UCHAR)prompts[i].pnum);
			transmitByte(0x20);
			printHexByte((UCHAR)prompts[i].row);
			transmitByte(0x20);
			printHexByte((UCHAR)prompts[i].col);
			transmitByte(0x20);
//			printHexByte((UCHAR)(prompts[i].offset>>8));
//			printHexByte((UCHAR)(prompts[i].offset));
			transmitByte(0x20);
//			printHexByte((UCHAR)prompts[i].len);
			transmitByte(0x20);
			printHexByte((UCHAR)prompts[i].type);
			printString("\r\n");
		}
		printString("\r\ndone reading eeprom\r\n");
#endif
#endif
	}
	else
	{
		printString("prompts not set - run eeprom_burn.c to burn eeprom\r\n");
	}

//******************************************************************************************//
//*********************************** start of main loop ***********************************//
//******************************************************************************************//
	GDispClrTxt();
	set_defaults();
	_delay_us(10);
	init_list();
	display_labels();
	done = 0;
	char param_string[10];

    while (1)
    {
        ret_char = receiveByte();
		ret_char = get_key(ret_char);
		done = parse_P24(ret_char, param_string, &xbyte, &xword);

// try this, just to be a little more cryptic:
//		done = parse_P24(get_key(receiveByte()), param_string, &xbyte, &xword);

		if(done)
		{
			if(current_param == RT_RPM)
			{
#ifdef TTY_DISPLAY
				printHexByte((UCHAR)(xword>>8));
				printHexByte((UCHAR)xword);
	//			GDispStringAt(2,2,param_string);
#else
				transmitByte((UCHAR)(xword>>8));
				transmitByte((UCHAR)xword);
	//			GDispStringAt(2,2,param_string);
#endif
			}
			else
			{
#ifdef TTY_DISPLAY
				printHexByte(xbyte);
	//			GDispStringAt(2,4,param_string);
#else
				transmitByte(xbyte);
	//			GDispStringAt(2,4,param_string);
#endif
			}
			for(i = 0;i < no_rtparams;i++)
			{
				if(rt_params[i].type == current_param)
				{
					GDispStringAt(rt_params[i].row,rt_params[i].col+9,"      ");
				}

				if(rt_params[i].type == current_param && rt_params[i].shown == SHOWN_SENT)
				{
					GDispStringAt(rt_params[i].row,rt_params[i].col+10,param_string);
				}
			}
			set_defaults();
		}

	}
    return (0);		// this should never happen
}
//******************************************************************************************//
//******************************************* dispRC ***************************************//
//******************************************************************************************//
// show on monitor where the current row, col is
void dispRC(int row, int col)
{
#if TTY_DISPLAY
    printString("col=");
    printByte(col);
    printString("\r\n");
    printString("row=");
    printByte(row);
    printString("\r\n");
    printString("\r\n");
#endif
}
//******************************************************************************************//
//****************************************** CheckRC ***************************************//
//******************************************************************************************//
// check row col to see if it is 'off the screen' so it can re-appear on the opposite border
void CheckRC(int *row, int *col, UCHAR *k)
{
    if(*row >= ROWS)
        *row = 0;

    if(*col >= COLUMN)
        *col = 0;

    if(*row < 0)
        *row = ROWS-1;

    if(*col < 0)
        *col = COLUMN-1;

    if(*k > 0x50)
        *k = 0x41;
}

void set_defaults(void)
{
	temp_UINT = 0;
	parse_state = IDLE;
 }

