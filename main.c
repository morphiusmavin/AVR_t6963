#include <avr/io.h>
#include "../avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include "sfr_helper.h"
#include <avr/eeprom.h>
#include "USART.h"
#include "t6963.h"
#include "macros.h"
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

char version[] = "version 1.5\0";
#define STRING_LEN   80

// Define EEMEM variables
#if 0
uint8_t eepromCounter EEMEM = 0;
char eepromString[STRING_LEN] EEMEM = "Welcome to the EEMEM Demo.\r\n";
uint16_t eepromWord EEMEM = 12345;
#endif
int main(void)
{
	uint8_t test1;
    int i,j,x,y,a;
    UCHAR k;
    UCHAR flag;

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
//	mychar = 0x21;
//	GDispInit();
//    _delay_us(10);
    initUSART();
#if 0
    _delay_us(10);
    GDispSetMode(XOR_MODE);
    _delay_us(10);
    GDispSetMode(TEXT_ON);
    _delay_us(10);
    GDispClrTxt();    
    _delay_us(10);
    _delay_ms(100);
    printString("LCD is on\r\n");
#endif
    printString("reset - Enter '0' to turn LCD on.\r\n");
    flag = 0;
    while(1)
    {
        test1 = receiveByte();  // just echo back what was sent
		_delay_us(5);
//        transmitByte(test1);
        if(flag == 0 && test1 != '0')
        {
            printString("LCD is off. Enter '0' to turn back on\r\n");
            test1 = 0;
        }
        switch(test1)
        {
            case '1':
                printString("pattern 1\r\n");
                GDispClrTxt();
                k = 0x20;
                for(i = 0;i < 20;i++)
                {
                    _delay_ms(20);
                   for(j = 0;j < 30;j++)
                   {
                      GDispCharAt(i,j,k);
                      if(++k > 0x7e)
                         k = 0x20;
                   }
                }
                break;
            case '2':
                k = 0x20;
                printString("pattern 2\r\n");
                GDispClrTxt();
                for(i = 19;i >= 0;i--)
                {
                    _delay_ms(20);
                   for(j = 29;j >= 0;j--)
                   {
                      GDispCharAt(i,j,k);
//                    _delay_us(20);
                      if(++k > 0x7e)
                         k = 0x20;
                   }
                }
                break;
            case '3':
                k = 0x20;
                printString("pattern 3\r\n");
                GDispClrTxt();
                for(i = 0;i < 10;i++)
                {
                    _delay_ms(20);
                   for(j = 0;j < 15;j++)
                   {
                      GDispCharAt(i,j,k);
                    _delay_us(1);
                      if(++k > 0x7e)
                         k = 0x20;
                   }
                }
                break;
            case 'A':
                printString("pattern 4\r\n");
                GDispClrTxt();
                for(y = 0;y < 16;y++)
                {
                    _delay_ms(20);
                    k = 0x20;
                    for(x = 0;x < 4+y;x++)
                    {
                        k = x+0x20;
                        i = y;
//                        for(j = 0;j< 40;j++)
//                        for(j = 0;j< 20;j++)
                        for(j = 0;j< 10;j++)
                        {
                            GDispCharAt(i,j,k++);
                            _delay_us(20);
                        }
                        GDispCharAt(0,0,a+0x30);
                    }
//                     _delay_us(20);
                }
                break;
            case '4':
                GDispClrTxt();
                GDispStringAt(0,1,"Entered 4");
                transmitByte(test1);
                transmitByte(0x20);
                break;
            case '5':
                GDispClrTxt();
                GDispStringAt(1,1,"Entered 5");
                transmitByte(test1);
                transmitByte(0x20);
                break;
            case '6':
                GDispClrTxt();
                GDispStringAt(2,1,"Entered 6");
                transmitByte(test1);
                transmitByte(0x20);
                break;
            case 'B':
                GDispClrTxt();
                GDispStringAt(3,1,"Entered B");
                transmitByte(test1);
                transmitByte(0x20);
                break;
            case '7':
                GDispClrTxt();
                GDispStringAt(4,1,"Entered 7");
                transmitByte(test1);
                transmitByte(0x20);
                break;
            case '8':
                GDispClrTxt();
                GDispStringAt(5,1,"Entered 8");
                transmitByte(test1);
                transmitByte(0x20);
                break;
            case '9':
                GDispClrTxt();
                GDispStringAt(6,1,"Entered 9");
                transmitByte(test1);
                transmitByte(0x20);
                break;
            case 'C':
                GDispClrTxt();
                GDispStringAt(7,1,"Entered C");
                transmitByte(test1);
                transmitByte(0x20);
                break;
            case '*':
                GDispClrTxt();
                GDispStringAt(8,1,"Ouch! You just");
                GDispStringAt(9,1,"hit the ampersand!");
                transmitByte(test1);
                transmitByte(0x20);
                break;
            case '0':
                if(flag == 0)
                {
                    printString("turning back on..\r\n");
	                GDispInit();
                    _delay_us(10);
                    initUSART();
                    _delay_us(10);
                    GDispSetMode(XOR_MODE);
                    _delay_us(10);
                    GDispSetMode(TEXT_ON);
                    _delay_us(10);
                    GDispClrTxt();    
                    GDispStringAt(1,1,"LCD on!");
                    printString("LCD is on.\r\n");
                    flag = 1;
                }
                else
                    printString("LCD already on!\r\n");
                break;
            case '#':
                GDispClrTxt();
                DisplayDisclaimer();
                break;
                transmitByte(test1);
                printByte(' ');
            case 'D':
                if(flag == 1)
                {
                GDispClrTxt();
                printString("turning off LCD. Enter '0' to turn back on\r\n");
                GDispStringAt(1,1,"Exiting...");
                _delay_ms(500);
                GDispSetMode(DISPLAY_OFF);
                flag = 0;
                }
                break;
            default:
                if(flag == 1)
                    GDispClrTxt();break;
                transmitByte(test1);
                printByte(' ');
        }
    }
    return 0;
}
void DisplayDisclaimer(void)
{
    printString("displaying disclaimer\r\n");
    GDispStringAt(1,15,"CAUTION\0");
    GDispStringAt(3,2,"Use of this system does not replace\0");
    GDispStringAt(4,2,"basic safety precautions and pro-\0");
    GDispStringAt(5,2,"cedures for operating the machine.\0");
    GDispStringAt(6,2,"Do not operate the machine while\0");
    GDispStringAt(7,2,"system communication are being\0");
    GDispStringAt(8,2,"established or diagnostic codes are\0");
    GDispStringAt(9,2,"present. Refer to the Operation and\0");
    GDispStringAt(10,2,"Maintenance Manual of the machine\0");
    GDispStringAt(11,2,"for additional information.\0");
}


