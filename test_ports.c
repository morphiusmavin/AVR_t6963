#include <avr/io.h>
#include "../avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include "sfr_helper.h"
#include "uart.h"
#include "t6963.h"
#define TIME_DELAY1 1

// available pins:
// PORTD 2->7	(D2->D7)	- PD0 & PD1 are RxD & TxD
// PORTC 0->3	(A0->A3)
// PORTB 0->2	(D8->D10)

#ifndef T6963_H
#define LCD_WR		PC0        // LCD Write control line pin number (A0)
#define LCD_CE		PC1       // LCD Enable control line pin number (A1)
#define LCD_RST		PC2        // LCD RST_ line (A2)
//#define LCD_FS		PORTC,3        // LCD font select (A3)
#define LCD_CD		PC3       // LCD Control/data Select line (A3)
#define LCD_RD		PB2       // LCD Read control line  pin number (D10)

#define DATA0		PD2		// D2
#define DATA1		PD3		// D3
#define DATA2		PD4		// D4
#define DATA3		PD5		// D5
#define DATA4		PD6		// D6
#define DATA5		PD7		// D7

#define DATA6		PB0		// D8
#define DATA7		PB1		// D9
#endif

// these are used by the programmer
// PORTB,2 (D10) is SS
// PORTB,3 (MOSI)
// PORTB,4 (MISO)
// PORTB,5 (SCK)

char version[] = "version 0.3\0";

int main(void)
{
	uint8_t mychar, test1;
    uartInit(BAUD_PS(9600));

    uartEnableTx();
    uartEnableRx();

    uartwString("Tesing ports...\0");
	mychar = 0x21;
#ifndef T6963_H
	DDRC |= 0x0F;	// set all used bits as outputs
	DDRB |= 0x07;
	DDRD |= 0xFC;
#else
	GDispInitPort();
#endif
	test1 = 0;
    while(1)
    {
//        uartwString(uartrString());
//        uartwChar('\r');
//        uartwChar('\n');
		uartwChar(mychar);
		if(++mychar > 0x7e)
			mychar = 0x21;
		_delay_ms(TIME_DELAY1);
#ifndef T6963_H
		_SB(PORTC,LCD_WR);
		_SB(PORTC,LCD_CE);
		_SB(PORTC,LCD_RST);
		_SB(PORTC,LCD_CD);
		_SB(PORTB,LCD_RD);
		_SB(PORTD,DATA0);
		_SB(PORTD,DATA1);
		_SB(PORTD,DATA2);
		_SB(PORTD,DATA3);
		_SB(PORTD,DATA4);
		_SB(PORTD,DATA5);
		_SB(PORTB,DATA6);
		_SB(PORTB,DATA7);
		_delay_ms(TIME_DELAY1);
		_CB(PORTC,LCD_WR);
		_CB(PORTC,LCD_CE);
		_CB(PORTC,LCD_RST);
		_CB(PORTC,LCD_CD);
		_CB(PORTB,LCD_RD);
		_CB(PORTD,DATA0);
		_CB(PORTD,DATA1);
		_CB(PORTD,DATA2);
		_CB(PORTD,DATA3);
		_CB(PORTD,DATA4);
		_CB(PORTD,DATA5);
		_CB(PORTB,DATA6);
		_CB(PORTB,DATA7);
#else
		Data_Out(test1++);
		_delay_ms(TIME_DELAY1);
		SET_WR();
		_delay_ms(TIME_DELAY1);
		SET_CE();
		_delay_ms(TIME_DELAY1);
		SET_RST();
		_delay_ms(TIME_DELAY1);
		SET_CD();
		_delay_ms(TIME_DELAY1);
		SET_RD();
		_delay_ms(TIME_DELAY1);
		CLR_WR();
		_delay_ms(TIME_DELAY1);
		CLR_CE();
		_delay_ms(TIME_DELAY1);
		CLR_RST();
		_delay_ms(TIME_DELAY1);
		CLR_CD();
		_delay_ms(TIME_DELAY1);
		CLR_RD();
#endif
    }
    return 0;
}

