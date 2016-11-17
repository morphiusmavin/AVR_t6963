#include <avr/io.h>
#include "../avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include "sfr_helper.h"
#include <avr/eeprom.h>
#include "USART.h"
#include "t6963.h"
#include "macros.h"
#include <string.h>
#define TIME_DELAY1 1

#define dispCharAt(_row,_col,_char) GDispCharAt((uint16_t)_row,(uint16_t)_col,(UCHAR)_char)
#define dispSetCursor(_mode,_row,_col,_type) GDispSetCursor ((UCHAR)_mode, (uint16_t)_row, (uint16_t)_col, (UCHAR)_type)
void dispRC(int row, int col);
void CheckRC(int *row, int *col, UCHAR *k);

// available pins:
// PORTD 2->7	(D2->D7)	- PD0 & PD1 are RxD & TxD
// PORTC 0->3	(A0->A3)
// PORTB 0->2	(D8->D10)

// these are used by the programmer
// PORTB,2 (D10) is SS
// PORTB,3 (MOSI)
// PORTB,4 (MISO)
// PORTB,5 (SCK)

char version[] = "version 1.5\0";
#define STRING_LEN   100
#define ROWS 16
// Define EEMEM variables

uint8_t eepromCounter EEMEM = 0;
char eepromString[STRING_LEN] EEMEM = "Welcome to the EEMEM Demo.\r\n";
uint16_t eepromWord EEMEM = 12345;

int main(void)
{
	uint8_t test1;
    int row,col;
    UCHAR k;
    UCHAR flag;
    int total_strlen;
    int ctr_arr[12];
    int i,j;

	char ramString[STRING_LEN];
	uint8_t counter;

    initUSART();
    printString("reset - Enter '0' to turn LCD on.\r\n");
    flag = 0;
    k = 0x41;
    row = 0;
    col = 0;
    while(1)
    {
        test1 = receiveByte();  // just echo back what was sent
        if(flag == 0 && test1 != '0')
        {
            printString("LCD is off. Enter '0' to turn back on\r\n");
            test1 = 0;
        }

        switch(test1)
        {
            case 'B':
                GDispClrTxt();
                k = 0x41;
                row = 0;
                col = 0;
                dispSetCursor(TEXT_ON | CURSOR_BLINK_ON,row,col,LINE_8_CURSOR);
                dispCharAt(row,col,k);
                dispRC(row,col);
                printString("init cursor position\r\n");
                break;
            case '7':
                col++;
                k++;
                CheckRC(&row,&col,&k);
                dispSetCursor(TEXT_ON | CURSOR_BLINK_ON,row,col,LINE_8_CURSOR);
                dispCharAt(row,col,k);
                dispRC(row,col);
                break;
            case '8':
                row++;
                k++;
                CheckRC(&row,&col,&k);
                dispSetCursor(TEXT_ON | CURSOR_BLINK_ON,row,col,LINE_8_CURSOR);
                dispCharAt(row,col,k);
                dispRC(row,col);
                break;
            case '9':
                row--;
                k++;
                CheckRC(&row,&col,&k);
                dispSetCursor(TEXT_ON | CURSOR_BLINK_ON,row,col,LINE_8_CURSOR);
                dispCharAt(row,col,k);
                dispRC(row,col);
                break;
            case 'C':
                col--;
                k++;
                CheckRC(&row,&col,&k);
                dispSetCursor(TEXT_ON | CURSOR_BLINK_ON,row,col,LINE_8_CURSOR);
                dispCharAt(row,col,k);
                dispRC(row,col);
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
            case '1':
                printString("writing to eeprom...\r\n");
                total_strlen = 0;
                i = 0;
                ctr_arr[i++] = total_strlen;
                strcpy(ramString,"CAUTION\0");
	            eeprom_update_block(ramString, eepromString+total_strlen, strlen(ramString)+1);
                total_strlen += strlen(ramString)+1;
                ctr_arr[i++] = total_strlen;
                strcpy(ramString,"Use of this system does not replace\0");
	            eeprom_update_block(ramString, eepromString+total_strlen, strlen(ramString)+1);
                total_strlen += strlen(ramString)+1;
                ctr_arr[i++] = total_strlen;
                strcpy(ramString,"basic safety precautions and pro-\0");
	            eeprom_update_block(ramString, eepromString+total_strlen, strlen(ramString)+1);
                total_strlen += strlen(ramString)+1;
                ctr_arr[i++] = total_strlen;
                strcpy(ramString,"cedures for operating the machine.\0");
	            eeprom_update_block(ramString, eepromString+total_strlen, strlen(ramString)+1);
                total_strlen += strlen(ramString)+1;
                ctr_arr[i++] = total_strlen;
                strcpy(ramString,"Do not operate the machine while\0");
	            eeprom_update_block(ramString, eepromString+total_strlen, strlen(ramString)+1);
                total_strlen += strlen(ramString)+1;
                ctr_arr[i++] = total_strlen;
                strcpy(ramString,"system communication are being\0");
	            eeprom_update_block(ramString, eepromString+total_strlen, strlen(ramString)+1);
                total_strlen += strlen(ramString)+1;
                ctr_arr[i++] = total_strlen;
                strcpy(ramString,"established or diagnostic codes are\0");
	            eeprom_update_block(ramString, eepromString+total_strlen, strlen(ramString)+1);
                total_strlen += strlen(ramString)+1;
                ctr_arr[i++] = total_strlen;
                strcpy(ramString,"present. Refer to the Operation and\0");
	            eeprom_update_block(ramString, eepromString+total_strlen, strlen(ramString)+1);
                total_strlen += strlen(ramString)+1;
                ctr_arr[i++] = total_strlen;
                strcpy(ramString,"Maintenance Manual of the machine\0");
	            eeprom_update_block(ramString, eepromString+total_strlen, strlen(ramString)+1);
                total_strlen += strlen(ramString)+1;
                ctr_arr[i++] = total_strlen;
                strcpy(ramString,"for additional information.\0");
	            eeprom_update_block(ramString, eepromString+total_strlen, strlen(ramString)+1);
                total_strlen += strlen(ramString)+1;
                ctr_arr[i++] = total_strlen;
                j = i;
/*
                for(i=0;i < j;i++)
                {
                    printByte((uint8_t)ctr_arr[i]);
                    printString("\r\n");
                }
*/
                printString("done\r\n");
                break;
            case '2':
                printString("reading eeprom\r\n");
                for(i = 0;i < j-1;i++)
                {
	                eeprom_read_block(ramString, eepromString+ctr_arr[i], STRING_LEN);
	                printString(ramString);
                    printString("\r\n");
                }
                break;
            case '3':
                k = 0x20;
                printString("pattern 3\r\n");
                GDispClrTxt();
                for(row = 0;row < ROWS;row++)
                {
                    _delay_ms(20);
                   for(col = 0;col < COLUMN;col++)
                   {
                      dispCharAt(row,col,k);
                    _delay_us(20);
                      if(++k > 0x7e)
                         k = 0x20;
                   }
                }
                break;
            case 'A':
                printString("\r\n");
                break;
            case '4':
                printString("pattern 1 fast\r\n");
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
                break;
            case '5':
                k = 0x20;
                printString("pattern 2 fast\r\n");
                GDispClrTxt();
                for(row = ROWS;row >= 0;row--)
                {
                   for(col = COLUMN;col >= 0;col--)
                   {
                      dispCharAt(row,col,k);
                      if(++k > 0x7e)
                         k = 0x20;
                   }
                }
                break;
            case '6':
                GDispClrTxt();
                DisplayDisclaimer2();
                break;
/*
                k = 0x20;
                printString("pattern 3 fast\r\n");
                GDispClrTxt();
                for(row = 0;row < ROWS;row++)
                {
                   for(col = 0;col < COLUMN;col++)
                   {
                      dispCharAt(row,col,k);
                      if(++k > 0x7e)
                         k = 0x20;
                   }
                }
                break;
*/
            case '0':
                if(flag == 0)
                {
                    printString("turning back on..\r\n");
	                GDispInit();
                    _delay_us(10);
                    initUSART();
                    _delay_us(10);
                    GDispSetMode(XOR_MODE);
//                    GDispSetMode(OR_MODE);
                    _delay_us(10);
                    GDispSetMode(TEXT_ON);
//                    GDispSetMode(TEXT_GRH_ON);
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
                DisplayDisclaimer(j,ctr_arr);
                break;
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

void DisplayDisclaimer(int arr_len, int *ctr_arr)
{
    int i;
	char ramString[STRING_LEN];
    printString("displaying disclaimer\r\n");
    GDispClrTxt();
    for(i = 0;i < arr_len-1;i++)
    {
        eeprom_read_block(ramString, eepromString+ctr_arr[i], STRING_LEN+1);
        GDispStringAt(i,0,ramString);
        printString(ramString);
        printString("\r\n");
    }
}

void DisplayDisclaimer2(void)
{
    GDispClrTxt();
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

