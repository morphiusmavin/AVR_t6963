#ifndef _MAIN_H_
#define _MAIN_H_
#define TIME_DELAY1 1
#define STRING_LEN   100
#define NUM_FPTS 15
#warning "main.h"
typedef unsigned char UCHAR;
typedef unsigned int UINT;
typedef unsigned char uint8_t;
#ifndef UINT16_T
#warning "UINT16_T not defined"
typedef unsigned int uint16_t;
#else
#warning "UINT16_T is defined"
#endif
typedef struct prompts
{
	uint8_t pnum;
	UCHAR row;
	UCHAR col;
    uint16_t offset;
	int len;
    uint8_t type;
} PROMPT_STRUCT;

enum menu_types
{
	MENU1,
	MENU2,
	MENU3,
	MENU4,
	MENU5,
	RT_LABEL
} MENU_TYPES;	

enum menu_order
{
	MAIN_MENU,
	MENU1A,
	MENU1B,
	MENU1C,
	MENU1D,
	MENU2A,
	MENU2B,
	MENU2C,
	MENU3A,
	MENU3B,
	MENU3C,
	MENU4A,
	MENU4B,
	MENU4C,
	NUM_ENTRY
} MENU_ORDER;

enum data_types
{
	RT_LOW,			// UCHAR without high bit set
	RT_HIGH0,			// UCHAR with bit 7 set
	RT_HIGH1,			// UINT with neither 7 or 15 set
	RT_HIGH2,			// bit 7 of UINT set
	RT_HIGH3			// bit 15 of UINT set
} DATA_TYPES;

enum rt_types
{
	RT_TRIP = 0xF6,
	RT_TIME,
	RT_AIRT,
	RT_O2,
	RT_MAP,
	RT_OILT,
	RT_OILP,
	RT_ENGT,
	RT_MPH,
	RT_RPM	// this should be 0xFF	
} RT_TYPES;

enum key_types
{
	KP_POUND = 0xE0, // '#'
	KP_AST, // '*'
	KP_0, // '0'
	KP_1, // '1'
	KP_2, // '2'
	KP_3, // '3'
	KP_4, // '4'
	KP_5, // '5'
	KP_6, // '6'
	KP_7, // '7'
	KP_8, // '8'
	KP_9, // '9'
	KP_A, // 'A'
	KP_B, // 'B'
	KP_C, // 'C'
	KP_D // 'D'
} KEY_TYPES;


#define MSG_0 0xD0	// message types sent from AVR to PIC24
#define MSG_1 0xD1
#define MSG_2 0xD2
#define MSG_3 0xD3

enum states
{
	IDLE,
	CHECK_HIGHBIT,
	SEND_UCHAR0,	// UCHAR without high bit set	
	SEND_UCHAR1,	// UCHAR with high bit set
	GET_CH0,
	GET_CH1,
	GET_CH2,
	SEND_UINT0,		// UINT with neither bit 7 or 15 set
	SEND_UINT1,		// UINT with bit 7 set
	SEND_UINT2		// UINT with bit 15 set
} STATES;	

#define NUM_ENTRY_SIZE 7
#define NUM_ENTRY_BEGIN_COL (COLUMN - COLUMN/3)
#define NUM_ENTRY_END_COL NUM_ENTRY_BEGIN_COL + NUM_ENTRY_SIZE
#define NUM_ENTRY_ROW 13

#define RTMAINC rt_main[curr_rt_layout]

#define NO_PROMPTS_EEPROM_LOCATION 0x03f0
#define PROMPT_INFO_OFFSET_EEPROM_LOCATION_LSB 0x03f2	// points to just after all the labels (prompt_info)
#define PROMPT_INFO_OFFSET_EEPROM_LOCATION_MSB 0x03f3
#define dispCharAt(_row,_col,_char) GDispCharAt((uint16_t)_row,(uint16_t)_col,(UCHAR)_char)
#define dispSetCursor(_mode,_row,_col,_type) GDispSetCursor ((UCHAR)_mode, (uint16_t)_row, (uint16_t)_col, (UCHAR)_type)

void dispRC(int row, int col);
void CheckRC(int *row, int *col, UCHAR *k);
void display_labels(void);
UCHAR main_menu_func(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col);
UCHAR menu1a(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col);
UCHAR menu1b(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col);
UCHAR menu1c(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col);
UCHAR menu1d(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col);
UCHAR menu2a(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col);
UCHAR menu2b(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col);
UCHAR menu2c(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col);
UCHAR menu3a(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col);
UCHAR menu3b(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col);
UCHAR menu3c(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col);
UCHAR menu4a(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col);
UCHAR menu4b(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col);
UCHAR menu4c(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col);
UCHAR number_entry(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col);
void cursor_forward(void);
void cursor_backward(void);
void cursor_forward_stuff(char);
void stuff_num(char);
void parse_PIC24(UCHAR);
void display_menus(void);
void set_defaults(void);

UCHAR current_param;
uint16_t temp_UINT;
UCHAR parse_state;
UCHAR last_fptr;

//PROMPT_STRUCT prompts[30];
uint8_t no_prompts;
int current_fptr;

char cur_global_number[NUM_ENTRY_SIZE];
char new_global_number[NUM_ENTRY_SIZE];
UCHAR cur_row, cur_col;	// used by the current menu/dialog function to keep track of the current row,col
#endif
