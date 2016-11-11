#include <avr/io.h>
#include "../avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include "sfr_helper.h"
#include "uart.h"
#include "t6963.h"

int main(void)
{
    int i,j,x,y,a;
    UCHAR k;

    uartInit(BAUD_PS(19200));

    uartEnableTx();
    uartEnableRx();

    uartwString("Testing LCD...");
	GDispInit();
	GDispSetMode(XOR_MODE);
	GDispSetMode(TEXT_ON);
	GDispClrTxt();	
    k = 0x20;
    DisplayDisclaimer();
    _delay_ms(1000);

    for(a = 0;a < 30;a++)
    {
    uartwChar(0x30 + a);
    uartwChar(0x20);
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
    _delay_ms(20);
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
    _delay_ms(20);
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
//                _delay_ms(1);
            }
            GDispCharAt(0,0,a+0x30);
        }
//        _delay_ms(2);
    }
    }
    GDispClrTxt();
    GDispStringAt(8,6,"Exiting...");
    _delay_ms(2000);
    GDispSetMode(DISPLAY_OFF);
    return 0;

}//end of main()

void DisplayDisclaimer(void)
{
    uartwString("disclaimer\n");
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


