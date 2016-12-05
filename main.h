#ifndef _MAIN_H_
#define _MAIN_H_
#define TIME_DELAY1 1
#define RT_LABEL 0x00
#define MENU1 0x01
#define MENU2 0x02
#define MENU3 0x03
#define STRING_LEN   255
#define NUM_FPTS 10

#define RT_RPM 0xFF
#define RT_MPH 0xFE
#define RT_ENGT 0xFD
#define RT_OILP 0xFC
#define RT_OILT 0xFB
#define RT_MAP 0xFA
#define RT_O2 0xF9
#define RT_AIRT 0xF8
#define RT_TIME 0xF7
#define RT_TRIP 0xF6
#define RT_HIGH0 0xF5
#define RT_HIGH1 0xF4
#define RT_HIGH2 0xF3

#define KP_POUND 0xE0 // '#'
#define KP_AST 0xE1 // '*'
#define KP_0 0xE2 // '0'
#define KP_1 0xE3 // '1'
#define KP_2 0xE4 // '2'
#define KP_3 0xE5 // '3'
#define KP_4 0xE6 // '4'
#define KP_5 0xE7 // '5'
#define KP_6 0xE8 // '6'
#define KP_7 0xE9 // '7'
#define KP_8 0xEA // '8'
#define KP_9 0xEB // '9'
#define KP_A 0xEC // 'A'
#define KP_B 0xED // 'B'
#define KP_C 0xEE // 'C'
#define KP_D 0xEF // 'D'

#define MSG_0 0xD0	// message types sent from AVR to PIC24
#define MSG_1 0xD1
#define MSG_2 0xD2
#define MSG_3 0xD3

#define IDLE 0
#define CHECK_HIGHBIT 1
#define SEND_UCHAR 2
#define SEND_UINT1 3
#define SEND_UINT2 4

#define RTMAINC rt_main[curr_rt_layout]

#define NO_PROMPTS_EEPROM_LOCATION 0x03f0
#define PROMPT_INFO_OFFSET_EEPROM_LOCATION_LSB 0x03f2	// points to just after all the labels (prompt_info)
#define PROMPT_INFO_OFFSET_EEPROM_LOCATION_MSB 0x03f3
#define NO_LAYOUTS_EEPROM_LOCATION 0x03f4
#define LAYOUT_OFFSET_EEPROM_LOCATION_LSB 0x03f6	// points to just after all the prompt info (layout info)
#define LAYOUT_OFFSET_EEPROM_LOCATION_MSB 0x03f7
#define dispCharAt(_row,_col,_char) GDispCharAt((uint16_t)_row,(uint16_t)_col,(UCHAR)_char)
#define dispSetCursor(_mode,_row,_col,_type) GDispSetCursor ((UCHAR)_mode, (uint16_t)_row, (uint16_t)_col, (UCHAR)_type)
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

RT_MAIN *rt_main;
char *labels;

void dispRC(int row, int col);
void CheckRC(int *row, int *col, UCHAR *k);
void display_labels(void);
UCHAR default_func(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col);
UCHAR main_menu_func(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col);
UCHAR menu1a(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col);
UCHAR menu1b(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col);
UCHAR menu1c(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col);
UCHAR menu1d(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col);
UCHAR menu2a(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col);
UCHAR menu2b(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col);
UCHAR menu2c(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col);
UCHAR menu2d(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col);
void parse_PIC24(UCHAR);
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

char cur_global_number[10];
char new_global_number[10];
UCHAR cur_row, cur_col;	// used by the current menu/dialog function to keep track of the current row,col
PROMPT_STRUCT *prompt_ptr;
#endif
