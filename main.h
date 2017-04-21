// main.h - used by test_write_data.c, menus.c, main.c eeprom.c and burn_eeprom.c
#ifndef _MAIN_H_
#define _MAIN_H_
#define TIME_DELAY1 1
#define STRING_LEN 100
#define NUM_FPTS 13
#define MAX_LABEL_LEN 9
#define NUM_LABELS 39
#define NUM_MENU_CHOICES 6
#define NUM_MENUS 17
#define NUM_MENU_STRUCTS NUM_MENUS*NUM_MENU_CHOICES
#define NUM_MENU_FUNCS 10
#define NUM_RT_PARAMS 20
#define DISP_OFFSET 18
#define NUM_CHECKBOXES 10

typedef struct rt_params
{
	UCHAR row;			// row, col tells where the param will appear on screen
	UCHAR col;
	UCHAR shown;		// if its shown or not
	UCHAR type;			// rt_types
} RT_PARAM;

typedef struct menu_func
{
	UCHAR enabled;		// if alt function will replace generic function
	UCHAR fptr;			// which function to call (menu_types)
	UCHAR menu;			// if fptr == 0 then it means goto this menu
	UCHAR label;		// which label to display in legend (labels)
} MENU_FUNC_STRUCT;

typedef struct checkboxes
{
	UCHAR index;
	UCHAR checked;
	char string[20];
} CHECKBOXES;

enum menu_types
{
	MAIN,
	MENU1A,
	MENU1B,
	MENU1C,
	MENU1D,
	num_entry,
	alnum_entry,
	chkboxes,
	MENU2A,
	MENU2B,
	MENU2C,
	MENU3A,
	MENU3B,
	MENU3C,
	MENU4A,
	MENU4B,
	MENU4C,		// total of 17 menus
/*
functions start here - these are indexes into the array of function pointers (in menus.c)

static UCHAR (*fptr[NUM_FPTS])(UCHAR) = {enter, backspace, escape, scr_alnum0, \
		 scr_alnum1, scr_alnum2, scr_alnum3, cursor_forward, alnum_enter, scrollup_checkboxes, \
			scrolldown_checkboxes, toggle_checkboxes, enter_checkboxes };

use these when calling update_menu_structs() in eeprom_burn.c
*/
	entr,
	back,
	esc,
	caps,
	small,
	spec,
	next,
	cur_for,
	alnum_ent,
	ckup,
	ckdown,
	cktoggle,
	ckenter
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
	RT_RPM,
	RT_ENGT,
	RT_TRIP,
	RT_TIME,
	RT_AIRT,
	RT_MPH,
	RT_OILP,
	RT_MAP,
	RT_OILT,
	RT_O2,
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

enum AUX_cmd_types
{
	PIC24_GET_DATA = 1,
	PIC24_DATA_READY,
	AVR_HAS_NEW_DATA,
	KEEP_IDLE,
	EXTRA
} AUX_CMDS;

enum AUX_param_types
{
	TYPE_1 = 1,
	TYPE_2,
	TYPE_3,
	TYPE_4
} AUX_PARAMS;

/*
1) AVR sends a REQ_DATA command along with param to tell what value it wants (if parse_P24 is done)
2) PIC24 reads the byte - if 0 then doesn't do anything; if > 0 then it goes out and gets the value(s)
3) on the next round, the AVR reads a byte sent by the PIC24 saying the data is ready
4) if its not ready, go another round (or go so many rounds and then send another command)
5) AVR reads the ready flag
6) AVR then reads data
7) AVR sets a flag showing value has been received
8) AVR will put value in checkbox dialog (if its a binary) or in num entry dialog (if UCHAR or UINT) (?word)
9) AVR will send a command telling the PIC24 that it can read the changed value(s) back
10) AVR will send data

each one of the following states is one whole cycle in the rt_params
*/
enum PIC24_aux_states
{
	P24_IDLE = 1,
	GET_DATA,			// when PIC24 gets a REQ_DATA then go out and get it
	SEND_DATA_READY,	// tell AVR data is ready to send
	SEND_DATA,			// send data (how much is predefined)
	P24_WAIT_NEW_DATA,		// wait for AVR to send the updated value(s)
	P24_STORE_NEW_DATA
} PIC24_AUX;

enum AVR_aux_states
{
	AVR_IDLE = 1,
	REQ_DATA,			//  when menu choice is get certain data (this one starts everything)
	WAIT_DATA_READY,	// wait for SEND_DATA_READY from PIC24 (keep making rounds until data is ready to read)
	READ_DATA,			// read data (how much is predefined)
	AVR_WAIT_NEW_DATA,		// wait for stupid user to finish editing new data
	SEND_NEW_DATA
} AVR_AUX;
#define NUM_ENTRY_SIZE 10
#define AUX_DATA_SIZE 8
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
#define NO_LABELS_EEPROM_LOCATION 0x03e0
#define NO_RTPARAMS_EEPROM_LOCATION 0x03e1
#define NO_MENUS_EEPROM_LOCATION 0x3e2
#define RTPARAMS_OFFSET_EEPROM_LOCATION_LSB 0x03e5	// points to just after all the labels (prompt_info)
#define RTPARAMS_OFFSET_EEPROM_LOCATION_MSB 0x03e6
#define MENUSTRUCT_OFFSET_EEPROM_LOCATION_LSB 0x03e7	// points to just after all the labels (prompt_info)
#define MENUSTRUCT_OFFSET_EEPROM_LOCATION_MSB 0x03e8
#define dispCharAt(_row,_col,_char) GDispCharAt((UINT)_row,(UINT)_col,(UCHAR)_char)
#define dispSetCursor(_mode,_row,_col,_type) GDispSetCursor ((UCHAR)_mode, (UINT)_row, (UINT)_col, (UCHAR)_type)
void dispRC(int row, int col);
void CheckRC(int *row, int *col, UCHAR *k);
void display_labels(void);
void set_defaults(void);
#ifdef NOAVR
void do_read(WINDOW *win, int fd, int display_offset);
void set_win2(WINDOW *win);
#endif
void init_list(void);
UCHAR get_key(UCHAR ch);
int curr_fptr_changed(void);
int get_curr_menu(void);
int get_str_len(void);
int burn_eeprom(void);
int read_eeprom(void);
#ifdef MAIN_C
int parse_P24(UCHAR ch, char *param_string, UCHAR *xbyte, UINT *xword);
#else
int parse_P24(WINDOW *win, int fd, UCHAR ch, char *param_string);
#endif
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
UCHAR aux_type;	// tells PIC24 what to send when code = RT_AUX
UCHAR paux_state;
UCHAR aaux_state;
UCHAR aux_data[AUX_DATA_SIZE];
void set_state_defaults(void);
CHECKBOXES check_boxes[NUM_CHECKBOXES];
int curr_checkbox;
int last_checkbox;
#endif
