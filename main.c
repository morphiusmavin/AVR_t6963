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

int main(void) 
{
    uint8_t test1;
    char ramString[STRING_LEN];
	int i,j;
//    int row,col;
//    UCHAR k;
	uint8_t temp;
	uint16_t temp2;
	int last_fptr = 0;
	uint16_t limit16 = 0;
	uint8_t limit8 = 0;
	UCHAR ret_char;
	uint16_t prompt_info_offset = 0;

//    size_t str_size = sizeof(PROMPT_STRUCT);
	
	UCHAR (*fptr[NUM_FPTS])(UCHAR, uint8_t, uint16_t, UCHAR, UCHAR) = { main_menu_func,\
		 menu1a, menu1b,\
		 menu1c, menu1d,\
		 menu2a, menu2b,\
		 menu2c, menu2d, number_entry };
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
		printString("reading prompt data into prompt structs\r\n");
		no_prompts = eeprom_read_byte((uint8_t*)NO_PROMPTS_EEPROM_LOCATION);
		printString("no_prompts: ");
		printHexByte(no_prompts);
		printString("\r\n");
		no_layouts = eeprom_read_byte((uint8_t*)NO_LAYOUTS_EEPROM_LOCATION);
		printString("no_layouts: ");
		printHexByte(no_layouts);
		printString("\r\n");
		prompt_info_offset = (uint16_t)eeprom_read_byte((uint8_t*)PROMPT_INFO_OFFSET_EEPROM_LOCATION_LSB);
		temp = eeprom_read_byte((uint8_t*)PROMPT_INFO_OFFSET_EEPROM_LOCATION_MSB);
		temp2 = (uint16_t)temp;
		prompt_info_offset |= (temp2 << 8);		// shift msb over and bit-or with lsb
		printString("prompt_info_offset: ");
		printHexByte((uint8_t)prompt_info_offset);
		printHexByte((uint8_t)(prompt_info_offset>>8));

		for(i = 0;i < no_prompts;i++)
		{
			eeprom_read_block((void*)&prompts[i], eepromString+(prompt_info_offset+(i*sizeof(PROMPT_STRUCT))),sizeof(PROMPT_STRUCT));
		}
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
	}
	else
	{
		printString("prompts not set - run eeprom_burn.c to burn eeprom\r\n");
	}
#endif
//******************************************************************************************//
//*********************************** start of main loop ***********************************//
//******************************************************************************************//
	curr_rt_layout = 0;
	GDispClrTxt();
//	display_labels();
	set_defaults();
//	display_menus();
    while (1) 
    {
        test1 = receiveByte();
//		printHexByte(test1);
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
	char temp[10];
	for(i = 0;i < no_prompts;i++)
	{
		if(prompts[i].type == current_fptr)
		{
			memcpy((void*)temp,(labels+prompts[i].offset),prompt_ptr[i].len+1);
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
	int i,j;
	uint8_t rowx,colx;
	uint8_t xbyte;
	uint16_t xword;
	uint8_t done = 0;
	char param_string[10];
	UCHAR temp;
/*
	dispCharAt(15,19,test);
	if(++test > 0x7e)
		test = 0x21;

	dispCharAt(15,0,parse_state+0x30);
*/	
	switch(parse_state)
	{
		case IDLE:
			if(ch <= RT_RPM && ch >= RT_TRIP)
			{
				current_param = ch;
				parse_state = CHECK_HIGHBIT;
//				dispCharAt(12,2,parse_state+0x30);
			}
			break;
		case CHECK_HIGHBIT:
			switch(ch)
			{
				case RT_HIGH0:
					parse_state = SEND_UCHAR;
//					dispCharAt(12,4,parse_state+0x30);
					break;
				case RT_HIGH1:
					temp_UINT = ch;
					parse_state = SEND_UINT1;
//					dispCharAt(12,6,parse_state+0x30);
					break;
				case RT_HIGH2:
					temp_UINT = ch;
//					dispCharAt(12,8,parse_state+0x30);
					parse_state = SEND_UINT2;
					break;
				default:
					xbyte = ch;
//					dispCharAt(12,10,parse_state+0x30);
					sprintf(param_string,"%3d",xbyte);
					done = 1;
					break;	
			}
			break;
		case SEND_UCHAR:
			xbyte = ch | 0x80;
//			dispCharAt(12,12,parse_state+0x30);
			sprintf(param_string,"%3d",xbyte);
			done = 1;
			break;
		case SEND_UINT1:
			xword = temp_UINT;
			temp_UINT = (uint16_t)ch;
			temp_UINT <<= 8;
			xword |= temp_UINT;
			xword |= 0x0080;
//			dispCharAt(12,14,parse_state+0x30);
			sprintf(param_string,"%4u",xword);
			done = 1;
			break;
		case SEND_UINT2:
			xword = temp_UINT;
			temp_UINT = (uint16_t)ch;
			temp_UINT <<= 8;
			xword |= temp_UINT;
			xword |= 0x8000;
//			dispCharAt(12,16,parse_state+0x30);
			sprintf(param_string,"%4u",xword);
			done = 1;
			break;
		default:
			break;
	}
	if(done)
	{
		temp = ~current_param;
		for(i = 0;i < no_prompts;i++)
		{
			if(prompts[i].type == RT_LABEL)
			{
				for(j = 0;j < RTMAINC.num_params;j++)
				{
					if(temp == RTMAINC.ptr_rt_layout[j].label_list)
					{
						rowx = RTMAINC.ptr_rt_layout[j].row;
						colx = RTMAINC.ptr_rt_layout[j].col;
//						GDispStringAt(RTMAINC.ptr_rt_layout[j].row,RTMAINC.ptr_rt_layout[j].col+prompt_ptr[i].len+5,param_string);
						GDispStringAt(rowx,colx+prompt_ptr[i].len+6,param_string);
						i = no_prompts;
					}
				}
			}
		}
		set_defaults();
	}
}
//******************************************************************************************//
//**************************************** display_labels **********************************//
//******************************************************************************************//
// displays only the labels of the current rt_layout
void display_labels(void)
{
	int i,j;
	char temp[20];
	j = 0;
//	GDispStringAt(15,0,RTMAINC.name);
//	printString("displaying labels\r\n");

	for(i = 0;i < no_prompts;i++)
	{
		for(j = 0;j < RTMAINC.num_params;j++)
		{
			if(prompt_ptr[i].type == RT_LABEL && i == (RTMAINC.ptr_rt_layout[j].label_list))
			{
				memcpy((void*)temp,(labels+prompt_ptr[i].offset),prompt_ptr[i].len+1);
				
//				GDispStringAt(prompt_ptr[i].row,prompt_ptr[i].col,temp);
//				printString(temp);
//				printString("\r\n");
				GDispStringAt(RTMAINC.ptr_rt_layout[j].row,RTMAINC.ptr_rt_layout[j].col,temp);
			}
		}
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
	sequence_counter = 0;
	current_param = 0;
	temp_UINT = 0;
	last_char = 0;
	parse_state = IDLE;
	test = 0x21;
}

