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

/* #include <stdio.h> */
/* #include <stdlib.h> */

int main(void) 
{
    uint8_t test1;
    char ramString[STRING_LEN];
	int i,j;
    int row,col;
    UCHAR k;
	uint8_t temp;
	uint16_t temp2;
	int current_fptr = 0;
	uint16_t limit16;
	uint8_t limit8;
	UCHAR ret_char;
	uint16_t prompt_info_offset = 0;
	uint16_t layout_offset = 0;

    size_t str_size = sizeof(PROMPT_STRUCT);
	
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
	k = 0x20;
	for(row = 0;row < ROWS;row++)
	{
		for(col = 0;col < COLUMN;col++)
		{
			dispCharAt(row,col,k);
			if(++k > 0x7e)
				k = 0x20;
		}
	}
    no_prompts = eeprom_read_byte((uint8_t*)0x03f0);

	if(no_prompts != 0xff)
	{
		printString("reading prompt data into prompt structs\r\n");
		no_prompts = eeprom_read_byte((uint8_t*)NO_PROMPTS_EEPROM_LOCATION);
		no_layouts = eeprom_read_byte((uint8_t*)NO_LAYOUTS_EEPROM_LOCATION);
		prompt_info_offset = (uint16_t)eeprom_read_byte((uint8_t*)PROMPT_INFO_OFFSET_EEPROM_LOCATION_LSB);	// read lsb
		temp = eeprom_read_byte((uint8_t*)PROMPT_INFO_OFFSET_EEPROM_LOCATION_MSB);		// read msb
		temp2 = (uint16_t)temp;
		prompt_info_offset |= (temp2 << 8);		// shift msb over and bit-or with lsb
		prompt_ptr = malloc((size_t)prompt_info_offset);
		printString("prompt_info_offset: ");
		printHexByte((uint8_t)prompt_info_offset);
		printHexByte((uint8_t)(prompt_info_offset>>8));
		printString("\r\n");
/*		
		for(i = 0;i < no_prompts;i++)
		{
			eeprom_read_block((void*)(&(prompts[i])),(eepromString+(i*str_size)+prompt_info_offset), str_size);
		}
*/		
		eeprom_read_block(prompt_ptr, eepromString, prompt_info_offset);
#if 0		
		for(i = 0;i < no_prompts;i++)
		{
			memcpy((void*)ramString,(prompt_ptr+prompts[i].offset),prompts[i].len+1);
//			printHexByte(ramString+prompts[i].len);
//			printHexByte(ramString+prompts[i].len+1);
//			printString(ramString);
//			printString("\r\n");
		}
#endif		
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
    while (1) 
    {
        test1 = receiveByte();
		if(test1 
		ret_char = (*fptr[current_fptr])(test1, limit8, limit16, cur_row, cur_col);
		else if(ret_char <= RT_RPM && ret_char >= RT_TRIP)
		{
			parse_PIC24(ret_char);
			display_screen();
		}

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
	uint8_t xbyte;
	uint16_t xword;
	char param_string[10];
	
	if(ch <= RT_RPM && ch >= RT_TRIP)
	{
		current_param = ch;
		sequence_counter = 1;
		return;
	}
		
	if(ch <= RT_HIGH0 && ch >= RT_HIGH2)
	{	
		if(sequence_counter != 1)
		{	
			set_defaults();
			return;
		}
		current_highbit = ch;
		return;
	}

	if((ch | 0x80) == 0)
	{
		if(current_param == RT_RPM)
		{
			if(sequence_counter == 1)
			{
				if(current_highbit == RT_HIGH1)
				{
					ch |= 0x80;
				}
				last_char = ch;
			}	
			else if(sequence_counter == 2)
			{
				if(current_highbit == RT_HIGH2)
				{
					ch |= 0x80;
				}
				xword = (uint16_t)ch;
				xword = (uint16_t)((xword << 8) | ch);
				sprintf(param_string,"%d",xword);
			}	
		}
		else
		{
			if(current_highbit == RT_HIGH0)
			{
				ch |= 0x80;
			}	
			xbyte = ch;
			sprintf(param_string,"%d",xbyte);
		}
		j = 0;
		for(i = 0;i < no_prompts;i++)
		{
			if(prompt_ptr[i].type == RT_LABEL && i == RTMAINC.ptr_rt_layout[j].label_list && j < RTMAINC.num_params)
			{
//				memcpy((void*)param_string,(prompt_ptr+prompts[i].offset),prompts[i].len+1);
				GDispStringAt(prompt_ptr[i].row,prompt_ptr[i].col+prompt_ptr[i].len+1,param_string);
				j++;
			}
		}
		
		sequence_counter++;
	}
}
//******************************************************************************************//
//**************************************** display_screen **********************************//
//******************************************************************************************//
// displays either the RT screen or any of the menus
void display_screen(void)
{
	int i,j;
	char temp[10];
/*	
	printf("%s %d\n",layout->name,layout->num_params);
	for(j= 0;j < 10;j++)
	{
		printf("row:%d col:%d label_list:%d offset:%d\n",\
			layout->rt_layout[j].row,\
			layout->rt_layout[j].col,\
			layout->rt_layout[j].label_list,\
			layout->rt_layout[j].offset);
	}
*/	
	for(j= 0;j < RTMAINC.num_params;j++)
	{
		for(i = 0;i < no_prompts;i++)
		{
			if(prompt_ptr[i].pnum == (RTMAINC.ptr_rt_layout[j].label_list))
			{
				memcpy((void*)temp,(prompt_ptr+prompt_ptr[i].offset),prompt_ptr[i].len+1);
				dispCharAt(RTMAINC.ptr_rt_layout[j].row, RTMAINC.ptr_rt_layout[j].col,temp);
			}	
		}
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
	GDispStringAt(0,0,RTMAINC.name);
	for(i = 0;i < no_prompts;i++)
	{
		if(prompt_ptr[i].type == RT_LABEL && i == RTMAINC.ptr_rt_layout[j].label_list && j < RTMAINC.num_params)
		{
			memcpy((void*)temp,(prompt_ptr+prompt_ptr[i].offset),prompt_ptr[i].len+1);
			GDispStringAt(prompt_ptr[i].row,prompt_ptr[i].col,temp);
			j++;
		}
	}
}
//******************************************************************************************//
//*************************************** default_func *************************************//
//******************************************************************************************//
// for when no menu is shown
UCHAR default_func(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col)
{
	UCHAR row1,col1,k;
	UCHAR ret_char;

	switch (ch)
	{
		case 0xE1:	// '*'
//			ret_char = '*';
			break;
		case 0xE2:	// '0'
			GDispClrTxt();
			k = 0x61;
			for(row1 = 0;row1 < ROWS;row1++)
			{
				for(col1 = 0;col1 < COLUMN;col1++)
				{
					dispCharAt(row1,col1,k);
					_delay_ms(10);
					if(++k > 0x7a)
						k = 0x61;
				}
			}

			break;
		case 0xE0:	// '#'
			GDispClrTxt();
			k = 0x21;
			for(row1 = 0;row1 < ROWS;row1++)
			{
				for(col1 = 0;col1 < COLUMN;col1++)
				{
					dispCharAt(row1,col1,k);
					_delay_ms(10);
					if(++k > 0x7e)
						k = 0x21;
				}
			}
			break;
		case 0xEF:	// 'D'
			GDispClrTxt();
			k = 0x41;
			for(row1 = 0;row1 < ROWS;row1++)
			{
				for(col1 = 0;col1 < COLUMN;col1++)
				{
					dispCharAt(row1,col1,k);
					_delay_ms(10);
					if(++k > 0x5a)
						k = 0x41;
				}
			}
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
		case 0xE0:
			ret_char = '#';
			break;
		case 0xE1:
			ret_char = '*';
			break;
		case 0xE2:
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
		case 0xE0:
			ret_char = '#';
			break;
		case 0xE1:
			ret_char = '*';
			break;
		case 0xE2:
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
		case 0xE0:
			ret_char = '#';
			break;
		case 0xE1:
			ret_char = '*';
			break;
		case 0xE2:
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
		case 0xE0:
			ret_char = '#';
			break;
		case 0xE1:
			ret_char = '*';
			break;
		case 0xE2:
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
		case 0xE0:
			ret_char = '#';
			break;
		case 0xE1:
			ret_char = '*';
			break;
		case 0xE2:
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
		case 0xE0:
			ret_char = '#';
			break;
		case 0xE1:
			ret_char = '*';
			break;
		case 0xE2:
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
		case 0xE0:
			ret_char = '#';
			break;
		case 0xE1:
			ret_char = '*';
			break;
		case 0xE2:
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
		case 0xE0:
			ret_char = '#';
			break;
		case 0xE1:
			ret_char = '*';
			break;
		case 0xE2:
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
		case 0xE0:
			ret_char = '#';
			break;
		case 0xE1:
			ret_char = '*';
			break;
		case 0xE2:
			ret_char = '0';
			break;
		default:
			ret_char = ch;
			break;
	}
	return ret_char;
}
/*
UCHAR menu3a(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col)
{
}
UCHAR menu3b(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col)
{
}
*/

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
	current_highbit = 0;
	last_char = 0;
}

#if 0
void printMenu()
{
	printString("a - display disclaimer\r\n");
	printString("b - display menu 1\r\n");
	printString("d - display menu 2\r\n");
	printString("c - display menu 3\r\n");
	printString("e - test LCD\r\n");
	printString("f - turn off LCD\r\n");
	printString("g - print prompt info\r\n");
}
    printHexByte(pnum);
    transmitByte(0x20);
    printHexByte(row);
    transmitByte(0x20);
    printHexByte(col);
    transmitByte(0x20);
    printHexByte((uint8_t)(offset>>8));
    printHexByte((uint8_t)offset);
    transmitByte(0x20);
    printHexByte((uint8_t)(len>>8));
    printHexByte((uint8_t)len);
    transmitByte(0x20);
    printHexByte(type);
    transmitByte(0x20);
    transmitByte(0x20);



        switch(test1)
        {
            case 'a':
                printString("displaying disclaimer\r\n");
                GDispClrTxt();
                for(i = 0;i < no_prompts;i++)
                {
                    if(prompts[i].type == DISCLAIMER)
                    {
                        eeprom_read_block(ramString, eepromString+prompts[i].offset,prompts[i].len+1);
                        GDispStringAt(prompts[i].row,prompts[i].col,ramString);
                        printString(ramString);
                        printString("\r\n");
                    }
                }
               printString("\r\ndone displaying disclaimer\r\n");
                break;
            case 'b':
                printString(" displaying menu 1\r\n");
                GDispClrTxt();
                for(i = 0;i < no_prompts;i++)
                {
                    if(prompts[i].type == MENU1)
                    {
                        eeprom_read_block(ramString, eepromString+prompts[i].offset,prompts[i].len+1);
                        GDispStringAt(prompts[i].row,prompts[i].col,ramString);
                        printString(ramString);
                        printString("\r\n");
                    }
                }
				break;
            case 'c':
                printString(" displaying menu 2\r\n");
                GDispClrTxt();
                for(i = 0;i < no_prompts;i++)
                {
                    if(prompts[i].type == MENU2)
                    {
                        eeprom_read_block(ramString, eepromString+prompts[i].offset,prompts[i].len+1);
                        GDispStringAt(prompts[i].row,prompts[i].col,ramString);
                        printString(ramString);
                        printString("\r\n");
                    }
                }
				break;
            case 'd':
                printString(" displaying menu 3\r\n");
                GDispClrTxt();
                for(i = 0;i < no_prompts;i++)
                {
                    if(prompts[i].type == MENU3)
                    {
                        eeprom_read_block(ramString, eepromString+prompts[i].offset,prompts[i].len+1);
                        GDispStringAt(prompts[i].row,prompts[i].col,ramString);
                        printString(ramString);
                        printString("\r\n");
                    }
                }
				break;
			case 'e':
				printString("tesing LCD!\r\n");
				GDispClrTxt();
				k = 0x20;
				for(row = 0;row < ROWS;row++)
				{
					for(col = 0;col < COLUMN;col++)
					{
						dispCharAt(row,col,k);
						_delay_ms(10);
						transmitByte(k);
						if(++k > 0x7e)
							k = 0x20;
					}
				}
				break;
			case 'f':
				GDispClrTxt();
				printString("turning off LCD. Enter '0' to turn back on\r\n");
				GDispStringAt(1,1,"Exiting...");
				_delay_ms(500);
				GDispSetMode(DISPLAY_OFF);
				return (0);
				break;
			case 'g':
                for(i = 0;i < no_prompts;i++)
                {
					printHexByte((uint8_t)prompts[i].pnum);
					transmitByte(0x20);
					printHexByte((uint8_t)prompts[i].row);
					transmitByte(0x20);
					printHexByte((uint8_t)prompts[i].col);
					transmitByte(0x20);
					printHexByte((uint8_t)prompts[i].offset);
					transmitByte(0x20);
					printHexByte((uint8_t)prompts[i].len);
					transmitByte(0x20);
					printHexByte((uint8_t)prompts[i].type);
                    printString("\r\n");
				}
				transmitByte(0x20);
				break;
            case '*':
                col++;
                row++;
                k++;
                CheckRC(&row,&col,&k);
                dispSetCursor(TEXT_ON | CURSOR_BLINK_ON,row,col,LINE_8_CURSOR);
                dispCharAt(row,col,k);
                dispRC(row,col);
				break;
            case 'D':
                col++;
                row++;
                k++;
                CheckRC(&row,&col,&k);
                dispSetCursor(TEXT_ON | CURSOR_BLINK_ON,row,col,LINE_8_CURSOR);
                dispCharAt(row,col,k);
                dispRC(row,col);
				break;
			default:
				break;
        }
    }
#endif	

