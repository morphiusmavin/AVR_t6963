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
	uint16_t layout_offset;

//    size_t str_size = sizeof(PROMPT_STRUCT);
	
	UCHAR (*fptr[NUM_FPTS])(UCHAR, uint8_t, uint16_t, UCHAR, UCHAR) = { default_func,\
		 main_menu_func,\
		 menu1a, menu1b,\
		 menu1c, menu1d,\
		 menu2a, menu2b,\
		 menu2c, menu2d };
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
	printString("LCD is on!.\r\n");

	printString("tesing LCD\r\n");

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
		layout_offset = (uint16_t)eeprom_read_byte((uint8_t*)LAYOUT_OFFSET_EEPROM_LOCATION_LSB);
		temp = eeprom_read_byte((uint8_t*)LAYOUT_OFFSET_EEPROM_LOCATION_MSB);
		temp2 = (uint16_t)temp;
		layout_offset |= (temp2 <<= 8);
		prompt_ptr = malloc((size_t)(layout_offset - prompt_info_offset));
		if(prompt_ptr == NULL)
		{
			printString("malloc returned NULL for prompt_ptr\r\n");
			return 1;
		}
		printString("\r\nlayout_offset: ");
		printHexByte((uint8_t)layout_offset);
		printHexByte((uint8_t)layout_offset<<8);
		labels = (char*)malloc((size_t)prompt_info_offset);
		if(labels == NULL)
		{
			printString("malloc for labels returned NULL\r\n");
		}	
		eeprom_read_block(labels, eepromString,prompt_info_offset);
		printString("\r\n");

		eeprom_read_block(prompt_ptr, eepromString+prompt_info_offset, sizeof(PROMPT_STRUCT)*no_prompts);

		for(i = 0;i < no_prompts;i++)
		{
			printHexByte((uint8_t)prompt_ptr[i].pnum);
			transmitByte(0x20);
			printHexByte((uint8_t)prompt_ptr[i].row);
			transmitByte(0x20);
			printHexByte((uint8_t)prompt_ptr[i].col);
			transmitByte(0x20);
			printHexByte((uint8_t)(prompt_ptr[i].offset>>8));
			printHexByte((uint8_t)(prompt_ptr[i].offset));
			transmitByte(0x20);
			printHexByte((uint8_t)prompt_ptr[i].len);
			transmitByte(0x20);
			printHexByte((uint8_t)prompt_ptr[i].type);
			printString("\r\n");
		}

		// now read into the heap all the real-time display format data
		rt_main = (RT_MAIN*)malloc(sizeof(RT_MAIN)*no_layouts);
		if(rt_main == NULL)
		{
			printString("malloc returned NULL for rt_main\r\n");
			return 1;
		}
		for(i = 0;i < no_layouts;i++)
		{
			eeprom_read_block((void*)(&(rt_main[i])),eepromString+((i*sizeof(RT_MAIN))+layout_offset),sizeof(RT_MAIN));
			strcpy(ramString,rt_main[i].name);
			printString(ramString);
			transmitByte(0x20);
			printString("num_params: ");
			printHexByte(rt_main[i].num_params);
			printString("\r\n");
		}

		for(i = 0;i < no_layouts;i++)
		{
			rt_main[i].ptr_rt_layout = (RT_LAYOUT*)malloc((sizeof(RT_LAYOUT)*(rt_main[i].num_params)));
			if(rt_main[i].ptr_rt_layout == NULL)
			{
				printString("malloc returned NULL for rt_main[i].ptr_rt_layout\r\n");
				return 1;
			}
		}

		// now all the new memory is malloc'd for the rt_layout and rt_main structs 

		for(i = 0;i < no_layouts;i++)
		{
			printHexByte(rt_main[i].offset);
			printHexByte(rt_main[i].offset>>8);
			printString("\r\n");

			for(j = 0;j < rt_main[i].num_params;j++)
			{
				eeprom_read_block((void*)(&(rt_main[i].ptr_rt_layout[j])),\
						eepromString+(rt_main[i].offset)+(j*sizeof(RT_LAYOUT)),\
								sizeof(RT_LAYOUT));

				printHexByte(rt_main[i].ptr_rt_layout[j].row);
				transmitByte(0x20);
				printHexByte(rt_main[i].ptr_rt_layout[j].col);
				transmitByte(0x20);
				printHexByte(rt_main[i].ptr_rt_layout[j].label_list);
				printString("\r\n");

			}	

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
	display_labels();
	set_defaults();
    while (1) 
    {
        test1 = receiveByte();
		ret_char = (*fptr[current_fptr])(test1, limit8, limit16, cur_row, cur_col);
		if(current_fptr != last_fptr)
			display_labels();
		last_fptr = current_fptr;
		parse_PIC24(ret_char);
	}
    return (0);		// this should never happen
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

	dispCharAt(15,19,test);
	if(++test > 0x7e)
		test = 0x21;

	dispCharAt(15,0,parse_state+0x30);
	
	switch(parse_state)
	{
		case IDLE:
			if(ch <= RT_RPM && ch >= RT_TRIP)
			{
				current_param = ch;
				parse_state = CHECK_HIGHBIT;
				dispCharAt(15,2,parse_state+0x30);
			}
			break;
		case CHECK_HIGHBIT:
			switch(ch)
			{
				case RT_HIGH0:
					parse_state = SEND_UCHAR;
					dispCharAt(15,4,parse_state+0x30);
					break;
				case RT_HIGH1:
					temp_UINT = ch;
					parse_state = SEND_UINT1;
					dispCharAt(15,6,parse_state+0x30);
					break;
				case RT_HIGH2:
					temp_UINT = ch;
					dispCharAt(15,8,parse_state+0x30);
					parse_state = SEND_UINT2;
					break;
				default:
					xbyte = ch;
					dispCharAt(15,10,parse_state+0x30);
					sprintf(param_string,"%3d",xbyte);
					done = 1;
					break;	
			}
			break;
		case SEND_UCHAR:
			xbyte = ch | 0x80;
			dispCharAt(15,12,parse_state+0x30);
			sprintf(param_string,"%3d",xbyte);
			done = 1;
			break;
		case SEND_UINT1:
			xword = temp_UINT;
			temp_UINT = (uint16_t)ch;
			temp_UINT <<= 8;
			xword |= temp_UINT;
			xword |= 0x0080;
			dispCharAt(15,14,parse_state+0x30);
			sprintf(param_string,"%4u",xword);
			done = 1;
			break;
		case SEND_UINT2:
			xword = temp_UINT;
			temp_UINT = (uint16_t)ch;
			temp_UINT <<= 8;
			xword |= temp_UINT;
			xword |= 0x8000;
			dispCharAt(15,16,parse_state+0x30);
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
			if(prompt_ptr[i].type == RT_LABEL)
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

