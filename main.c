// main.c - main program that drives the t6963 LCD (32 col x 15 row) display - text only
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

PROMPT_STRUCT prompts[26];	// this must be large enough for no_prompts

int main(void) 
{
    uint8_t test1;
	int i;
	uint8_t temp;
	uint16_t temp2;
	uint16_t limit16 = 0;
	uint8_t limit8 = 0;
	UCHAR ret_char;
	uint16_t prompt_info_offset = 0;

//    size_t str_size = sizeof(PROMPT_STRUCT);
	
	UCHAR (*fptr[NUM_FPTS])(UCHAR, uint8_t, uint16_t, UCHAR, UCHAR) = { main_menu_func,\
		 menu1a, menu1b,\
		 menu1c, menu1d,\
		 menu2a, menu2b, menu2c,\
		 menu3a, menu3b, menu3c,\
		 menu4a, menu4b, menu4c, number_entry };
#if 1
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
    no_prompts = eeprom_read_byte((uint8_t*)NO_PROMPTS_EEPROM_LOCATION);

	if(no_prompts != 0xff)
	{
//		printString("reading prompt data into prompt structs\r\n");
		no_prompts = eeprom_read_byte((uint8_t*)NO_PROMPTS_EEPROM_LOCATION);
//		printString("no_prompts: ");
		printHexByte(no_prompts);
//		printString("\r\n");
		prompt_info_offset = (uint16_t)eeprom_read_byte((uint8_t*)PROMPT_INFO_OFFSET_EEPROM_LOCATION_LSB);
		temp = eeprom_read_byte((uint8_t*)PROMPT_INFO_OFFSET_EEPROM_LOCATION_MSB);
		temp2 = (uint16_t)temp;
		prompt_info_offset |= (temp2 << 8);		// shift msb over and bit-or with lsb
//		printString("prompt_info_offset: ");
//		printHexByte((uint8_t)prompt_info_offset);
//		printHexByte((uint8_t)(prompt_info_offset>>8));
//		printString("\r\n");
		for(i = 0;i < no_prompts;i++)
		{
			eeprom_read_block((void*)&prompts[i], eepromString+(prompt_info_offset+(i*sizeof(PROMPT_STRUCT))),sizeof(PROMPT_STRUCT));
		}
/*		
		for(i = 0;i < no_prompts;i++)
		{
			printHexByte((uint8_t)prompts[i].pnum);
			transmitByte(0x20);
			printHexByte((uint8_t)prompts[i].row);
			transmitByte(0x20);
			printHexByte((uint8_t)prompts[i].col);
			transmitByte(0x20);
			printHexByte((uint8_t)(prompts[i].offset>>8));
			printHexByte((uint8_t)(prompts[i].offset));
			transmitByte(0x20);
			printHexByte((uint8_t)prompts[i].len);
			transmitByte(0x20);
			printHexByte((uint8_t)prompts[i].type);
			printString("\r\n");
		}
		printString("\r\ndone reading eeprom\r\n");
*/		
	}
	else
	{
		printString("prompts not set - run eeprom_burn.c to burn eeprom\r\n");
	}
#endif
//******************************************************************************************//
//*********************************** start of main loop ***********************************//
//******************************************************************************************//
	GDispClrTxt();
/*
	_delay_ms(5000);
	printString("\r\ndisplaying labels...");
	display_labels();
*/	
	set_defaults();
	_delay_ms(500);
//	printString("\r\ndisplaying menus...\r\n");
	display_menus();
	display_labels();
    while (1) 
    {
        test1 = receiveByte();
//		printHexByte(current_fptr);
//		printString("\r\n");
		ret_char = (*fptr[current_fptr])(test1, limit8, limit16, cur_row, cur_col);
		if(current_fptr != last_fptr)
			display_menus();
		last_fptr = current_fptr;
		parse_PIC24(ret_char);
	}
    return (0);		// this should never happen
}
//******************************************************************************************//
//************************************* display_menus **************************************//
//******************************************************************************************//
// display a different menu
void display_menus(void)
{
	int i;
	char temp[20];
	for(i = 0;i < no_prompts;i++)
	{
		if(prompts[i].type == current_fptr)
		{
			eeprom_read_block(temp, eepromString+prompts[i].offset,prompts[i].len+1);
			GDispStringAt(prompts[i].row,prompts[i].col,temp);
		}
	}	
}
//******************************************************************************************//
//**************************************** display_labels **********************************//
//******************************************************************************************//
// displays only the labels of the current rt_layout
void display_labels(void)
{
	int i;
	char temp[20];

	for(i = 0;i < no_prompts;i++)
	{
		if(prompts[i].type == RT_LABEL)
		{
			eeprom_read_block(temp, eepromString+prompts[i].offset,prompts[i].len+1);
			GDispStringAt(prompts[i].row,prompts[i].col,temp);	
		}
	}
}
//******************************************************************************************//
//*************************************** parse_PIC24 **************************************//
//******************************************************************************************//
// get data from PIC24, convert from int to strings and place in param_string
void parse_PIC24(UCHAR ch)
{
	int i;
	uint8_t xbyte;
	uint16_t xword;
	uint8_t done = 0;
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
			printString("\r");
			printString(param_string);
		}
		for(i = 0;i < no_prompts;i++)
		{
			if(prompts[i].type == RT_LABEL && temp == prompts[i].pnum)
			{
				GDispStringAt(prompts[i].row,prompts[i].col+10,param_string);
			}
		}
		set_defaults();
	}
}
//******************************************************************************************//
//******************************************* dispRC ***************************************//
//******************************************************************************************//
// show on monitor where the current row, col is
void dispRC(int row, int col)
{
    printString("col=");
    printByte(col);
    printString("\r\n");
    printString("row=");
    printByte(row);
    printString("\r\n");
    printString("\r\n");
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
	current_param = 0;
	temp_UINT = 0;
	parse_state = IDLE;
	current_fptr = 0;
	last_fptr = 0;
}

