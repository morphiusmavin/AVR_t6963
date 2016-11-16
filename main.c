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
	GDispInit();
    printString("\n\rpast init\n\r");
    GDispSetMode(XOR_MODE);
    printString("past XOR_MODE\n\r");
    GDispSetMode(TEXT_ON);
    printString("past TEXT_ON\n\r");
    GDispClrTxt();    
    printString("past ClrTxt()\n\r");
    initUSART();
    _delay_ms(100);

    while(1)
    {
        test1 = receiveByte();  // just echo back what was sent
		_delay_us(5);
        transmitByte(test1);
        switch(test1)
        {
            case '1':
                for(a = 0;a < 30;a++)
                {
                    transmitByte(0x30 + a);
                    transmitByte(0x20);
                    k = 0x20;
                    _delay_ms(20);
                    for(i = 0;i < 20;i++)
                    {
                       for(j = 0;j < 40;j++)
                       {
                          GDispCharAt(i,j,k);
                          if(++k > 0x7e)
                             k = 0x20;
                       }
                    }
                }
                break;
            case '2':
                k = 0x20;
                for(i = 19;i >= 0;i--)
                {
                   for(j = 39;j >= 0;j--)
                   {
                      GDispCharAt(i,j,k);
                      if(++k > 0x7e)
                         k = 0x20;
                   }
                }
                break;
            case '3':
                k = 0;
                for(i = 0;i < 20;i++)
                {
                   for(j = 0;j < 40;j++)
                   {
                      GDispCharAt(i,j,k);
                      if(++k > 0x7e)
                         k = 0x20;
                   }
                }
                break;
            case 'A':
                for(y = 0;y < 16;y++)
                {
                    k = 0x20;
                    for(x = 0;x < 4+y;x++)
                    {
                        k = x+0x20;
                        i = y;
                        for(j = 0;j< 40;j++)
                        {
                            GDispCharAt(i,j,k++);
                            _delay_ms(1);
                        }
                        GDispCharAt(0,0,a+0x30);
                    }
                     _delay_ms(1);
                }
                break;
            case '4':
                GDispClrTxt();
                break;
            case '5':
                GDispClrTxt();
                break;
            case '6':
                GDispClrTxt();
                break;
            case 'B':
                GDispClrTxt();
                break;
            case '7':
                GDispClrTxt();
                break;
            case '8':
                GDispClrTxt();
                break;
            case '9':
                GDispClrTxt();
                break;
            case 'C':
                GDispClrTxt();
                break;
            case '*':
                GDispClrTxt();
                break;
            case '0':
                GDispClrTxt();
                break;
            case '#':
                GDispClrTxt();
                DisplayDisclaimer();
                break;
            case 'D':
                GDispClrTxt();
                printString("Exiting...\n");
                GDispStringAt(8,6,"Exiting...");
                _delay_ms(2000);
                GDispSetMode(DISPLAY_OFF);
                break;
            default:GDispClrTxt();break;
        }
    }
    return 0;
}
void DisplayDisclaimer(void)
{
    printString("disclaimer\n");
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
