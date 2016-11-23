#include <avr/io.h>
#include "../avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include "sfr_helper.h"
#include <avr/eeprom.h>
#include "USART.h"
#include "t6963.h"
#include "macros.h"
#include <string.h>
#define TIME_DELAY1 1
#define DISCLAIMER 0xAA
#define MENU1 0x55
#define MENU2 0x56
#define MENU3 0x57
#define STRING_LEN   255
#define ROWS 16
#define dispCharAt(_row,_col,_char) GDispCharAt((uint16_t)_row,(uint16_t)_col,(UCHAR)_char)
#define dispSetCursor(_mode,_row,_col,_type) GDispSetCursor ((UCHAR)_mode, (uint16_t)_row, (uint16_t)_col, (UCHAR)_type)

/* #include <stdio.h> */
/* #include <stdlib.h> */

// Define EEMEM variables

void update_prompt_struct(uint8_t pnum, UCHAR row, UCHAR col, uint16_t *offset, uint8_t type,char *ramstr);

void dispRC(int row, int col);
void CheckRC(int *row, int *col, UCHAR *k);

void printMenu(void);
typedef struct prompts
{
	uint8_t pnum;
	UCHAR row;
	UCHAR col;
    uint16_t offset;
	int len;
    uint8_t type;
} PROMPT_STRUCT;

char eepromString[STRING_LEN] EEMEM;

PROMPT_STRUCT prompts[30];
uint8_t no_prompts = 0;
uint16_t prompt_info_offset = 0;

void update_prompt_struct(uint8_t pnum, UCHAR row, UCHAR col, uint16_t *offset, uint8_t type,char* ramstr);

int main(void) 
{
    uint8_t test1;
    char ramString[STRING_LEN];
	int i;
    int row,col;
    UCHAR k;
	uint8_t temp;
	uint16_t temp2;

    uint8_t promptString[sizeof(PROMPT_STRUCT)];
    size_t str_size = sizeof(PROMPT_STRUCT);
    uint16_t total_strlen;

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
		no_prompts = eeprom_read_byte((uint8_t*)0x03f0);
		prompt_info_offset = (uint16_t)eeprom_read_byte((uint8_t*)0x03f2);
		temp = eeprom_read_byte((uint8_t*)0x03f3);
		temp2 = (uint16_t)temp;
		printHexByte((uint8_t)temp2);
		printHexByte((uint8_t)temp2>>8);
		prompt_info_offset |= (temp2 << 8);
		transmitByte(0x20);
		printHexByte((uint8_t)prompt_info_offset);
		printHexByte((uint8_t)(prompt_info_offset>>8));
		for(i = 0;i < no_prompts;i++)
		{
			eeprom_read_block((void*)(&(prompts[i])),(eepromString+(i*str_size)+prompt_info_offset), str_size);
		}
	}
	else
	{
		printString("prompts not set - hit 'a' to program eeprom\r\n");
	}


    while (1) 
    {
		printMenu();
        test1 = receiveByte();
        switch(test1)
        {
            case 'a':
                printString("\r\nwriting to eeprom...\r\n");
                total_strlen = 0;
                i = 0;
				update_prompt_struct(i,i,10,&total_strlen,DISCLAIMER,"CAUTION\0");

				i++;
				update_prompt_struct(i,i,0,&total_strlen,DISCLAIMER,"Use of this system does not\0");

				i++;
				update_prompt_struct(i,i,0,&total_strlen,DISCLAIMER,"replace basic safety precautions\0");

				i++;
				update_prompt_struct(i,i,0,&total_strlen,DISCLAIMER,"and procedures for operating the\0");

				i++;
				update_prompt_struct(i,i,0,&total_strlen,DISCLAIMER,"machine. Do not operate the\0");

				i++;
				update_prompt_struct(i,i,0,&total_strlen,DISCLAIMER,"machine while system communi-\0");

				i++;
				update_prompt_struct(i,i,0,&total_strlen,DISCLAIMER,"cation are being established\0");

				i++;
				update_prompt_struct(i,i,0,&total_strlen,DISCLAIMER,"or diagnostic codes are\0");

				i++;
				update_prompt_struct(i,i,0,&total_strlen,DISCLAIMER,"present. Refer to the Operation\0");

				i++;
				update_prompt_struct(i,i,0,&total_strlen,DISCLAIMER,"and Maintenance Manual of the\0");

				i++;
				update_prompt_struct(i,i,0,&total_strlen,DISCLAIMER,"machine for additional\0");

				i++;
				update_prompt_struct(i,i,0,&total_strlen,DISCLAIMER,"information.\0");

				i++;
				update_prompt_struct(i,15,0,&total_strlen,MENU1,"MENU1a\0");

				i++;
				update_prompt_struct(i,15,8,&total_strlen,MENU1,"MENU2a\0");

				i++;
				update_prompt_struct(i,15,16,&total_strlen,MENU1,"MENU3a\0");

				i++;
				update_prompt_struct(i,15,24,&total_strlen,MENU1,"MENU4a\0");

				i++;
				update_prompt_struct(i,15,0,&total_strlen,MENU2,"MENU1b\0");

				i++;
				update_prompt_struct(i,15,8,&total_strlen,MENU2,"MENU2b\0");

				i++;
				update_prompt_struct(i,15,16,&total_strlen,MENU2,"MENU3b\0");

				i++;
				update_prompt_struct(i,15,24,&total_strlen,MENU2,"MENU4b\0");

				i++;
				update_prompt_struct(i,15,0,&total_strlen,MENU3,"MENU1c\0");

				i++;
				update_prompt_struct(i,15,0,&total_strlen,MENU3,"MENU2c\0");

				i++;
				update_prompt_struct(i,15,0,&total_strlen,MENU3,"MENU3c\0");

				i++;
				update_prompt_struct(i,15,0,&total_strlen,MENU3,"MENU4c\0");

                no_prompts = i+1;
                prompt_info_offset = total_strlen;
				printString("\r\n");
				printHexByte((uint8_t)prompt_info_offset>>8);
				transmitByte(0x20);
				printHexByte((uint8_t)prompt_info_offset);
				transmitByte(0x20);
				printHexByte((uint8_t)no_prompts);
				eeprom_update_byte((uint8_t *)0x03f0,no_prompts);
				eeprom_update_word((uint16_t *)0x03f2,prompt_info_offset);

				printString("\r\ndone writing prompts to eeprom\r\n");

                for(i = 0;i < no_prompts;i++)
                {
					// memcpy(dest,src,size)
                    memcpy((void*)(promptString),(void *)(&(prompts[i])),str_size);
					// eeprom_block_update(src,dest,size)
                    eeprom_update_block(promptString,(eepromString+((i*(uint8_t)str_size))+prompt_info_offset), str_size);
                }
				printString("done writing prompts structs to eeprom\r\n");
                break;
            case 'b':
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
            case 'c':
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
            case 'd':
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


void printMenu()
{
	printString("\r\na - burn eeprom\r\n");
	printString("b - display disclaimer\r\n");
	printString("c - display menu 1\r\n");
	printString("d - display menu 2\r\n");
	printString("e - test LCD\r\n");
	printString("f - turn off LCD\r\n");
	printString("g - print prompt info\r\n");
}
void update_prompt_struct(uint8_t pnum, UCHAR row, UCHAR col, uint16_t *offset, uint8_t type,char *ramstr)
{
	int len;	
	printString(ramstr);
	len = strlen(ramstr) + 1;
	prompts[pnum].len = len;
	prompts[pnum].pnum = pnum;
	prompts[pnum].row = row;
    prompts[pnum].offset = *offset;
	prompts[pnum].col = col;
    prompts[pnum].type = type;
    eeprom_update_block(ramstr, eepromString+*offset, len);
    *offset += len+1;
}

/*
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
*/


