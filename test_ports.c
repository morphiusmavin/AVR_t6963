#include <avr/io.h>
#include "../avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include "sfr_helper.h"
#include <avr/eeprom.h>
#include "USART.h"
#include "t6963.h"
#define TIME_DELAY1 1000

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

char version[] = "version 1.3\0";
#define STRING_LEN   80

// Define EEMEM variables
#if 0
uint8_t eepromCounter EEMEM = 0;
char eepromString[STRING_LEN] EEMEM = "Welcome to the EEMEM Demo.\r\n";
uint16_t eepromWord EEMEM = 12345;
#endif
int main(void)
{
	uint8_t mychar, test1, test2;
    int i;
//	initUSART();
#if 0    
	char ramString[STRING_LEN];
	uint8_t counter;

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
#endif
	mychar = 0x21;
#ifndef T6963_H
#warning "t6963 not defined"
	DDRC |= 0x0F;	// set all used bits as outputs
	DDRB |= 0x07;
	DDRD |= 0xFC;
#else
#warning "t6963 defined"
	GDispInitPort();
#endif
    initUSART();
    _delay_ms(100);
//    printString("\n\rTesting...\n\r");
	test1 = 0x70;
    i = 0;
    transmitByte(test1);    // start the ball rolling (testAVR.c waits for a char
                            // before sending it back
    _delay_ms(TIME_DELAY1);
    transmitByte(0x23);
    while(1)
    {
		if(++mychar > 0x7e)
			mychar = 0x21;
		_delay_us(5);
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
		_delay_us(TIME_DELAY1);
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
        test1 = receiveByte();  // just echo back what was sent
        _delay_ms(TIME_DELAY1);
        transmitByte(test1);
        transmitByte(test2);
/*
        _SB(PORTD,DATA0);
        _delay_ms(2);
        _CB(PORTD,DATA0);
*/
		Data_Out(test2++);

		_delay_us(TIME_DELAY1);
		SET_WR();
		_delay_us(TIME_DELAY1);
		SET_CE();
		_delay_us(TIME_DELAY1);
		SET_RST();
		_delay_us(TIME_DELAY1);
		SET_CD();
		_delay_us(TIME_DELAY1);
		SET_RD();
		_delay_us(TIME_DELAY1);
		CLR_WR();
		_delay_us(TIME_DELAY1);
		CLR_CE();
		_delay_us(TIME_DELAY1);
		CLR_RST();
		_delay_us(TIME_DELAY1);
		CLR_CD();
		_delay_us(TIME_DELAY1);
		CLR_RD();

#endif
    }
    return 0;
}

