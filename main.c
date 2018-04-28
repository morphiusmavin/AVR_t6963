//******************************************************************************************//
//*************************************** main.c  ******************************************//
//******************************************************************************************//
// main program that drives the t6963 LCD (32 col x 15 row) display - text only
// see t6963_notes.txt for more details
#include <avr/io.h>
//#include "../avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include "../../Atmel_other/avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include "sfr_helper.h"
#include <avr/eeprom.h>
#include <stdlib.h>
#include "USART.h"
#include "t6963.h"
//#include "spi.h"
#include "macros.h"
#include <string.h>
//#include "main.h"
#include <stdio.h>
#include <stdlib.h>

#define LEN 200
#define DEBUG_CHAR_AT 0
#define DEBUG_STRING_AT 1
#define DEBUG_SET_CURSOR 2
#define DEBUG_CLRSCR1 3
#define DEBUG_CLRSCR2 4
#define DEBUG_CLRSCR3 5
#define DEBUG_MSG1 6
#define COLUMN              40      //Set column number to be e.g. 32 for 8x8 fonts, 2 pages
#define ROWS                16
// really cranking
#define TIME_DELAY 2

int main(void)
{
	int i,j,k;
	UCHAR xbyte;
	UCHAR ch;
	UCHAR key;
	UCHAR buff[LEN];
	UCHAR row, col, mode, type;
	char str[30];
	UCHAR str_len;

//#if 0
	GDispInit();
    initUSART();
//	GDispCmdAddrSend (0x0002, OFFSET_REG_SET);
	_delay_us(10);
	GDispSetMode(XOR_MODE);
//	GDispSetMode(EXT_CG_MODE);
	_delay_us(10);
	GDispSetMode(TEXT_ON);
	_delay_us(10);
	GDispClrTxt();
	GDispStringAt(7,15,"LCD is on!");
//#endif

//	initSPImaster();
//******************************************************************************************//
//*********************************** start of main loop ***********************************//
//******************************************************************************************//
	_delay_ms(1000);
	GDispStringAt(7,15,"          ");

	xbyte = 0x30;
	
	i = 0;
	k = 0;
	col = row = 0;		
    while (1)
    {
//		xbyte = receiveByte();
		_delay_ms(5);
		GDispCharAt(row,col,xbyte);
		transmitByte(xbyte);
//		if(++xbyte > 0x7e)
		if(++i > 40)
		{
			xbyte = 0x21;
			i = 0;
		}else xbyte++;

		if(++col > COLUMN-1)
		{
			col = 0;
			if(++row > ROWS-1)
			{
				row = 0;
				_delay_ms(600);
				GDispClrTxt();
			}
		}
//		transmitByte(key);
//    	_delay_ms(10);
/*
		buff[i] = key;
		i++;
		if(key == 0xfe)
		{
			switch(buff[0])
			{
				case DEBUG_CHAR_AT:
					row = buff[1];
					col = buff[2];
					ch = buff[3];
					GDispCharAt((UINT)row,(UINT)col,ch);
				break;
				case DEBUG_STRING_AT:
					row = buff[1];
					col = buff[2];
					str_len = buff[3];

					memset(str,0x20,sizeof(str));	
					memcpy(str,&buff[4],str_len);
					str[str_len] = 0;
					GDispStringAt((UINT)row,(UINT)col,str);
				break;
				case DEBUG_SET_CURSOR:
					mode = buff[1];
					row = buff[2];
					col = buff[3];
					type = buff[4];
					GDispSetCursor(mode,(UINT)row,(UINT)col,type);
				break;
				case DEBUG_CLRSCR3:
					GDispClrTxt();
				break;
				case DEBUG_MSG1:
					row = buff[2];
					row <<= 8;
					row |= buff[1];
					col = buff[4];
					col <<= 8;
					col |= buff[3];
					str_len = buff[5];
					memset(str,0x20,sizeof(str));	
					memcpy(str,&buff[7],str_len);
					str[str_len] = 0;
				break;
				default:
				break;
			}
			i = 0;
		}
*/
	}
    return (0);		// this should never happen
}


