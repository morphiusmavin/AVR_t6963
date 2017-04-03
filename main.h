// main.h
#ifndef _MAIN_H_
#define _MAIN_H_
#define TIME_DELAY1 1
#define STRING_LEN   100
#define NUM_FPTS 17
#warning "main.h"
typedef struct prompts
{
#ifdef NOAVR
	char label[12];
#endif
	UCHAR pnum;
	UCHAR row;
	UCHAR col;
    UINT offset;
	int len;
    UCHAR type;
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
	MAIN_MENU,	// 0
	MENU1A,		// 1
	MENU1B,		// 2
	MENU1C,		// 3
	MENU1D,		// 4
	MENU2A,		// 5
	MENU2B,		// 6
	MENU2C,		// 7
	MENU3A,		// 8
	MENU3B,		// 9
	MENU3C,		// 10
	MENU4A,		// 11
	MENU4B,		// 12
	MENU4C,		// 13
	NUM_ENTRY,	// 14
	ALNUM_ENTRY	// 15
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
	RT_AUX = 0xF5,
	RT_TRIP,
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

#define NUM_ENTRY_SIZE 20
//#define NUM_ENTRY_BEGIN_COL (COLUMN - COLUMN/2)
#define NUM_ENTRY_BEGIN_COL 3
#define NUM_ENTRY_END_COL NUM_ENTRY_BEGIN_COL + NUM_ENTRY_SIZE
#define NUM_ENTRY_ROW 6

// '!' - '9' (33 - 58) + 'A' - 'Z' (26) + 'a' - 'z' (26) = 77
//#define NUM_ALNUM 77
//#define NUM_ALNUM 52		// without the '!' - '9'
#define NUM_ALNUM 85		// include all special chars
#define ALNUM_SCROLL_LIST_LEN 13
#define MENU_START_ROW 12
#define MENU_START_COL 15
#define MENU_BLANK "          "

#define RTMAINC rt_main[curr_rt_layout]

#define NO_PROMPTS_EEPROM_LOCATION 0x03f0
#define PROMPT_INFO_OFFSET_EEPROM_LOCATION_LSB 0x03f2	// points to just after all the labels (prompt_info)
#define PROMPT_INFO_OFFSET_EEPROM_LOCATION_MSB 0x03f3
#define dispCharAt(_row,_col,_char) GDispCharAt((UINT)_row,(UINT)_col,(UCHAR)_char)
#define dispSetCursor(_mode,_row,_col,_type) GDispSetCursor ((UCHAR)_mode, (UINT)_row, (UINT)_col, (UCHAR)_type)

void dispRC(int row, int col);
void CheckRC(int *row, int *col, UCHAR *k);
void display_labels(void);
void parse_PIC24(UCHAR);
void set_defaults(void);

void init_list(void);
UCHAR get_key(UCHAR ch, UCHAR limit8, UINT limit16);
int get_curr_fptr(void);
int curr_fptr_changed(void);
int get_curr_menu(void);
int get_type(void);

UCHAR current_param;
UINT temp_UINT;
UCHAR parse_state;
UCHAR cursor_row, cursor_col;

//PROMPT_STRUCT prompts[30];
UCHAR no_prompts;

char cur_global_number[NUM_ENTRY_SIZE];
char new_global_number[NUM_ENTRY_SIZE];
#endif
