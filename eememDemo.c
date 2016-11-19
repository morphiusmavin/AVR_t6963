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
#define MENU 0x55
#define STRING_LEN   255
#define ROWS 16
#define dispCharAt(_row,_col,_char) GDispCharAt((uint16_t)_row,(uint16_t)_col,(UCHAR)_char)

/* #include <stdio.h> */
/* #include <stdlib.h> */

// Define EEMEM variables

void update_prompt_struct(uint8_t pnum, UCHAR row, UCHAR col, uint16_t offset, int len, uint8_t type);
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
const void *eepromBuffer[sizeof(PROMPT_STRUCT)] EEMEM;

PROMPT_STRUCT prompts[20];
uint16_t prompt_info_offset;

//uint8_t eepromCounter EEMEM = 0;
//uint16_t eepromWord EEMEM = 12345;
void update_prompt_struct(uint8_t pnum, UCHAR row, UCHAR col, uint16_t offset, int len, uint8_t type);


int main(void) 
{
    uint8_t test1;
    char ramString[STRING_LEN];
    uint8_t counter;
	int i,j;
    int row,col;
    UCHAR k;

    uint8_t no_prompts = 0;
    uint8_t promptString[sizeof(PROMPT_STRUCT)];
    size_t str_size = sizeof(PROMPT_STRUCT);
    int total_strlen;

    initUSART();

	printString("turning back on..\r\n");
	GDispInit();
	_delay_us(10);
	GDispSetMode(XOR_MODE);
	_delay_us(10);
	GDispSetMode(TEXT_ON);
	_delay_us(10);
	GDispClrTxt();    
	GDispStringAt(1,1,"LCD on!");
	printString("LCD is on.\r\n");
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

    while (1) 
    {
        test1 = receiveByte();
        switch(test1)
        {
/*
            case 'a':

                printString("\r\n------------------\r\n");
                eeprom_read_block(ramString, eepromString, STRING_LEN);
                printString(ramString);

                printString("\r\nThe counter reads: ");
                counter = eeprom_read_byte(&eepromCounter);
                printByte(counter);

                printString("\r\nMy uint16_t value is: ");
                printWord(eeprom_read_word(&eepromWord));

                printString("\r\n   Enter a new introduction string below:\r\n");
                readString(ramString, STRING_LEN);
                eeprom_update_block(ramString, eepromString, STRING_LEN);
                counter++;
                eeprom_update_byte(&eepromCounter, counter);
                printString("done writing test data to eeprom\r\n");
*/
				
                break;
            case 'a':
                printString("writing to eeprom...\r\n");
                total_strlen = 0;
                i = 0;
                strcpy(ramString,"CAUTION\0");
				update_prompt_struct(i,i,10,total_strlen,strlen(ramString),DISCLAIMER);
	            eeprom_update_block(ramString, eepromString+total_strlen, strlen(ramString)+1);
                total_strlen += strlen(ramString)+1;

                strcpy(ramString,"Use of this system does not\0");
				i++;
				update_prompt_struct(i,i,0,total_strlen,strlen(ramString),DISCLAIMER);
	            eeprom_update_block(ramString, eepromString+total_strlen, strlen(ramString)+1);
                total_strlen += strlen(ramString)+1;

                strcpy(ramString,"replace basic safety precautions\0");
				i++;
				update_prompt_struct(i,i,0,total_strlen,strlen(ramString),DISCLAIMER);
	            eeprom_update_block(ramString, eepromString+total_strlen, strlen(ramString)+1);
                total_strlen += strlen(ramString)+1;

                strcpy(ramString,"and procedures for operating the\0");
				i++;
				update_prompt_struct(i,i,0,total_strlen,strlen(ramString),DISCLAIMER);
	            eeprom_update_block(ramString, eepromString+total_strlen, strlen(ramString)+1);
                total_strlen += strlen(ramString)+1;

                strcpy(ramString,"machine. Do not operate the\0");
				i++;
				update_prompt_struct(i,i,0,total_strlen,strlen(ramString),DISCLAIMER);
	            eeprom_update_block(ramString, eepromString+total_strlen, strlen(ramString)+1);
                total_strlen += strlen(ramString)+1;

                strcpy(ramString,"machine while system communi-\0");
				i++;
				update_prompt_struct(i,i,0,total_strlen,strlen(ramString),DISCLAIMER);
	            eeprom_update_block(ramString, eepromString+total_strlen, strlen(ramString)+1);
                total_strlen += strlen(ramString)+1;

                strcpy(ramString,"cation are being established\0");
				i++;
				update_prompt_struct(i,i,0,total_strlen,strlen(ramString),DISCLAIMER);
	            eeprom_update_block(ramString, eepromString+total_strlen, strlen(ramString)+1);
                total_strlen += strlen(ramString)+1;

                strcpy(ramString,"or diagnostic codes are\0");
				i++;
				update_prompt_struct(i,i,0,total_strlen,strlen(ramString),DISCLAIMER);
	            eeprom_update_block(ramString, eepromString+total_strlen, strlen(ramString)+1);
                total_strlen += strlen(ramString)+1;

                strcpy(ramString,"present. Refer to the Operation\0");
				i++;
				update_prompt_struct(i,i,0,total_strlen,strlen(ramString),DISCLAIMER);
	            eeprom_update_block(ramString, eepromString+total_strlen, strlen(ramString)+1);
                total_strlen += strlen(ramString)+1;

                strcpy(ramString,"and Maintenance Manual of the\0");
				i++;
				update_prompt_struct(i,i,0,total_strlen,strlen(ramString),DISCLAIMER);
	            eeprom_update_block(ramString, eepromString+total_strlen, strlen(ramString)+1);
                total_strlen += strlen(ramString)+1;

                strcpy(ramString,"machine for additional\0");
				i++;
				update_prompt_struct(i,i,0,total_strlen,strlen(ramString),DISCLAIMER);
	            eeprom_update_block(ramString, eepromString+total_strlen, strlen(ramString)+1);
                total_strlen += strlen(ramString)+1;

                strcpy(ramString,"information.    \0");
				i++;
				update_prompt_struct(i,i,0,total_strlen,strlen(ramString),DISCLAIMER);
	            eeprom_update_block(ramString, eepromString+total_strlen, strlen(ramString)+1);
                total_strlen += strlen(ramString)+1;

                strcpy(ramString,"TEST1\0");
				i++;
				update_prompt_struct(i,15,0,total_strlen,strlen(ramString),MENU);
	            eeprom_update_block(ramString, eepromString+total_strlen, strlen(ramString)+1);
                total_strlen += strlen(ramString)+1;

                strcpy(ramString,"TEST2\0");
				i++;
				update_prompt_struct(i,15,8,total_strlen,strlen(ramString),MENU);
	            eeprom_update_block(ramString, eepromString+total_strlen, strlen(ramString)+1);
                total_strlen += strlen(ramString)+1;

                strcpy(ramString,"TEST3\0");
				i++;
				update_prompt_struct(i,15,16,total_strlen,strlen(ramString),MENU);
	            eeprom_update_block(ramString, eepromString+total_strlen, strlen(ramString)+1);
                total_strlen += strlen(ramString)+1;

                strcpy(ramString,"TEST4\0");
				i++;
				update_prompt_struct(i,15,24,total_strlen,strlen(ramString),MENU);
	            eeprom_update_block(ramString, eepromString+total_strlen, strlen(ramString)+1);
                total_strlen += strlen(ramString)+1;

                no_prompts = i+1;
                prompt_info_offset = total_strlen;

				transmitByte(0x20);
				transmitByte(0x20);
				printHexByte((uint8_t)prompt_info_offset>>8);
				printHexByte((uint8_t)prompt_info_offset);
				transmitByte(0x20);
				transmitByte(0x20);
				printHexByte((uint8_t)no_prompts);
				eeprom_update_byte((uint8_t *)0x0200,no_prompts);
				eeprom_update_word((uint16_t *)0x0210,prompt_info_offset);


				printString("\r\ndone writing prompts to eeprom\r\n");

                for(i = 0;i < no_prompts;i++)
                {
// memcpy(dest,src,size)
                    memcpy((void*)(promptString),(void *)(&(prompts[i])),str_size);
// eeprom_block_update(src,dest,size)
//                    eeprom_update_block(promptString,(eepromBuffer+(i*(uint8_t)str_size)+total_strlen), str_size);
                    eeprom_update_block(promptString,(eepromString+((i*(uint8_t)str_size))+prompt_info_offset), str_size);
                }
				printString("done writing prompts structs to eeprom\r\n");
                break;
            case 'b':
                printString("reading eeprom\r\n");
                for(i = 0;i < no_prompts;i++)
                {
	                eeprom_read_block(ramString,eepromString+prompts[i].offset, prompts[i].len);
//                    for(j=0;j<prompts[i].len;j++)
//                       transmitByte(ramString[j]);
                    ramString[prompts[i].len] = 0;    
//                    printByte(ramString[j]+1);
	                printString(ramString);
                    printString("\r\n");
                }
                printString("done\r\n");
                break;
            case 'c':
                printString("reading prompt data into prompt structs\r\n");
                no_prompts = eeprom_read_byte((uint8_t*)0x0200);
				prompt_info_offset = eeprom_read_word((uint16_t*)0x0210);
				printHexByte((uint8_t)prompt_info_offset>>8);
				printHexByte((uint8_t)prompt_info_offset);
				transmitByte(0x20);
				printHexByte((uint8_t)no_prompts);
                for(i = 0;i < no_prompts;i++)
                {
                    eeprom_read_block(promptString,(eepromString+(i*(uint8_t)str_size)+0x0162), str_size);
//                    eeprom_read_block(ramString, eepromString+prompts[i].offset,prompts[i].len+1);
                }
				printString("done\r\n");
                break;
            case 'd':
                printString("displaying disclaimer\r\n");
                GDispClrTxt();
//                for(i = 0;i < no_prompts;i++)
                for(i = 0;i < 16;i++)
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
            case 'e':
                printString("displaying menus\r\n");
                GDispClrTxt();
//                for(i = 0;i < no_prompts;i++)
                for(i = 0;i < 16;i++)
                {
                    if(prompts[i].type == MENU)
                    {
                        eeprom_read_block(ramString, eepromString+prompts[i].offset,prompts[i].len+1);
                        GDispStringAt(prompts[i].row,prompts[i].col,ramString);
                        printString(ramString);
                        printString("\r\n");
                    }
                }
				break;
			case 'f':
				printString("tesing LCD\r\n");
				GDispClrTxt();
				k = 0x20;
				for(row = 0;row < ROWS;row++)
				{
					_delay_ms(200);
					for(col = 0;col < COLUMN;col++)
					{
						dispCharAt(row,col,k);
						_delay_us(500);
						transmitByte(k);
						if(++k > 0x7e)
							k = 0x20;
					}
				}
				break;
			case 'g':
				GDispClrTxt();
				printString("turning off LCD. Enter '0' to turn back on\r\n");
				GDispStringAt(1,1,"Exiting...");
				_delay_ms(500);
				GDispSetMode(DISPLAY_OFF);
				return (0);
				break;
			case 'h':
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
					transmitByte(0x20);
					transmitByte(0x20);
				}
				transmitByte(0x20);
				break;
			case 'i':
				break;
			default:
				break;
        }
    }
    return (0);
}

void update_prompt_struct(uint8_t pnum, UCHAR row, UCHAR col, uint16_t offset, int len, uint8_t type)
{
	prompts[pnum].len = len;
	prompts[pnum].pnum = pnum;
	prompts[pnum].row = row;
    prompts[pnum].offset = offset;
	prompts[pnum].col = col;
    prompts[pnum].type = type;
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
}

