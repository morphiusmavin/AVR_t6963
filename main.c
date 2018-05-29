//******************************************************************************************//
//*************************************** main.c  ******************************************//
//******************************************************************************************//
// main program that drives the t6963 LCD (32 col x 15 row) display - text only
// see t6963_notes.txt for more details
#include <avr/io.h>
#include<avr/interrupt.h>
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

#define LEN 200
#define DEBUG_CHAR_AT		0
#define DEBUG_STRING_AT		1
#define DEBUG_CHAR			2
#define DEBUG_GOTO			3
#define DEBUG_SET_MODE		4
#define DEBUG_CLRSCR1		5
#define DEBUG_CLRSCR2		6
#define DEBUG_CLRSCR3		7
#define DEBUG_MSG1			8
#define DEBUG_SETPWM		9

#define COLUMN              40      //Set column number to be e.g. 32 for 8x8 fonts, 2 pages
#define ROWS                16
// really cranking
#define TIME_DELAY 2
volatile UCHAR pwm_on;
volatile UCHAR pwm_off;
volatile UCHAR opwm_on;
volatile UCHAR opwm_off;
volatile UCHAR xbyte;
volatile UCHAR high_delay;

volatile int onoff;
volatile int dc2;

ISR(TIMER1_OVF_vect) 
{ 
/*
	if(++dc2 % 1000 == 0)
	{
		if(++xbyte > 0x7e)
		{
			xbyte = 0x21;
		}
		transmitByte(xbyte);
	}
*/
	if(onoff == 1)
	{
		if(--pwm_on < 2)
		{
			pwm_on = opwm_on;
			CLR_PWM();
			onoff = 0;
		}
	}else if(onoff == 0)
	{	
		if(--pwm_off < 2)
		{
			pwm_off = opwm_off;
			SET_PWM();
			onoff = 1;
		}
	}else SET_PWM();
	TCNT1 = (UINT)((high_delay << 8) & 0xFF00);
//	TCNT1 = 0xF800;
}

int main(void)
{
	int i,j,k;
	UCHAR ch;
	UCHAR key;
	UCHAR buff[LEN];
	UCHAR mode, type;
	UINT row, col;
	char str[30];
	UCHAR str_len;
	UCHAR spi_ret;
	UCHAR temp;

	GDispInit();
//	GDispInitPort();
	_delay_ms(10);
    initUSART();
	_delay_ms(20);
//	initSPImaster();
//	initSPIslave();

//#if 0
	GDispSetMode(XOR_MODE);
	_delay_us(10);
	GDispSetMode(TEXT_ON);
	_delay_us(10);
	GDispClrTxt();
	SET_PWM();
	GDispStringAt(7,15,"LCD is on!");

//	initSPImaster();
//******************************************************************************************//
//*********************************** start of main loop ***********************************//
//******************************************************************************************//
	_delay_ms(2000);
//#endif

	xbyte = 0x21;
	opwm_on = pwm_on = 1;
	opwm_off = pwm_off = 6;
	onoff = 2;
	high_delay = 0xF8;
	TCNT1 = 0xFF00;
	TCCR1A = 0x00;
//	TCCR1B = (1<<CS10) | (1<<CS12);;  // Timer mode with 1024 prescler
	TCCR1B = (1<<CS11);
	TIMSK1 = (1 << TOIE1) ;   // Enable timer1 overflow interrupt(TOIE1)
	sei(); // Enable global interrupts by setting global interrupt enable bit in SREG

#if 0
	while(1)
	{
		for(row = 0;row < ROWS;row++)
		{
			for(col = 0;col < COLUMN-1;col++)
			{
/*
				xbyte = receiveByte();
				if(xbyte == 0xFE)
				{
					GDispClrTxt();
					row = col = 0;
					xbyte = 0x21;
				}
*/
//				GDispCharAt(row,col,xbyte);
				pwm_off++;
				pwm_on--;
				if(pwm_off < 1)
				{
					pwm_off = 2;
					pwm_on = 100;
					onoff = 1;
				}
/*
				if(++xbyte > 0x7e)
				{
					xbyte = 0x21;
				}
				transmitByte(xbyte);
*/
				_delay_ms(20);
			}
		}
	}
	_delay_ms(200);
#endif
	GDispClrTxt();

	i = 0;
	dc2 = 0;

	for(row = 0;row < ROWS;row++)
	{
		for(col = 0;col < COLUMN-1;col++)
		{
			GDispCharAt(row,col,xbyte);
			_delay_ms(2);
			if(++xbyte > 0x7e)
			{
				xbyte = 0x21;
			}
		}
	}

	_delay_ms(1000);
	GDispClrTxt();

	row = col = 0;
	
    while (1)
    {
		key = receiveByte();
//		GDispCharAt(0,0,key);
/*
		if(++dc2 % 2 == 0)
			_SB(PORTB,PORTB1);
		else
			_CB(PORTB,PORTB1);
*/
		buff[i] = key;
		i++;
//#if 0
		if(key == 0xfe)
		{
			switch(buff[0])
			{
				case DEBUG_CHAR_AT:
					row = (UINT)buff[1];
					col = (UINT)buff[2];
					ch = buff[3];
					GDispCharAt(row,col,ch);
				break;
				case DEBUG_STRING_AT:
					row = (UINT)buff[1];
					col = (UINT)buff[2];
					str_len = buff[3];

					memset(str,0x20,sizeof(str));	
					memcpy(str,&buff[4],str_len);
					str[str_len] = 0;
					GDispStringAt(row,col,str);
				break;
				case DEBUG_CHAR:
					ch = buff[1];
					GDispChar(ch);
				break;
				case DEBUG_GOTO:
					row = (UINT)buff[1];
					col = (UINT)buff[2];
					GDispGoto(row,col);
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
				case DEBUG_SETPWM:
					opwm_on = pwm_on = buff[1];
					opwm_off = pwm_off = buff[2];
					high_delay = buff[3];
					if(pwm_off == 0)
						onoff = 2;
					else	
						onoff = 1;
				break;
				default:
				break;
			}
			i = 0;
		}
//#endif
	}
    return (0);		// this should never happen
}

#if 0
ISR(SPI_STC_vect)
{
//	loop_until_bit_is_set(SPSR, SPIF);			  /* wait until done */
//	spi_ret = SPDR;
//	transmitByte(spi_ret);
//}
#endif

