#include <avr/io.h>
#include "../avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include "sfr_helper.h"
#include <avr/eeprom.h>
#include "USART.h"
#include "t6963.h"
#include "macros.h"
#include "main.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void update_prompt_struct(UCHAR pnum, UCHAR row, UCHAR col, uint16_t *offset, uint8_t type,char *ramstr);
void printMenu(void);
int burn_eeprom(void);
int read_eeprom(void);

char eepromString[STRING_LEN] EEMEM;

PROMPT_STRUCT prompts[30];
							
int main(void) 
{
    uint8_t test1;
    char ramString[STRING_LEN];
	int i;
    int row,col;
    UCHAR k;
	
	uint8_t no_prompts = 0;

    initUSART();

	GDispInit();
	_delay_us(10);
	GDispSetMode(XOR_MODE);
	_delay_us(10);
	GDispSetMode(TEXT_ON);
	_delay_us(10);
	GDispClrTxt();    
//	printMenu();

    no_prompts = eeprom_read_byte((uint8_t*)NO_PROMPTS_EEPROM_LOCATION);

	printString("starting eeprom_burn...\r\n");
	if(no_prompts == 0xff)
		printString("no_prompts is blank\r\n");
/*		
	test_ptr = (UCHAR *)malloc((size_t)1000);
	if(test_ptr == NULL)
	{
		printString("test_ptr == NULL\r\n");
	}
	else
	{
		memset((void*)test_ptr,0,sizeof(test_ptr));
		for(i = 0;i < 1000;i++)
			*(test_ptr + i) = i;
		for(i = 0;i < 1000;i++)
			printHexByte(i);
	}		
*/		
    while (1) 
    {
        test1 = receiveByte();
        printHexByte(test1);
        switch(test1)
        {
			case KP_A:
				if(burn_eeprom() == 1)
					printString("error in burn_eeprom\r\n");
				break;
			case KP_B:
				if(read_eeprom() == 1)
					printString("error in read_eeprom\r\n");
				break;
            case KP_C:
                printString("displaying RT labels\r\n");
                GDispClrTxt();
                for(i = 0;i < no_prompts;i++)
                {
                    if(prompts[i].type == RT_LABEL)
                    {
                        eeprom_read_block(ramString, eepromString+prompts[i].offset,prompts[i].len+1);
                        GDispStringAt(prompts[i].row,prompts[i].col,ramString);

                        printString(ramString);
                        printString("\r\n");
                    }
                }
//               printString("\r\ndone displaying RT labels\r\n");
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
                printString(" displaying menu 4\r\n");
                GDispClrTxt();
                for(i = 0;i < no_prompts;i++)
                {
                    if(prompts[i].type == MENU4)
                    {
                        eeprom_read_block(ramString, eepromString+prompts[i].offset,prompts[i].len+1);
                        GDispStringAt(prompts[i].row,prompts[i].col,ramString);
                        printString(ramString);
                        printString("\r\n");
                    }
                }
				break;
			case 'h':
				printMenu();
				break;
			case KP_1:
				printString("tesing pattern 1\r\n");
				GDispClrTxt();
				k = 0x20;
				for(row = 0;row < ROWS;row++)
				{
					for(col = 0;col < COLUMN;col++)
					{
						dispCharAt(row,col,k);
						_delay_ms(1);
//						transmitByte(k);
						if(++k > 0x7e)
							k = 0x20;
					}
				}
				printString("done\r\n");
				break;
			case KP_2:
				printString("tesing pattern 2\r\n");
				GDispClrTxt();
				k = 0x30;
				for(row = 0;row < ROWS;row++)
				{
					for(col = 0;col < COLUMN;col++)
					{
						dispCharAt(row,col,k);
						_delay_ms(1);
//						transmitByte(k);
						if(++k > 0x39)
							k = 0x30;
					}
				}
				printString("done\r\n");
				break;
			case KP_3:
				printString("tesing pattern 3\r\n");
				GDispClrTxt();
				k = 0x41;
				for(row = 0;row < ROWS;row++)
				{
					for(col = 0;col < COLUMN;col++)
					{
						dispCharAt(row,col,k);
						_delay_ms(1);
//						transmitByte(k);
						if(++k > 0x5A)
							k = 0x41;
					}
				}
				printString("done\r\n");
				break;
			case KP_4:
				printString("tesing pattern 4\r\n");
				GDispClrTxt();
				k = 0x61;
				for(row = 0;row < ROWS;row++)
				{
					for(col = 0;col < COLUMN;col++)
					{
						dispCharAt(row,col,k);
						_delay_ms(1);
//						transmitByte(k);
						if(++k > 0x7A)
							k = 0x61;
					}
				}
				printString("done\r\n");
				break;
			case KP_5:
				printString("tesing pattern 1 (fast)\r\n");
				GDispClrTxt();
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
				printString("done\r\n");
				break;
			case KP_6:
				printString("tesing pattern 2 (fast)\r\n");
				GDispClrTxt();
				k = 0x30;
				for(row = 0;row < ROWS;row++)
				{
					for(col = 0;col < COLUMN;col++)
					{
						dispCharAt(row,col,k);
						if(++k > 0x39)
							k = 0x30;
					}
				}
				printString("done\r\n");
				break;
			case KP_7:	
				printString("tesing pattern 3 (fast)\r\n");
				GDispClrTxt();
				k = 0x41;
				for(row = 0;row < ROWS;row++)
				{
					for(col = 0;col < COLUMN;col++)
					{
						dispCharAt(row,col,k);
						if(++k > 0x5A)
							k = 0x41;
					}
				}
				printString("done\r\n");
				break;
			case KP_8:
				printString("tesing pattern 4 (fast)\r\n");
				GDispClrTxt();
				k = 0x61;
				for(row = 0;row < ROWS;row++)
				{
					for(col = 0;col < COLUMN;col++)
					{
						dispCharAt(row,col,k);
						if(++k > 0x7A)
							k = 0x61;
					}
				}
				printString("done\r\n");
				break;
/*				
            case KP_AST:
                row++;
                k++;
                CheckRC(&row,&col,&k);
                dispSetCursor(TEXT_ON | CURSOR_BLINK_ON,row,col,LINE_8_CURSOR);
                dispCharAt(row,col,k);
                dispRC(row,col);
				break;
            case KP_0:
                row--;
                k++;
                CheckRC(&row,&col,&k);
                dispSetCursor(TEXT_ON | CURSOR_BLINK_ON,row,col,LINE_8_CURSOR);
                dispCharAt(row,col,k);
                dispRC(row,col);
				break;
            case KP_POUND:
                col++;
                k++;
                CheckRC(&row,&col,&k);
                dispSetCursor(TEXT_ON | CURSOR_BLINK_ON,row,col,LINE_8_CURSOR);
                dispCharAt(row,col,k);
                dispRC(row,col);
				break;
            case KP_D:
                col--;
                k++;
                CheckRC(&row,&col,&k);
                dispSetCursor(TEXT_ON | CURSOR_BLINK_ON,row,col,LINE_8_CURSOR);
                dispCharAt(row,col,k);
                dispRC(row,col);
				break;
*/				
			default:
				break;
        }
    }
    return (0);
}

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

int burn_eeprom(void)
{
	int i;
	uint8_t no_prompts = 0;
	uint16_t prompt_info_offset = 0;
    uint16_t total_strlen = 0;
    uint8_t promptString[sizeof(PROMPT_STRUCT)];
    size_t str_size = sizeof(PROMPT_STRUCT);
	
    printString("\r\nwriting to eeprom...\r\n");
    i = 0;
    // the row,col elements don't really apply for the RT params
    // because the are displayed according to the row,col elements of the rt_layout
	update_prompt_struct((UCHAR)i,(UCHAR)i,0,&total_strlen,RT_LABEL,"RPM\0");

	i++;
	update_prompt_struct((UCHAR)i,(UCHAR)i,0,&total_strlen,RT_LABEL,"MPH\0");

	i++;
	update_prompt_struct((UCHAR)i,(UCHAR)i,0,&total_strlen,RT_LABEL,"ENG TEMP\0");

	i++;
	update_prompt_struct((UCHAR)i,(UCHAR)i,0,&total_strlen,RT_LABEL,"OIL PRESS\0");

	i++;
	update_prompt_struct((UCHAR)i,(UCHAR)i,0,&total_strlen,RT_LABEL,"OIL TEMP\0");

	i++;
	update_prompt_struct((UCHAR)i,(UCHAR)i-5,15,&total_strlen,RT_LABEL,"MAP\0");

	i++;
	update_prompt_struct((UCHAR)i,(UCHAR)i-5,15,&total_strlen,RT_LABEL,"O2\0");

	i++;
	update_prompt_struct((UCHAR)i,(UCHAR)i-5,15,&total_strlen,RT_LABEL,"AIR TEMP\0");

	i++;
	update_prompt_struct((UCHAR)i,(UCHAR)i-5,15,&total_strlen,RT_LABEL,"TIME\0");

	i++;
	update_prompt_struct((UCHAR)i,(UCHAR)i-5,15,&total_strlen,RT_LABEL,"TRIP\0");

	i++;
	// the row, col elements specify where at the bottom of the screen 
	// menu choices will be
	update_prompt_struct((UCHAR)i,15,0,&total_strlen,MENU1,"MENU1a\0");

	i++;
	update_prompt_struct((UCHAR)i,15,8,&total_strlen,MENU1,"MENU1b\0");

	i++;
	update_prompt_struct((UCHAR)i,15,16,&total_strlen,MENU1,"MENU1c\0");

	i++;
	update_prompt_struct((UCHAR)i,15,24,&total_strlen,MENU1,"MENU1d\0");

	i++;
	update_prompt_struct((UCHAR)i,15,8,&total_strlen,MENU2,"MENU2a\0");

	i++;
	update_prompt_struct((UCHAR)i,15,16,&total_strlen,MENU2,"MENU2b\0");

	i++;
	update_prompt_struct((UCHAR)i,15,24,&total_strlen,MENU2,"MENU2c\0");

	i++;
	update_prompt_struct((UCHAR)i,15,8,&total_strlen,MENU3,"MENU3a\0");

	i++;
	update_prompt_struct((UCHAR)i,15,16,&total_strlen,MENU3,"MENU3b\0");

	i++;
	update_prompt_struct((UCHAR)i,15,24,&total_strlen,MENU3,"MENU3c\0");

	i++;
	update_prompt_struct((UCHAR)i,15,8,&total_strlen,MENU4,"MENU4a\0");

	i++;
	update_prompt_struct((UCHAR)i,15,16,&total_strlen,MENU4,"MENU4b\0");

	i++;
	update_prompt_struct((UCHAR)i,15,24,&total_strlen,MENU4,"MENU4c\0");

	i++;
	update_prompt_struct((UCHAR)i,15,0,&total_strlen,MENU5,"NUM_ENTRY\0");

	i++;
	update_prompt_struct((UCHAR)i,15,0,&total_strlen,MENU5,"ALNUM_ENTRY\0");

    no_prompts = i+1;
    prompt_info_offset = total_strlen;
	printString("\r\n");
	printHexByte((uint8_t)prompt_info_offset>>8);
	transmitByte(0x20);
	printHexByte((uint8_t)prompt_info_offset);
	transmitByte(0x20);
	printHexByte((uint8_t)no_prompts);

	eeprom_update_byte((uint8_t *)NO_PROMPTS_EEPROM_LOCATION,no_prompts);
	eeprom_update_word((uint16_t *)PROMPT_INFO_OFFSET_EEPROM_LOCATION_LSB,prompt_info_offset);

	printString("\r\ndone writing prompts to eeprom\r\n");

    for(i = 0;i < no_prompts;i++)
    {
		// memcpy(dest,src,size)
        memcpy((void*)(promptString),(void *)(&(prompts[i])),str_size);
		// eeprom_block_update(src,dest,size)

        eeprom_update_block(promptString,(eepromString+((i*(uint8_t)str_size))+prompt_info_offset), str_size);
    }
	printString("done writing eeprom\r\n");
	return 0;
}

int read_eeprom(void)
{
	int i;
	uint8_t no_prompts = 0;
	uint16_t prompt_info_offset = 0;
	uint8_t temp;
	uint16_t temp2;
	
	printString("reading prompt data into prompt structs\r\n");
	no_prompts = eeprom_read_byte((uint8_t*)NO_PROMPTS_EEPROM_LOCATION);
	printString("no_prompts: ");
	printHexByte(no_prompts);
	printString("\r\n");
	prompt_info_offset = (uint16_t)eeprom_read_byte((uint8_t*)PROMPT_INFO_OFFSET_EEPROM_LOCATION_LSB);	// read lsb
	temp = eeprom_read_byte((uint8_t*)PROMPT_INFO_OFFSET_EEPROM_LOCATION_MSB);		// read msb
	temp2 = (uint16_t)temp;
	prompt_info_offset |= (temp2 << 8);		// shift msb over and bit-or with lsb
	printString("prompt_info_offset: ");
	printHexByte((uint8_t)(prompt_info_offset>>8));
	printHexByte((uint8_t)prompt_info_offset);
	printString("\r\n");
	printString("\r\n");

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
	printString("done reading eeprom\r\n");
	return 0;
}

void printMenu(void)
{
	printString("\r\na - burn eeprom\r\n");
	printString("b - read data from eeprom into ram\r\n");
	printString("c - display rt labels\r\n");
	printString("d - display menu 1\r\n");
	printString("e - display menu 2\r\n");
	printString("f - display menu 3\r\n");
	printString("g - test LCD\r\n");
	printString("h - help (menu)\r\n");
	printString("i - display prompt data currently in ram\r\n");
	printString("j - print layout info\r\n");
}
void update_prompt_struct(UCHAR pnum, UCHAR row, UCHAR col, uint16_t *offset, uint8_t type,char *ramstr)
{
	int len;	
	printString(ramstr);
	printString("\r\n");
	len = strlen(ramstr) + 1;
	prompts[pnum].len = len;
	prompts[pnum].pnum = pnum;
	prompts[pnum].row = row;
    prompts[pnum].offset = *offset;
	prompts[pnum].col = col;
    prompts[pnum].type = type;

    eeprom_update_block(ramstr, eepromString+*offset, len);

    *offset += len;
}

