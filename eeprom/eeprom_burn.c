// eeprom_burn.c - get used by test_write_data/test_write_data.c and eeprom/main_burn.c
// NOAVR is define when compiled in test_write_data
#include "../sfr_helper.h"
#ifndef NOAVR
#include <avr/io.h>
#include "../../avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include <avr/eeprom.h>
#include "../USART.h"
#include "../t6963.h"
#include "../macros.h"
#else
#include <ncurses.h>
#warning "NOAVR defined"
#endif
#include "../main.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef NOAVR
extern char eepromString[STRING_LEN] EEMEM;
#endif

//******************************************************************************************//
//***************************************** burn_eeprom ************************************//
//******************************************************************************************//
int burn_eeprom(void)
{
	int i,j;
	no_labels = 0;
	no_rtparams = 0;
	no_menu_structs = 0;
	total_offset = 0;
	char *ch;

#ifndef NOAVR
    printString("\r\nwriting to eeprom...\r\n");
#endif

    i = 0;
	j = 0;
    // the row,col elements don't really apply for the RT params
    // because the are displayed according to the row,col elements of the rt_layout
#if 1
	i = update_labels(i,"RPM\0");
	i = update_labels(i,"MPH\0");
	i = update_labels(i,"ENG TEMP\0");
	i = update_labels(i,"OIL PRESS\0");
	i = update_labels(i,"OIL TEMP\0");
	i = update_labels(i,"MAP\0");
	i = update_labels(i,"O2\0");
	i = update_labels(i,"AIR TEMP\0");
	i = update_labels(i,"TIME\0");
	i = update_labels(i,"TRIP\0");
	i = update_labels(i,"MENU1a\0");
	i = update_labels(i,"MENU1b\0");
	i = update_labels(i,"MENU1c\0");
	i = update_labels(i,"MENU2a\0");
	i = update_labels(i,"MENU2b\0");
	i = update_labels(i,"MENU2c\0");
	i = update_labels(i,"MENU3a\0");
	i = update_labels(i,"MENU3b\0");
	i = update_labels(i,"MENU3c\0");
	i = update_labels(i,"MENU4a\0");
	i = update_labels(i,"MENU4b\0");
	i = update_labels(i,"MENU4c\0");
	i = update_labels(i,"start num entry\0");
	i = update_labels(i,"num entry\0");
	i = update_labels(i,"alum entry\0");
	i = update_labels(i,"check box\0");
	i = update_labels(i,"<back>\0");
	no_labels = i;
#endif
	i = 0;
#ifndef NOAVR
	eeprom_update_word((UINT *)NO_LABELS_EEPROM_LOCATION,no_labels);
	printString("\r\nno_labels: ");
	printHexByte((UCHAR)no_labels>>8);
	printHexByte((UCHAR)no_labels);
	printString("\r\ndone writing no_labels to eeprom\r\n");
	eeprom_update_word((UINT *)RTPARAMS_OFFSET_EEPROM_LOCATION_LSB,total_offset);
	printString("\r\total_offset: ");
	printHexByte((UCHAR)total_offset>>8);
	printHexByte((UCHAR)total_offset);
	printString("\r\n");
#endif
	i = update_rtparams(i, 0, 0, 1, RT_RPM);	// first label is at offset 0
	i = update_rtparams(i, 1, 0, 1, RT_TRIP);	// first element of offset_array has offset of 2nd label
	i = update_rtparams(i, 2, 0, 1, RT_TIME);
	i = update_rtparams(i, 3, 0, 1, RT_AIRT);
	i = update_rtparams(i, 4, 0, 1, RT_O2);
	i = update_rtparams(i, 0, 15, 1, RT_MAP);
	i = update_rtparams(i, 1, 15, 1, RT_OILT);
	i = update_rtparams(i, 2, 15, 1, RT_OILP);
	i = update_rtparams(i, 3, 15, 1, RT_ENGT);
	i = update_rtparams(i, 4, 15, 1, RT_MPH);
	no_rtparams = i;

// write to the number of rt_params location in eeprom the number of rt_params
#ifndef NOAVR
	eeprom_update_word((UINT *)NO_RTPARAMS_EEPROM_LOCATION,no_rtparams);
	printString("\r\nno_rtparams: ");
	printHexByte((UCHAR)no_rtparams>>8);
	printHexByte((UCHAR)no_rtparams);
	printString("\r\ndone writing no_rtparams to eeprom\r\n");
	eeprom_update_word((UINT *)MENUSTRUCT_OFFSET_EEPROM_LOCATION_LSB,total_offset);
	printString("\r\total_offset: ");
	printHexByte((UCHAR)total_offset>>8);
	printHexByte((UCHAR)total_offset);
	printString("\r\n");
#endif
#if 1
	i = 0;
// still need to put the titles of each menu up somehow - prolly have another array of strings and index on pnum/4
	// main menu
	i = update_menu_structs(i, MENU_START_ROW+2, 5, KP_A, MENU1A, MAIN_MENU);
	i = update_menu_structs(i, MENU_START_ROW+2, MENU_START_COL+2, KP_B, MENU1B, MAIN_MENU);
	i = update_menu_structs(i, MENU_START_ROW+3, 5, KP_POUND, MENU1C, MAIN_MENU);
	i = update_menu_structs(i, MENU_START_ROW+3, MENU_START_COL+2, KP_AST, MENU1D, MAIN_MENU);

	// menu1a
	i = update_menu_structs(i, MENU_START_ROW+2, 5, KP_A, MENU2A, MENU1A);
	i = update_menu_structs(i, MENU_START_ROW+2, MENU_START_COL+2, KP_B, MENU2B, MENU1A);
	i = update_menu_structs(i, MENU_START_ROW+3, 5, KP_POUND, MENU2C, MENU1A);
	i = update_menu_structs(i, MENU_START_ROW+3, MENU_START_COL+2, KP_AST, GO_BACK, MENU1A);

	// menu1b
	i = update_menu_structs(i, MENU_START_ROW+2, 5, KP_A, MENU3A,  MENU1B);
	i = update_menu_structs(i, MENU_START_ROW+2, MENU_START_COL+2, KP_B, MENU3B, MENU1B);
	i = update_menu_structs(i, MENU_START_ROW+3, 5, KP_POUND, MENU3C, MENU1B);
	i = update_menu_structs(i, MENU_START_ROW+3, MENU_START_COL+2, KP_AST, GO_BACK, MENU1B);

	// menu1c
	i = update_menu_structs(i, MENU_START_ROW+2, 5, KP_A, MENU4A, MENU1C);
	i = update_menu_structs(i, MENU_START_ROW+2, MENU_START_COL+2, KP_B, MENU4B, MENU1C);
	i = update_menu_structs(i, MENU_START_ROW+3, 5, KP_POUND, MENU4C, MENU1C);
	i = update_menu_structs(i, MENU_START_ROW+3, MENU_START_COL+2, KP_AST, GO_BACK, MENU1C);

	// menu1d
	i = update_menu_structs(i, MENU_START_ROW+2, 5, KP_A,  GO_BACK,MENU1D);
	i = update_menu_structs(i, MENU_START_ROW+2, MENU_START_COL+2, KP_B, GO_BACK, MENU1D);
	i = update_menu_structs(i, MENU_START_ROW+3, 5, KP_POUND,  GO_BACK,MENU1D);
	i = update_menu_structs(i, MENU_START_ROW+3, MENU_START_COL+2, KP_AST,  GO_BACK,MENU1D);

	// menu2a
	i = update_menu_structs(i, MENU_START_ROW+2, 5, KP_A, GO_BACK,MENU2A);
	i = update_menu_structs(i, MENU_START_ROW+2, MENU_START_COL+2, KP_B,  GO_BACK,MENU2A);
	i = update_menu_structs(i, MENU_START_ROW+3, 5, KP_POUND,  GO_BACK,MENU2A);
	i = update_menu_structs(i, MENU_START_ROW+3, MENU_START_COL+2, KP_AST, GO_BACK, MENU2A);

	// menu2b
	i = update_menu_structs(i, MENU_START_ROW+2, 5, KP_A, GO_BACK, MENU2B);
	i = update_menu_structs(i, MENU_START_ROW+2, MENU_START_COL+2, KP_B,  GO_BACK,MENU2B);
	i = update_menu_structs(i, MENU_START_ROW+3, 5, KP_POUND,  GO_BACK,MENU2B);
	i = update_menu_structs(i, MENU_START_ROW+3, MENU_START_COL+2, KP_AST, GO_BACK, MENU2B);

	// menu2c
	i = update_menu_structs(i, MENU_START_ROW+2, 5, KP_A,  GO_BACK,MENU2C);
	i = update_menu_structs(i, MENU_START_ROW+2, MENU_START_COL+2, KP_B, GO_BACK, MENU2C);
	i = update_menu_structs(i, MENU_START_ROW+3, 5, KP_POUND,  GO_BACK,MENU2C);
	i = update_menu_structs(i, MENU_START_ROW+3, MENU_START_COL+2, KP_AST, GO_BACK, MENU2C);

	// menu3a
	i = update_menu_structs(i, MENU_START_ROW+2, 5, KP_A, GO_BACK, MENU3A);
	i = update_menu_structs(i, MENU_START_ROW+2, MENU_START_COL+2, KP_B,  GO_BACK,MENU3A);
	i = update_menu_structs(i, MENU_START_ROW+3, 5, KP_POUND,  GO_BACK,MENU3A);
	i = update_menu_structs(i, MENU_START_ROW+3, MENU_START_COL+2, KP_AST, GO_BACK, MENU3A);

	// menu3b
	i = update_menu_structs(i, MENU_START_ROW+2, 5, KP_A,  GO_BACK,MENU3B);
	i = update_menu_structs(i, MENU_START_ROW+2, MENU_START_COL+2, KP_B, GO_BACK, MENU3B);
	i = update_menu_structs(i, MENU_START_ROW+3, 5, KP_POUND,  GO_BACK,MENU3B);
	i = update_menu_structs(i, MENU_START_ROW+3, MENU_START_COL+2, KP_AST,  GO_BACK,MENU3B);

	// menu3c
	i = update_menu_structs(i, MENU_START_ROW+2, 5, KP_A, GO_BACK, MENU3C);
	i = update_menu_structs(i, MENU_START_ROW+2, MENU_START_COL+2, KP_B,  GO_BACK,MENU3C);
	i = update_menu_structs(i, MENU_START_ROW+3, 5, KP_POUND, GO_BACK, MENU3C);
	i = update_menu_structs(i, MENU_START_ROW+3, MENU_START_COL+2, KP_AST, GO_BACK, MENU3C);

	// menu4a
	i = update_menu_structs(i, MENU_START_ROW+2, 5, KP_A, GO_BACK, MENU4A);
	i = update_menu_structs(i, MENU_START_ROW+2, MENU_START_COL+2, KP_B, GO_BACK, MENU4A);
	i = update_menu_structs(i, MENU_START_ROW+3, 5, KP_POUND, GO_BACK, MENU4A);
	i = update_menu_structs(i, MENU_START_ROW+3, MENU_START_COL+2, KP_AST, GO_BACK, MENU4A);

	// menu4b
	i = update_menu_structs(i, MENU_START_ROW+2, 5, KP_A, GO_BACK, MENU4B);
	i = update_menu_structs(i, MENU_START_ROW+2, MENU_START_COL+2, KP_B, GO_BACK, MENU4B);
	i = update_menu_structs(i, MENU_START_ROW+3, 5, KP_POUND, GO_BACK, MENU4B);
	i = update_menu_structs(i, MENU_START_ROW+3, MENU_START_COL+2, KP_AST, GO_BACK, MENU4B);

	// memu4c
	i = update_menu_structs(i, MENU_START_ROW+2, 5, KP_A, GO_BACK, MENU4C);
	i = update_menu_structs(i, MENU_START_ROW+2, MENU_START_COL+2, KP_B, GO_BACK, MENU4C);
	i = update_menu_structs(i, MENU_START_ROW+3, 5, KP_POUND, GO_BACK, MENU4C);
	i = update_menu_structs(i, MENU_START_ROW+3, MENU_START_COL+2, KP_AST, GO_BACK, MENU4C);

	// start num entry
	i = update_menu_structs(i, MENU_START_ROW+2, 5, KP_A, GO_BACK, START_NUM_ENTRY);
	i = update_menu_structs(i, MENU_START_ROW+2, MENU_START_COL+2, KP_B, GO_BACK, START_NUM_ENTRY);
	i = update_menu_structs(i, MENU_START_ROW+3, 5, KP_POUND, GO_BACK,NUM_ENTRY);
	i = update_menu_structs(i, MENU_START_ROW+3, MENU_START_COL+2, KP_AST, GO_BACK, START_NUM_ENTRY);

	// num entry
	i = update_menu_structs(i, MENU_START_ROW+2, 5, KP_A, GO_BACK, NUM_ENTRY);
	i = update_menu_structs(i, MENU_START_ROW+2, MENU_START_COL+2, KP_B, GO_BACK, NUM_ENTRY);
	i = update_menu_structs(i, MENU_START_ROW+3, 5, KP_POUND, GO_BACK, NUM_ENTRY);
	i = update_menu_structs(i, MENU_START_ROW+3, MENU_START_COL+2, KP_AST, GO_BACK, NUM_ENTRY);

	// alnum entry
	i = update_menu_structs(i, MENU_START_ROW+2, 5, KP_A, GO_BACK, ALNUM_ENTRY);
	i = update_menu_structs(i, MENU_START_ROW+2, MENU_START_COL+2, KP_B, GO_BACK, ALNUM_ENTRY);
	i = update_menu_structs(i, MENU_START_ROW+3, 5, KP_POUND, GO_BACK, ALNUM_ENTRY);
	i = update_menu_structs(i, MENU_START_ROW+3, MENU_START_COL+2, KP_AST, GO_BACK, ALNUM_ENTRY);
#endif
	no_menu_structs = i;
	// now we can write to the NO_MENUS_EEPROM_LOCATION in eeprom
#ifndef	NOAVR
	eeprom_update_word((UINT *)NO_MENUS_EEPROM_LOCATION,no_menu_structs);
	printString("\r\nno_menu_structs: ");
	printHexByte((UCHAR)no_menu_structs>>8);
	printHexByte((UCHAR)no_menu_structs);
	printString("\r\ndone writing prompts to eeprom\r\n");
	printString("\r\total_offset: ");
	printHexByte((UCHAR)total_offset>>8);
	printHexByte((UCHAR)total_offset);
	printString("\r\n");
#endif
	return 0;
}
//******************************************************************************************//
//************************************** update_labels *************************************//
//******************************************************************************************//
int update_labels(int index, char *ramstr)
{
	int len;
	len = strlen(ramstr);
	len = (len > MAX_LABEL_LEN?MAX_LABEL_LEN:len);
	len++;
	label_offsets[index] = len;
	total_offset += len;
//	printf("len=%d i= %d\n",len,index);
// if NOAVR then just copy to array of strings
#ifndef NOAVR
    eeprom_update_block(ramstr, eepromString+len, len);
#else
	strncpy(labels[index],ramstr,len);
#endif
	index++;
	return index;
}
//******************************************************************************************//
//************************************* update_rtparams*************************************//
//******************************************************************************************//
int update_rtparams(int i, UCHAR row, UCHAR col, UCHAR shown, UCHAR type)
{
	rt_params[i].row = row;					// row, col tells where the param will appear on screen
	rt_params[i].col = col;
	rt_params[i].shown = shown;				// if its shown or not
	rt_params[i].type = type;					// 0 - UCHAR; 1 - UINT; 2 - string
#ifndef NOAVR
    eeprom_update_block(&rt_params[i], eepromString+total_offset, sizeof(RT_PARAM));
#endif
	total_offset += sizeof(RT_PARAM);
	i++;
	return i;
}
//******************************************************************************************//
//********************************* update_menu_structs*************************************//
//******************************************************************************************//
//int update_menu_structs(int i, char *label, UCHAR row, UCHAR col, UCHAR choice, UCHAR ch_type, UCHAR type)
int update_menu_structs(int i, UCHAR row, UCHAR col, UCHAR choice, UCHAR ch_type, UCHAR type)
{
	int len;
#ifdef MAIN_C
	menu_structs.pnum = i;				// index
	menu_structs.row = row;				// row, col where the label will be displayed
	menu_structs.col = col;
	menu_structs.menu_choice = choice;	// the type of keypress in the switch statement of the menu
	menu_structs.ch_type = ch_type;
	menu_structs.type = type;			// index of which function pointer this menu label is associated with
    eeprom_update_block(&menu_structs, eepromString+total_offset, sizeof(MENU_STRUCT));
#endif
#ifdef NOAVR
	menu_structs[i].pnum = i;				// index
	menu_structs[i].row = row;				// row, col where the label will be displayed
	menu_structs[i].col = col;
	menu_structs[i].menu_choice = choice;	// the type of keypress in the switch statement of the menu
	menu_structs[i].ch_type = ch_type;
	menu_structs[i].type = type;			// index of which function pointer this menu label is associated with
#endif
	total_offset += sizeof(MENU_STRUCT);
	i++;
	return i;
}
#ifndef NOAVR
//******************************************************************************************//
//************************************** read_eeprom****************************************//
//******************************************************************************************//
int read_eeprom(void)
{
	int i,j;
	UCHAR no_prompts = 0;
	UINT prompt_info_offset = 0;
	UCHAR temp;
	UINT temp2;
	char temp_label[MAX_LABEL_LEN];
	int offset = 0;
	char *ch;

	// read the labels into ram (if in NOAVR mode)
	printString("reading no_labels\r\n");
	no_labels = eeprom_read_byte((UCHAR*)NO_LABELS_EEPROM_LOCATION);
	printString("no_labels: ");
	printHexByte((UCHAR)(no_labels>>8));
	printHexByte((UCHAR)no_labels);
	printString("\r\n");

	printString("reading no_rtparams\r\n");
	no_rtparams = eeprom_read_byte((UCHAR*)NO_RTPARAMS_EEPROM_LOCATION);
	printString("no_rtparams: ");
	printHexByte((UCHAR)(no_rtparams>>8));
	printHexByte((UCHAR)no_rtparams);
	printString("\r\n");

	printString("reading no_menu_structs\r\n");
	no_menu_structs = eeprom_read_byte((UCHAR*)NO_MENUS_EEPROM_LOCATION);
	printString("no_menu_structs: ");
	printHexByte((UCHAR)(no_menu_structs>>8));
	printHexByte((UCHAR)no_menu_structs);
	printString("\r\n");

	rt_params_offset = (UINT)eeprom_read_byte((UCHAR*)RTPARAMS_OFFSET_EEPROM_LOCATION_LSB);	// read lsb
	temp = eeprom_read_byte((UCHAR*)RTPARAMS_OFFSET_EEPROM_LOCATION_MSB);		// read msb
	temp2 = (UINT)temp;
	rt_params_offset |= (temp2 << 8);		// shift msb over and bit-or with lsb
	printString("rt_params_offset: ");
	printHexByte((UCHAR)(rt_params_offset>>8));
	printHexByte((UCHAR)rt_params_offset);
	printString("\r\n");

	for(i = 0;i < no_labels;i++)
	{
		j = 0;
		eeprom_read_block(temp_label, eepromString+offset, MAX_LABEL_LEN);
		ch = temp_label;
		while(*ch != 0 && j < MAX_LABEL_LEN)
		{
			ch++;
			j++;
		}
		j++;		// adjust for zero at end
		offset += j;
		label_offsets[i] = j;	// fill label_offsets array so we can lookup and display all the labels in main_burn.c
	}

	menu_struct_offset = (UINT)eeprom_read_byte((UCHAR*)MENUSTRUCT_OFFSET_EEPROM_LOCATION_LSB);	// read lsb
	temp = eeprom_read_byte((UCHAR*)MENUSTRUCT_OFFSET_EEPROM_LOCATION_MSB);		// read msb
	temp2 = (UINT)temp;
	menu_struct_offset |= (temp2 << 8);		// shift msb over and bit-or with lsb
	printString("menu_struct_offset: ");
	printHexByte((UCHAR)(menu_struct_offset>>8));
	printHexByte((UCHAR)menu_struct_offset);
	printString("\r\n");

#if 0
	for(i = 0;i < no_prompts;i++)
	{
		eeprom_read_block((void*)&prompts[i], eepromString+(prompt_info_offset+(i*sizeof(PROMPT_STRUCT))),sizeof(PROMPT_STRUCT));
	}
	for(i = 0;i < no_prompts;i++)
	{
		printHexByte((UCHAR)prompts[i].pnum);
		transmitByte(0x20);
		printHexByte((UCHAR)prompts[i].row);
		transmitByte(0x20);
		printHexByte((UCHAR)prompts[i].col);
		transmitByte(0x20);
		printHexByte((UCHAR)(prompts[i].offset>>8));
		printHexByte((UCHAR)(prompts[i].offset));
		transmitByte(0x20);
		printHexByte((UCHAR)prompts[i].len);
		transmitByte(0x20);
		printHexByte((UCHAR)prompts[i].type);
		printString("\r\n");
	}
	printString("done reading eeprom\r\n");

	printString("reading prompt data into prompt structs\r\n");
	no_prompts = eeprom_read_byte((UCHAR*)NO_PROMPTS_EEPROM_LOCATION);
	printString("no_prompts: ");
	printHexByte(no_prompts);
	printString("\r\n");
	prompt_info_offset = (UINT)eeprom_read_byte((UCHAR*)PROMPT_INFO_OFFSET_EEPROM_LOCATION_LSB);	// read lsb
	temp = eeprom_read_byte((UCHAR*)PROMPT_INFO_OFFSET_EEPROM_LOCATION_MSB);		// read msb
	temp2 = (UINT)temp;
	prompt_info_offset |= (temp2 << 8);		// shift msb over and bit-or with lsb
	printString("prompt_info_offset: ");
	printHexByte((UCHAR)(prompt_info_offset>>8));
	printHexByte((UCHAR)prompt_info_offset);
	printString("\r\n");
	printString("\r\n");

	for(i = 0;i < no_prompts;i++)
	{
		eeprom_read_block((void*)&prompts[i], eepromString+(prompt_info_offset+(i*sizeof(PROMPT_STRUCT))),sizeof(PROMPT_STRUCT));
	}
	for(i = 0;i < no_prompts;i++)
	{
		printHexByte((UCHAR)prompts[i].pnum);
		transmitByte(0x20);
		printHexByte((UCHAR)prompts[i].row);
		transmitByte(0x20);
		printHexByte((UCHAR)prompts[i].col);
		transmitByte(0x20);
		printHexByte((UCHAR)(prompts[i].offset>>8));
		printHexByte((UCHAR)(prompts[i].offset));
		transmitByte(0x20);
		printHexByte((UCHAR)prompts[i].len);
		transmitByte(0x20);
		printHexByte((UCHAR)prompts[i].type);
		printString("\r\n");
	}
	printString("done reading eeprom\r\n");

	printString("reading prompt data into prompt structs\r\n");
	no_prompts = eeprom_read_byte((UCHAR*)NO_PROMPTS_EEPROM_LOCATION);
	printString("no_prompts: ");
	printHexByte(no_prompts);
	printString("\r\n");
	prompt_info_offset = (UINT)eeprom_read_byte((UCHAR*)PROMPT_INFO_OFFSET_EEPROM_LOCATION_LSB);	// read lsb
	temp = eeprom_read_byte((UCHAR*)PROMPT_INFO_OFFSET_EEPROM_LOCATION_MSB);		// read msb
	temp2 = (UINT)temp;
	prompt_info_offset |= (temp2 << 8);		// shift msb over and bit-or with lsb
	printString("prompt_info_offset: ");
	printHexByte((UCHAR)(prompt_info_offset>>8));
	printHexByte((UCHAR)prompt_info_offset);
	printString("\r\n");
	printString("\r\n");

	for(i = 0;i < no_prompts;i++)
	{
		eeprom_read_block((void*)&prompts[i], eepromString+(prompt_info_offset+(i*sizeof(PROMPT_STRUCT))),sizeof(PROMPT_STRUCT));
	}
	for(i = 0;i < no_prompts;i++)
	{
		printHexByte((UCHAR)prompts[i].pnum);
		transmitByte(0x20);
		printHexByte((UCHAR)prompts[i].row);
		transmitByte(0x20);
		printHexByte((UCHAR)prompts[i].col);
		transmitByte(0x20);
		printHexByte((UCHAR)(prompts[i].offset>>8));
		printHexByte((UCHAR)(prompts[i].offset));
		transmitByte(0x20);
		printHexByte((UCHAR)prompts[i].len);
		transmitByte(0x20);
		printHexByte((UCHAR)prompts[i].type);
		printString("\r\n");
	}
#endif
	printString("done reading eeprom\r\n");
	return 0;
}

#endif
