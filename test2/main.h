// main.h - used by test_write_data.c, menus.c, main.c eeprom.c and burn_eeprom.c
#ifndef _MAIN_H_
#define _MAIN_H_
#define TIME_DELAY1 1
#define STRING_LEN   100
#define NUM_FPTS 9
#define MAX_LABEL_LEN 9
#define NUM_LABELS 33
#define NUM_MENU_CHOICES 6
#define NUM_MENUS 17
#define NUM_MENU_STRUCTS NUM_MENUS*NUM_MENU_CHOICES
#define NUM_MENU_FUNCS 10
#define NUM_RT_PARAMS 20
typedef struct rt_params
{
	UCHAR row;			// row, col tells where the param will appear on screen
	UCHAR col;
	UCHAR shown;		// if its shown or not
	UCHAR type;			// 0 - UCHAR; 1 - UINT; 2 - string
} RT_PARAM;

typedef struct menu_func
{
	UCHAR enabled;		// if alt function will replace generic function
	UCHAR fptr;			// which function to call (menu_types)
	UCHAR menu;			// if fptr == 0 then it means goto this menu
	UCHAR label;			// which label to display in legend (labels)
} MENU_FUNC_STRUCT;

enum menu_types
{
	MAIN_MENU,			// 0
	MENU1A,				// 1
	MENU1B,				// 2
	MENU1C,				// 3
	MENU1D,				// 4
	MENU2A,				// 5
	MENU2B,				// 6
	MENU2C,				// 7
	MENU3A,				// 8
	MENU3B,				// 9
	MENU3C,				// 10
	MENU4A,				// 11
	MENU4B,				// 12
	MENU4C,				// 13
	test1,				// 14
	test2,				// 15
	test3,				// 16
	test4,				// 17
	START_NUM_ENTRY,	// 18
	NUM_ENTRY,			// 19
	ALNUM_ENTRY,		// 20
	CHECK_BOX,			// 21
	GO_BACK				// 22
} MENU_TYPES;

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
	RT_RPM = 2,
	RT_TRIP,
	RT_TIME,
	RT_AIRT,
	RT_O2,
	RT_MAP,
	RT_OILT,
	RT_OILP,
	RT_ENGT,
	RT_MPH,
	RT_AUX
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
	IDLE = 1,
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

#define NO_LABELS_EEPROM_LOCATION 0x03e0
#define NO_RTPARAMS_EEPROM_LOCATION 0x03e1
#define NO_MENUS_EEPROM_LOCATION 0x3e2

//#define LABEL_INFO_OFFSET_EEPROM_LOCATION_LSB 0x03e3	// points to just after all the labels - stores the offsets of each label
//#define LABEL_INFO_OFFSET_EEPROM_LOCATION_MSB 0x03e4

#define RTPARAMS_OFFSET_EEPROM_LOCATION_LSB 0x03e5	// points to just after all the labels (prompt_info)
#define RTPARAMS_OFFSET_EEPROM_LOCATION_MSB 0x03e6

#define MENUSTRUCT_OFFSET_EEPROM_LOCATION_LSB 0x03e7	// points to just after all the labels (prompt_info)
#define MENUSTRUCT_OFFSET_EEPROM_LOCATION_MSB 0x03e8

//#define dispCharAt(_row,_col,_char) GDispCharAt((UINT)_row,(UINT)_col,(UCHAR)_char)
#define dispCharAt(_row,_col,_char) printf("%c",(UCHAR)_char)
#define dispSetCursor(_mode,_row,_col,_type) GDispSetCursor ((UCHAR)_mode, (UINT)_row, (UINT)_col, (UCHAR)_type)

void dispRC(int row, int col);
void CheckRC(int *row, int *col, UCHAR *k);
void display_labels(void);
void parse_PIC24(UCHAR);
void set_defaults(void);
#ifdef NOAVR
int do_read(WINDOW *win, int fd, int display_offset);
void set_win2(WINDOW *win);
#endif
void init_list(void);
UCHAR get_key(UCHAR ch);
int get_curr_fptr(void);
int curr_fptr_changed(void);
int get_curr_menu(void);
int get_type(void);
int get_str_len(void);
char *get_label(int index);
int burn_eeprom(void);
int read_eeprom(void);
//int update_menu_structs(int i, char *label, UCHAR row, UCHAR col, UCHAR choice, UCHAR ch_type, UCHAR type);
int update_menu_structs(int i, UCHAR enabled, UCHAR fptr, UCHAR menu, UCHAR label);
int update_rtparams(int i, UCHAR row, UCHAR col, UCHAR shown, UCHAR type);
int update_labels(int i, char *ramstr);

UCHAR current_param;
UINT temp_UINT;
UCHAR parse_state;
UCHAR cursor_row, cursor_col;

int no_labels;
int no_rtparams;
int no_menu_structs;

//UINT label_info_offset;
UINT rt_params_offset;
UINT menu_struct_offset;

char labels[NUM_LABELS][MAX_LABEL_LEN];

// just have 1 copy in ram and reload from eeprom every time we change menus
MENU_FUNC_STRUCT menu_structs[NUM_MENU_STRUCTS];

int get_menu_struct_type(int index);
int get_menu_struct_choice(int index);
int get_menu_struct_chtype(int index);
char *get_label(int index);
UCHAR get_row(int index);
UCHAR get_col(int index);
RT_PARAM rt_params[NUM_RT_PARAMS];
//#ifdef NOAVR
// we could read the labels into ram when in AVR mode but its just as easy to read them from
// eeprom directly - doesn't take that much more time, plus it saves ram space in AVR
// not worried about it when compiling in NOAVR mode on a linux box in 32/64 bit mode.
// label_offsets is an array that get set to the length of each label by searching
// for the first 0
int label_offsets[NUM_LABELS];
int total_offset;
//#endif
char cur_global_number[NUM_ENTRY_SIZE];
char new_global_number[NUM_ENTRY_SIZE];
#endif
