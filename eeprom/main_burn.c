#include <avr/io.h>
#include "../../avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include "../sfr_helper.h"
#include <avr/eeprom.h>
#include "../USART.h"
#include "../t6963.h"
#include "../macros.h"
#include "../main.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
void printMenu(void);

char eepromString[STRING_LEN] EEMEM;

int main(void)
{
    uint8_t test1;
    char ramString[STRING_LEN];
	int i,j;
	char *ch;
    int row,col;
    UCHAR k;
	int offset = 0;

    initUSART();

#ifdef SCREEN_EN
#warning "SCREEN_EN defined"
	GDispInit();
	_delay_us(10);
	GDispSetMode(XOR_MODE);
	_delay_us(10);
	GDispSetMode(TEXT_ON);
	_delay_us(10);
	GDispClrTxt();
	printString("screen is enabled\r\n");
	GDispStringAt(10, 10, "LCD is On!");
#endif
//	printMenu();

    while (1)
    {
        test1 = receiveByte();
//        printHexByte(test1);
        switch(test1)
        {
			case 'a':
				if(burn_eeprom() == 1)
					printString("error in burn_eeprom\r\n");
				break;
			case 'b':
				if(read_eeprom() == 1)
					printString("error in read_eeprom\r\n");
				break;
            case 'c':
                printString("displaying labels\r\n");
                for(i = 0;i < no_labels;i++)
                {
					j = 0;
					memset(ramString,0,STRING_LEN);
					eeprom_read_block(ramString, eepromString+offset,label_offsets[i]);
					offset += label_offsets[i];
					ch = ramString;
					while(ch++ != 0 && j++ < MAX_LABEL_LEN);
					*ch = 0;
					printString(ramString);
					printString("\r\n");
                }
//               printString("\r\ndone displaying RT labels\r\n");
				break;
			case 'd':
                printString(" displaying rt_params\r\n");
/*
	UCHAR row;			// row, col tells where the param will appear on screen
	UCHAR col;
	UCHAR shown;		// if its shown or not
	UCHAR type;			// 0 - UCHAR; 1 - UINT; 2 - string
*/
                for(i = 0;i < no_rtparams;i++)
                {
					j = 0;
					eeprom_read_block(&rt_params[i], eepromString+(rt_params_offset+(sizeof(RT_PARAM)*i)),sizeof(RT_PARAM));
					printHexByte((UCHAR)(rt_params[i].row));
					printHexByte(rt_params[i].col);
					printHexByte(rt_params[i].shown);
					printHexByte(rt_params[i].type);
					ch = ramString;
					while(ch++ != 0 && j++ < MAX_LABEL_LEN);
					*ch = 0;
					printString(ramString);
					printString("\r\n");
                }
                break;
			case 'e':
/*
	UCHAR pnum;			// index
	UCHAR row;			// row, col where the label will be displayed
	UCHAR col;
	UCHAR menu_choice;	// the type of keypress in the switch statement of the menu
	UCHAR type;			// index of which function pointer this menu label is associated with
*/
                printString(" displaying menu structs\r\n");
                for(i = 0;i < no_menu_structs;i++)
                {
/*
	UCHAR enabled;		// if alt function will replace generic function
	UCHAR key;			// which keypress applies
	UCHAR fptr;			// which function to call (menu_types)
	UCHAR menu;			// if fptr == 0 then it means goto a this menu
	UCHAR label;			// which label to display in legend (labels)
*/
					eeprom_read_block(&menu_structs[i],  \
					eepromString+menu_struct_offset+(sizeof(MENU_FUNC_STRUCT)*i),sizeof(MENU_FUNC_STRUCT));
					printHexByte(menu_structs[i].enabled);
					printHexByte(menu_structs[i].fptr);
					printHexByte(menu_structs[i].menu);
					printHexByte(menu_structs[i].label);
					// TODO: look up the label in the table for labels and print it out here
					printString("\r\n");
                }
                break;
			case 'f':
                printString(" unimplemented\r\n");
                break;
			case 'g':
                printString(" unimplemented\r\n");
				break;
			case 'h':
				printMenu();
				break;
			case 'i':
				printString("tesing pattern 1\r\n");
#ifdef SCREEN_EN
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
			case 'j':
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
			case 'k':
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
			case 'l':
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
			case 'm':
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
			case 'n':
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
			case 'o':
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
			case 'p':
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
				break;
#endif
			case 'q':
				for(k = 0x21;k < 0x7e;k++)
					transmitByte(k);
				printString("done\r\n");
				break;
			case 'r':
				GDispClrTxt();
				break;
		default:
			break;
        }
    }
    return (0);
}


void printMenu(void)
{
	printString("\r\na - burn eeprom\r\n");
	printString("b - read data from eeprom into ram\r\n");
	printString("c - display labels\r\n");
	printString("d - display rt_params\r\n");
	printString("e - display menu_structs\r\n");
	printString("f - \r\n");
	printString("g - \r\n");
	printString("h - help (menu)\r\n");
	printString("i - test pattern 1\r\n");
	printString("j - test pattern 2\r\n");
	printString("k - test pattern 3\r\n");
	printString("l - test pattern 4\r\n");
	printString("m - test pattern 1 (fast)\r\n");
	printString("n - test pattern 2 (fast)\r\n");
	printString("o - test pattern 3 (fast)\r\n");
	printString("p - test pattern 4 (fast)\r\n");
	printString("q - (sorry, there is no quit)\r\n");
	printString("r - clear screen\r\n");
}

