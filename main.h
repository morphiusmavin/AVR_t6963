// main.h - used by test_write_data.c, menus.c, main.c eeprom.c and burn_eeprom.c
#ifndef _MAIN_H_
#define _MAIN_H_
#define TIME_DELAY1 1
#define STRING_LEN 100
#define NUM_FPTS 14
#define MAX_LABEL_LEN 9
#define NUM_MENU_LABELS 31
#define NUM_RT_PARAMS 12
#define NUM_RT_LABELS NUM_RT_PARAMS
#define NUM_MENU_CHOICES 6
#define NUM_MENUS 17
#define NUM_MENU_STRUCTS NUM_MENUS*NUM_MENU_CHOICES
#define DISP_OFFSET 18
#define NUM_CHECKBOXES 10
#define SCALE_DISP_ALL 0
#define SCALE_DISP_SOME 1
#define SCALE_DISP_NONE 2
#define RT_OFFSET 0x70

typedef struct rt_params
{
	UCHAR row;			// row, col tells where the param will appear on screen
	UCHAR col;
	UCHAR shown;		// SHOWN_SENT = shown & sent; NOSHOWN_SENT = sent but not shown; NOSHOWN_NOSENT
	UCHAR dtype;		// 0 = UCHAR; 1 = UINT; 2 = dword?
	UCHAR type;			// rt_types
} RT_PARAM;

enum shown_types
{
	SHOWN_SENT,
	NOSHOWN_SENT,
	NOSHOWN_NOSENT
} SHOWN_TYPES;
	

typedef struct menu_func
{
	UCHAR enabled;		// if alt function will replace generic function
	UCHAR fptr;			// which function to call (menu_types)
	UCHAR menu;			// if fptr == 0 then it means goto this menu
	UCHAR label;		// which label to display in legend (labels)
	UCHAR index;		// if > 0 then this is index into sample_data
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
	testnum1,
	testnum2,		// total of 17 menus
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
	ckenter,
	ckesc
} MENU_TYPES;

enum data_types
{
	RT_LOW,				// UCHAR without high bit set
	RT_HIGH0,			// UCHAR with bit 7 set
	RT_HIGH1,			// UINT with neither 7 or 15 set
	RT_HIGH2,			// bit 7 of UINT set
	RT_HIGH3			// bit 15 of UINT set
} DATA_TYPES;
enum rt_types
{
	RT_RPM = RT_OFFSET,
	RT_ENGT,
	RT_TRIP,
	RT_TIME,
	RT_AIRT,
	RT_MPH,
	RT_OILP,
	RT_MAP,
	RT_OILT,
	RT_O2,
	RT_AUX1,
	RT_AUX2
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
	KP_D // 'D'	(0xEF)
} KEY_TYPES;
enum states
{
	IDLE = 1,
	CHECK_HIGHBIT,
	GET_CH0,
	GET_CH1,
	GET_CH2,
	SEND_UCHAR0,	// UCHAR without high bit set
	SEND_UCHAR1,	// UCHAR with high bit set
	SEND_UINT0,		// UINT with neither bit 7 or 15 set
	SEND_UINT1,		// UINT with bit 7 set
	SEND_UINT2		// UINT with bit 15 set
} STATES;

enum aux_states
{
	IDLE_AUX,
	DATA_REQ,
	VALID_DATA,
	DATA_READY,
	EXTRA
} AUX_STATES;

enum aux_commands
{
	CMD_GET_DATA = 6,		// AVR tells PIC24 what data it wants
	CMD_DATA_READY,			// PIC24 tells AVR it has the data to send
	CMD_NEW_DATA,			// AVR tells PIC24 it has new data to store
	CMD_EXTRA
} AUX_CMDS;	

#define NUM_ENTRY_SIZE 7
#define AUX_DATA_SIZE 4
//#define NUM_ENTRY_BEGIN_COL (COLUMN - COLUMN/2)
#define NUM_ENTRY_BEGIN_COL 3
#define NUM_ENTRY_END_COL NUM_ENTRY_BEGIN_COL + NUM_ENTRY_SIZE
#define NUM_ENTRY_ROW 8
// '!' - '9' (33 - 58) + 'A' - 'Z' (26) + 'a' - 'z' (26) = 77
//#define NUM_ALNUM 77
//#define NUM_ALNUM 52		// without the '!' - '9'
#define NUM_ALNUM 85		// include all special chars
#define ALNUM_SCROLL_LIST_LEN 13
#define MENU_START_ROW 12
#define MENU_START_COL 15
#define MENU_BLANK "          "
#define NO_MENU_LABELS_EEPROM_LOCATION 0x03e0
#define NO_RT_LABELS_EEPROM_LOCATION 0x03e2
#define NO_RTPARAMS_EEPROM_LOCATION 0x03e4
#define NO_MENUS_EEPROM_LOCATION 0x3e6
#define RTPARAMS_OFFSET_EEPROM_LOCATION_LSB 0x03e8	// points to just after all the labels (prompt_info)
#define RTPARAMS_OFFSET_EEPROM_LOCATION_MSB 0x03ea
#define MENUSTRUCT_OFFSET_EEPROM_LOCATION_LSB 0x03ec	// points to just after all the labels (prompt_info)
#define MENUSTRUCT_OFFSET_EEPROM_LOCATION_MSB 0x03ee
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
#ifndef NOPARSE
#ifdef MAIN_C
int parse_P24(UCHAR ch, char *param_string, UCHAR *xbyte, UINT *xword);
#else
int parse_P24(WINDOW *win, int fd, UCHAR ch, char *param_string);
#endif
#endif
//int update_menu_structs(int i, char *label, UCHAR row, UCHAR col, UCHAR choice, UCHAR ch_type, UCHAR type);
int update_menu_structs(int i, UCHAR enabled, UCHAR fptr, UCHAR menu, UCHAR label, UCHAR index);
int update_rtparams(int i, UCHAR row, UCHAR col, UCHAR shown, UCHAR dtype, UCHAR type);
int update_menu_labels(int i, char *ramstr);
int update_rt_labels(int index, char *ramstr);
UCHAR current_param;
UINT temp_UINT;
UCHAR parse_state;
UCHAR cursor_row, cursor_col;
int no_rt_labels;
int no_rtparams;
int no_menu_structs;
int no_menu_labels;
int no_data_index;
//UINT label_info_offset;
UINT rt_params_offset;
UINT menu_struct_offset;
char menu_labels[NUM_MENU_LABELS][MAX_LABEL_LEN];
char rt_labels[NUM_RT_LABELS][MAX_LABEL_LEN];
//char labels[1][MAX_LABEL_LEN];
// just have 1 copy in ram and reload from eeprom every time we change menus
MENU_FUNC_STRUCT menu_structs[NUM_MENU_STRUCTS];
char *get_label(int index);
UCHAR get_row(int index);
UCHAR get_col(int index);
RT_PARAM rt_params[NUM_RT_PARAMS];
// define a separate rt_params for the write part of test_write_data.c_str
// because we want to handle this as if a separate array is running on the PIC24
#ifdef NOAVR
RT_PARAM P24_rt_params[NUM_RT_PARAMS];
void disp_cmdtype(UCHAR state, char *str);
#endif
//#ifdef NOAVR
// we could read the labels into ram when in AVR mode but its just as easy to read them from
// eeprom directly - doesn't take that much more time, plus it saves ram space in AVR
// not worried about it when compiling in NOAVR mode on a linux box in 32/64 bit mode.
// label_offsets is an array that get set to the length of each label by searching
// for the first 0
int label_offsets[NUM_MENU_LABELS+NUM_RT_LABELS];
int total_offset;
//#endif
char cur_global_number[NUM_ENTRY_SIZE];
char new_global_number[NUM_ENTRY_SIZE];
UCHAR aux_type;	// tells PIC24 what to send when code = RT_AUX
UCHAR paux_state;
UCHAR aaux_state;
//UCHAR aux_data[AUX_DATA_SIZE];
//UCHAR aux_data2[AUX_DATA_SIZE];
void set_state_defaults(void);
CHECKBOXES check_boxes[NUM_CHECKBOXES];
int curr_checkbox;
int last_checkbox;
int scale_type;
int prev_scale_type;
UCHAR ask_data_ready;
UCHAR aux_index;
UCHAR new_data_ready;
UCHAR mod_data_ready;
UCHAR data_entry_mode;
//UCHAR auxcmd;
//UCHAR auxparam;

#endif
