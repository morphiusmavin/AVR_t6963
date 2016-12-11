#ifndef _MAIN_H_
#define _MAIN_H_
#define TIME_DELAY1 1
#define RT_LABEL 0
#define STRING_LEN   255
#define NUM_FPTS 10

typedef struct prompts
{
	uint8_t pnum;
	UCHAR row;
	UCHAR col;
    uint16_t offset;
	int len;
    uint8_t type;
} PROMPT_STRUCT;

typedef struct RT_layout
{
	UCHAR row;		// row, col where this label goes (param is end of label + 1 space)
	UCHAR col;
	int label_list;		// index into list of labels (malloc_ptr)
} RT_LAYOUT;

typedef struct RT_main
{
	char name[10];	// name of layout style
	int num_params;		// 
	RT_LAYOUT *ptr_rt_layout;	// pointer to list of type RT_LAYOUT's (this is num_params long)
	uint16_t offset;		// used by malloc after read from eeprom
} RT_MAIN;


enum menu_types
{
	MENU1 = 1,
	MENU2,
	MENU3,
	MENU4
} MENU_TYPES;	

enum rt_types
{
	RT_HIGH2 = 0xF3,
	RT_HIGH1,
	RT_HIGH0,
	RT_TRIP,
	RT_TIME,
	RT_AIRT,
	RT_OS,
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
	SEND_UCHAR,
	SEND_UINT1,
	SEND_UINT2
} STATES;	

#define NUM_ENTRY_SIZE 7
#define NUM_ENTRY_BEGIN_COL (COLUMN - COLUMN/3)
#define NUM_ENTRY_END_COL NUM_ENTRY_BEGIN_COL + NUM_ENTRY_SIZE
#define NUM_ENTRY_ROW 13

#define RTMAINC rt_main[curr_rt_layout]

#define NO_PROMPTS_EEPROM_LOCATION 0x03f0
#define PROMPT_INFO_OFFSET_EEPROM_LOCATION_LSB 0x03f2	// points to just after all the labels (prompt_info)
#define PROMPT_INFO_OFFSET_EEPROM_LOCATION_MSB 0x03f3
#define NO_LAYOUTS_EEPROM_LOCATION 0x03f4
#define LAYOUT_OFFSET_EEPROM_LOCATION_LSB 0x03f6	// points to just after all the prompt info (layout info)
#define LAYOUT_OFFSET_EEPROM_LOCATION_MSB 0x03f7
#define dispCharAt(_row,_col,_char) GDispCharAt((uint16_t)_row,(uint16_t)_col,(UCHAR)_char)
#define dispSetCursor(_mode,_row,_col,_type) GDispSetCursor ((UCHAR)_mode, (uint16_t)_row, (uint16_t)_col, (UCHAR)_type)

RT_MAIN *rt_main;
RT_LAYOUT *rt_tlayout;
//char *labels;
char labels[200];

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
UCHAR menu2d(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col);
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
UCHAR last_char;
UCHAR parse_state;

//PROMPT_STRUCT prompts[30];
uint16_t prompt_info_offset;
uint8_t hide_menu;
uint8_t no_layouts;
uint8_t no_prompts;
int curr_rt_layout;
int sequence_counter;
int current_fptr;
uint8_t test;

char cur_global_number[NUM_ENTRY_SIZE];
char new_global_number[NUM_ENTRY_SIZE];
UCHAR cur_row, cur_col;	// used by the current menu/dialog function to keep track of the current row,col
PROMPT_STRUCT *prompt_ptr;
#endif
