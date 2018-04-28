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
#include "spi.h"
#include "macros.h"
#include <string.h>
//#include "main.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	int i,j,k;
	UCHAR xbyte;
	UCHAR read_byte;
	UINT offset;

//#if 0
	GDispInit();
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

    initUSART();
//	initSPImaster();
//******************************************************************************************//
//*********************************** start of main loop ***********************************//
//******************************************************************************************//
	_delay_ms(3000);
	GDispStringAt(7,15,"          ");
	_delay_us(10);

//	xbyte = 0x30;
	xbyte = 0x21;
	
	i = 0;
	k = 0;
	offset = 0;
		
    while (1)
    {
//		xbyte = pxbyte;
		for(i = 0;i < ROWS;i++)
		{
			for(j = 0;j < COLUMN;j++)
			{
				if(j % 4 == 0)
				{
					CLR_TEST1();
					SET_TEST2();
				}
				GDispCharAt(i,j,xbyte);

				if(++k > 30)
				{
//					transmitByte(0xFE);
					k = 0;
				}

				_delay_us(2);
				xbyte = receiveByte();
				_delay_us(2);

//				if(++xbyte > 0x7e)
//					xbyte = 0x21;
//				_delay_ms(20);

				if(xbyte > 0x1f && xbyte < 0x7e)
					transmitByte(xbyte);
//				_delay_us(10);
//				SPI_write(xbyte);
//				_delay_us(10);


				if(j % 4 == 0)
				{
					SET_TEST1();
					CLR_TEST2();
				}
			}
		}
//		_delay_ms(1000);
/*
		GDispCharAt(ROWS-1,COLUMN-1,0x20);	// put spaces at all 4 corners
		GDispCharAt(0,COLUMN-1,0x20);
		GDispCharAt(0,0,0x20);
		GDispCharAt(ROWS-1,0,0x20);
*/
		GDispClrTxt();
	}
    return (0);		// this should never happen
}

