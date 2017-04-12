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
	UCHAR temp;
	uint16_t temp2;
	UCHAR ret_char;

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
	no_labels  = eeprom_read_byte((UCHAR*)NO_LABELS_EEPROM_LOCATION);

	if(no_labels != 0xff)
	{
#ifdef TTY_DISPLAY
#warning "TTY_DISPLAY defined"
		printString("reading prompt data into prompt structs\r\n");
#endif
		no_labels  = eeprom_read_byte((UCHAR*)NO_LABELS_EEPROM_LOCATION);
		no_rtparams  = eeprom_read_byte((UCHAR*)NO_LABELS_EEPROM_LOCATION);
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
/*
		label_info_offset = (UINT)eeprom_read_byte((UCHAR*)LABEL_INFO_OFFSET_EEPROM_LOCATION_LSB);
		temp = eeprom_read_byte((UCHAR*)LABEL_INFO_OFFSET_EEPROM_LOCATION_MSB);
		temp2 = (UINT)temp;
		label_info_offset |= (temp2 << 8);		// shift msb over and bit-or with lsb
*/
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
    while (1)
    {
        ret_char = receiveByte();
#ifdef TTY_DISPLAY
		printHexByte(current_fptr);
		printString("\r\n");
#endif
//		ret_char = (*fptr[current_fptr])(ret_char, limit8, limit16, cur_row, cur_col);
		ret_char = get_key(ret_char);
//		if(current_fptr != last_fptr)
//		if(curr_fptr_changed())
//			display_menus();
//		last_fptr = current_fptr;
		parse_PIC24(ret_char);
	}
    return (0);		// this should never happen
}

//******************************************************************************************//
//********************************** get_menu_struct_type **********************************//
//******************************************************************************************//
#ifdef MAIN_C
int get_menu_struct_type(int index)
{
#if 0
	MENU_STRUCT ms;
	eeprom_read_block(&ms, eepromString+menu_struct_offset+(sizeof(MENU_STRUCT)*index),sizeof(MENU_STRUCT));
	return ms.type;
#endif
	return 0;
}
//******************************************************************************************//
//********************************* get_menu_struct_choice *********************************//
//******************************************************************************************//
int get_menu_struct_choice(int index)
{
#if 0
	MENU_STRUCT ms;
	eeprom_read_block(&ms, eepromString+menu_struct_offset+(sizeof(MENU_STRUCT)*index),sizeof(MENU_STRUCT));
	return ms.menu_choice;
#endif
	return 0;
}
//******************************************************************************************//
//********************************* get_menu_struct_choice *********************************//
//******************************************************************************************//
int get_menu_struct_chtype(int index)
{
#if 0
	MENU_STRUCT ms;
	eeprom_read_block(&ms, eepromString+menu_struct_offset+(sizeof(MENU_STRUCT)*index),sizeof(MENU_STRUCT));
	return ms.ch_type;
#endif
	return 0;
}
//******************************************************************************************//
//*************************************** get_label ****************************************//
//******************************************************************************************//
char *get_label(int index)
{
	int offset = 0;
	int j;
	char label[MAX_LABEL_LEN];

	for(j = 0;j < index;j++)
		offset += label_offsets[j];
	eeprom_read_block(label, eepromString+offset,MAX_LABEL_LEN);
	return label;
}
//******************************************************************************************//
//**************************************** get_row *****************************************//
//******************************************************************************************//
UCHAR get_row(int index)
{
/*
	MENU_STRUCT ms;
	int offset = (int)menu_struct_offset + index * sizeof(MENU_STRUCT);
	eeprom_read_block(&ms,eepromString+offset,sizeof(MENU_STRUCT));
	return ms.row;
*/	return 0;
}
//******************************************************************************************//
//**************************************** get_col *****************************************//
//******************************************************************************************//
UCHAR get_col(int index)
{
/*
	MENU_STRUCT ms;
	int offset = (int)menu_struct_offset + index * sizeof(MENU_STRUCT);
	eeprom_read_block(&ms,eepromString+offset,sizeof(MENU_STRUCT));
	return ms.col;
*/	return 0;
}
#endif
//******************************************************************************************//
//*************************************** parse_PIC24 **************************************//
//******************************************************************************************//
// get data from PIC24, convert from int to strings and place in param_string
void parse_PIC24(UCHAR ch)
{
	int i;
	UCHAR xbyte;
	uint16_t xword = 0;
	UCHAR done = 0;
	char param_string[10];
	UCHAR temp;

	switch(parse_state)
	{
		case IDLE:
			if(ch <= RT_RPM && ch >= RT_TRIP)
			{
				current_param = ch;
				parse_state = CHECK_HIGHBIT;
			}
			else
			{
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
//			printString("\r\n");
//			printString(param_string);
			done = 1;
			break;
		case SEND_UCHAR1:
			xbyte = ch | 0x80;
			sprintf(param_string,"%4d",xbyte);
//			printString("\r\n");
//			printString(param_string);
			done = 1;
			break;
		case SEND_UINT0:
			xword = (uint16_t)temp_UINT;
			temp_UINT = (uint16_t)ch;
			temp_UINT <<= 8;
			temp_UINT &= 0xff00;
			xword |= temp_UINT;
			sprintf(param_string,"%4u",xword);
//			printString("\r\n");
//			printString(param_string);

			done = 1;
			break;
		case SEND_UINT1:
			xword = (uint16_t)temp_UINT;
			temp_UINT = (uint16_t)ch;
			temp_UINT <<= 8;
			temp_UINT &= 0xff00;
			xword |= temp_UINT;
			xword |= 0x0080;
			sprintf(param_string,"%4u",xword);
//			printString("\r\n");
//			printString(param_string);
			done = 1;
			break;
		case SEND_UINT2:
			xword = (uint16_t)temp_UINT;
			temp_UINT = (uint16_t)ch;
			temp_UINT <<= 8;
			temp_UINT &= 0xff00;
			xword |= temp_UINT;
			xword |= 0x8000;
			sprintf(param_string,"%4u",xword);
//			printString("\r\n");
//			printString(param_string);
			done = 1;
			break;
		default:
			printString("default ");
			set_defaults();
			break;
	}
	if(done)
	{
		temp = ~current_param;
		if(temp == 0)
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
/*
		for(i = 0;i < no_prompts;i++)
		{
			if(prompts[i].type == RT_LABEL && temp == prompts[i].pnum)
			{
				GDispStringAt(prompts[i].row,prompts[i].col+10,param_string);
			}
		}
*/
		set_defaults();
	}
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

