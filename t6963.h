/*
***********************************************************************************************
*                           Toshiba T6963C Graphical LCD Display Driver
*
* File name     : t6963.h
* Programmer    : John Leung, TechToys, Hong Kong
* Web presence  : www.TechToys.com.hk
* Note          : References - picant complier web page, epe magazine Feb 2001
*                 & Jean J Labrosse's Embedded Systems Building Blocks
*                 & treitmey's driver on CCS forum
* Language      : CCS C complier for PIC mid-range MCU, PCM version 3.170, under MPLAB IDE 7.01
* Hardware      : Prototype board PCB ver 2.5(home-grown), EQS Console, MCU 16F877a, 20MHz Xtal
* Date          : Version 0.0 (20 April 2005 - 27 April 2005)
* Date          : 3 May 2005
*               : Routine void GDispPixFontAt(x, y, textptr, size, color) and font5x7.h added
***********************************************************************************************
*                                       DESCRIPTION
*
* This module provides an interface to Toshiba T6963C-0101 Graphical LCD of size 128x64 dots
* A 128-word character generator ROM (code 0101) T6963C-0101 is built in as standard.
* Graphics l.c.d. pinout function
* pin 1 FG  frame ground
* pin 2 GND signal ground
* pin 3 +5V Positive supply
* pin 4 CX  Negative supply (-3.5V approx)
* pin 5 WR  Data write (active low)
* pin 6 RD  Data read (active low)
* pin 7 CE  Chip enable (active low)
* pin 8 CD  CD=1, WR=0: command write
*           CD=1, WR=1: command read
*           CD=0, WR=0: data write
*           CD=0, WR=1: data read
* pin 9 RST Module reset (active low)
* pin 10 - 17   Data bus
* pin 18 FS Font select: FS=0(8x8 font), FS=1(or open circuit, 6x8 font)
* In EQS console ver 2.5 PCB, FS is determined by JP23 jumper. FS=0 when shorted
*
*
***********************************************************************************************
*/

/*
***********************************************************************************************
*                                       PORT DEFINITION
***********************************************************************************************
*/
//I/O port for data definition
#ifndef T6963_H
#define T6963_H
#if 1
//Control pin setting
#define NEW_PIN		// swap B2 with C4 because B2 doubles as SS and I wanted
					// to use the SPI port - C4 is 'snuck in there' behind PC2 & 3
					// on the middle right with the reset button on the bottom
#ifdef NEW_PIN
#warning "NEW_PIN defined"
#define TEST_PIN1	PORTB2
#define LCD_RD		PORTC4       // LCD Read control line  pin number (D10)
#else
#define TEST_PIN1	PORTC4
#define LCD_RD		PORTB2       // LCD Read control line  pin number (D10)
#endif

#define PWM_PIN		PORTC5			// used for dimming display
#define LCD_WR		PORTC0        // LCD Write control line pin number (A0)
#define LCD_CE		PORTC1       // LCD Enable control line pin number (A1)
#define LCD_RST		PORTC2        // LCD RST_ line (A2)
//#define LCD_FS		PORTC,3        // LCD font select (this one just tied low)
#define LCD_CD		PORTC3       // LCD Control/data Select line (A3)

#define DATA0		PORTD2		// D2
#define DATA1		PORTD3		// D3
#define DATA2		PORTD4		// D4
#define DATA3		PORTD5		// D5
#define DATA4		PORTD6		// D6
#define DATA5		PORTD7		// D7
#define DATA6		PORTB0		// D8
#define DATA7		PORTB1		// D9

#ifdef NEW_PIN
#define SET_RD()	_SB(PORTC,LCD_RD)
#define CLR_RD()	_CB(PORTC,LCD_RD)
#define SET_TEST1()	_SB(PORTB,TEST_PIN1)
#define CLR_TEST1()	_CB(PORTB,TEST_PIN1)
#else
#define SET_RD()	_SB(PORTB,LCD_RD)
#define CLR_RD()	_CB(PORTB,LCD_RD)
#define SET_TEST1()	_SB(PORTC,TEST_PIN1)
#define CLR_TEST1()	_CB(PORTC,TEST_PIN1)
#endif

#define SET_PWM()	_SB(PORTC,PWM_PIN)
#define CLR_PWM()	_CB(PORTC,PWM_PIN)

#define SET_WR()	_SB(PORTC,LCD_WR)
#define SET_CE()	_SB(PORTC,LCD_CE)
#define SET_RST()	_SB(PORTC,LCD_RST)
#define SET_CD()	_SB(PORTC,LCD_CD)

#define CLR_WR()	_CB(PORTC,LCD_WR)
#define CLR_CE()	_CB(PORTC,LCD_CE)
#define CLR_RST()	_CB(PORTC,LCD_RST)
#define CLR_CD()	_CB(PORTC,LCD_CD)

#define SET_DATA_DIR_OUT()	DDRB |= 0x03;	\
							DDRD |= 0xFC;

#define SET_DATA_DIR_IN()	DDRB &= 0xFC;	\
							DDRD &= 0x03;

#endif

/*
***********************************************************************************************
*                                       GLOBAL CONSTANTS
***********************************************************************************************
*/

/*
***********************************************************************************************
* Note      :   The T6963 controller's RAM address space from $0000 - $FFFF, total 64kbyte RAM.
*               It is just the RAM address set, not the actual RAM amount can be set!
*               However, everything manufacturer may mount different amount of RAM on its
*               module. Example, Powertip's PG12864-F has 4 kbyte RAM only.
*               RAM space to handle 128x64 dots in 8 font = 128x64/8 = 1,024 byte.
*               Thus, the LCD's actual visible screen area is 1 kbyte.
*               For 8-font, 16 COLUMN,
*               TEXT_HOME_ADDR  starts 0x0000 ending 0x01FF -> 512  bytes (for text use)?
*               GRH_HOME_ADDR   starts 0x0200 ending 0x05FF -> 1,024 bytes ?
*               CG_HOME_ADDR    starts 0x1400 ending 0x147F -> 128  bytes (custom characters) ?
***********************************************************************************************
*/
#if 1
#define TEXT_HOME_ADDR      0x0000
#define GRH_HOME_ADDR       0x0200
#define CG_HOME_ADDR        0x1400
#define COLUMN              40      //Set column number to be e.g. 40 for 6x8 fonts, 2 pages
#define ROWS                16
#define MAX_ROW_PIXEL       128      //MAX_ROW_PIXEL the physical matrix length (y direction)
#define MAX_COL_PIXEL       128     //MAX_COL_PIXEL the physical matrix width (x direction)
#define ENABLE              1
#define DISABLE             0
#define BLACK               1
#define WHITE               0
#endif
/*

***********************************************************************************************
*                                       DISPLAY MODE
***********************************************************************************************
*/
#if 1
#define DISPLAY_OFF         0x90    //0b10010000, display off
#define CURSOR_ON_BLINK_OFF 0x92    //0b1001xx10, cursor on without blinking
#define CURSOR_BLINK_ON     0x93    //0b1001xx11, cursor on with blinking
#define TEXT_ON             0x94    //0b100101xx, text on, graphics off
#define GRAPHIC_ON          0x98    //0b100110xx, text off, graphics on
#define TEXT_GRH_ON         0x9C    //0b100111xx, text on, graphics on
#endif

/*
***********************************************************************************************
*                                           MODE
***********************************************************************************************
*   Example:    OR_MODE|EXT_CG_MODE     ->  OR mode, external CG ram mode
*               AND_MODE                ->  AND mode, internal CG ROM mode
***********************************************************************************************
*/
/*
***********************************************************************************************
*       In internal CG ROM MODE (INT_CG_MODE), character codes 00h - 7Fh represent the
*       pre-defined "internal" CG ROM characters, and codes 80h - FFh represent the
*       users' own "external" characters. In external CG ROM MODE (EXT_CG_MODE),
*       all 256 codes from 00h - FFh can be used to represent the users' own characters.
***********************************************************************************************
*/
#if 1
#define INT_CG_MODE         0x80    //0b10000xxx, Internal CG ROM mode
#define EXT_CG_MODE         0x88    //0b10001xxx, External CG RAM mode
#define OR_MODE             0x80    //0b1000x000, OR mode, internal CG ROM mODE
#define XOR_MODE            0x81    //0b1000x001, XOR mode, internal CG ROM mode
#define AND_MODE            0x83    //0b1000x011, AND mode, internal CG ROM mode
#define TEXT_ATTR_MODE      0x84    //0b1000x100, text attribute mode, internal CG ROM mode
#endif
/*

***********************************************************************************************
*                                       CURSOR MODE
***********************************************************************************************
*/
#if 1
#define LINE_8_CURSOR       0xA7    //8-line cursor mode
#define LINE_7_CURSOR       0xA6    //7-line cursor mode
#define LINE_6_CURSOR       0xA5    //6-line cursor mode
#define LINE_5_CURSOR       0xA4    //5-line cursor mode
#define LINE_4_CURSOR       0xA3    //4-line cursor mode
#define LINE_3_CURSOR       0xA2    //3-Line cursor mode
#define LINE_2_CURSOR       0xA1    //2-Line cursor mode
#define LINE_1_CURSOR       0xA0    //1-Line cursor mode
#endif
/*
***********************************************************************************************
*                                   TEXT ATTRIBUTE MODE
***********************************************************************************************
*/
#if 1
#define ATTR_NORMAL         0x00    //Normal Display
#define ATTR_REVERSE        0x05    //Reverse Di../t6963.h:189:33: error: expected ';', ',' or ')' before 'mode'splay
#define ATTR_INHIBIT        0x03    //Inhibit Display
#define ATTR_BLINK          0x08    //Blinking of Normal Display
#define ATTR_BLINK_REVERSE  0x0D    //Blinking of Reverse Display
#define ATTR_BLINK_INHIBIT  0x0B    //Blinking of Inhibit Display
#endif

/*
***********************************************************************************************
*                                     GLOBAL VARIABLES
***********************************************************************************************
*/
#if 1
extern UCHAR    DisplayMode;    /* Keeps state of the display mode (DISPLAY MODE)   */
extern UCHAR    Mode;           /* Keeps state of the MODE                          */
//extern UCHAR    FontSize;       /* 6 font or 8 font                                 */
extern UCHAR    CGBuffer[8];    /* Buffer for custom pattern                        */
#endif
/*
***********************************************************************************************
*                                   FUNCTION PROTOTYPES
***********************************************************************************************
*/
/*
***********************************************************************************************
*                                   FUNCTION PROTOTYPES
*                                    HARDWARE SPECIFIC
***********************************************************************************************
*/
#if 1
#define GDispChipEn     _CB(PORTC,LCD_CE)           /* Low level Chip Enable macro          */
#define GDispChipDi     _SB(PORTC,LCD_CE)           /* Low level Chip Disable macro         */

#define GDispCmdWr()	SET_DATA_DIR_OUT();		\
						SET_RD(); 	\
						SET_CD(); 	\
						CLR_WR();

#define GDispCmdRd()	SET_DATA_DIR_IN();  \
						CLR_RD(); \
						SET_CD(); \
						SET_WR();

#define GDispDatWr()	SET_DATA_DIR_OUT(); \
						SET_RD(); \
						CLR_CD(); \
						CLR_WR();

#define GDispDatRd()	SET_DATA_DIR_IN();	\
						CLR_CD();	\
						SET_WR();	\
						CLR_RD();
#endif
void GDispInit(void);
void GDispSetMode(UCHAR mode);
void GDispClrTxt(void);
void GDispGoto(UINT row, UINT col);
void GDispChar(UCHAR c);
void GDispCharAt(UINT row, UINT col, UCHAR c);
void GDispSetCursor(UCHAR mode, UINT row, UINT col, UCHAR type);
void GDispSetAttrb(UINT row, UINT col, UCHAR mode);
void GDispCGCharAt(UINT row, UINT col, UCHAR id);
//void GDispDatRd(void);
//void GDispDatWr(void);
//void GDispCmdWr(void);
void GDispStringAt(UINT row, UINT col, char *c);
void GDispInitPort(void);
void GDispBusyChk(void);
void GDispAutoWrChk(void);
void GDispDataWr(UCHAR data);
void GDispDataWr(UCHAR data);
void GDispAutoDataWr(UCHAR data);
void GDispCmdSend(UCHAR cmd);
void GDispPeekChk (void);
UCHAR GDispScreenPeek(UINT row, UINT col);
void Data_Out(UCHAR data);
UCHAR Data_In(void);
#endif
