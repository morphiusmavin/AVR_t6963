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
	i = update_labels(i,"OIL PRES\0");
	i = update_labels(i,"OIL TEMP\0");
	i = update_labels(i,"MAP\0");
	i = update_labels(i,"O2\0");
	i = update_labels(i,"AIR TEMP\0");
	i = update_labels(i,"TIME\0");
	i = update_labels(i,"TRIP\0");
	i = update_labels(i,"MAINMENU\0");
	i = update_labels(i,"MENU__1a\0");
	i = update_labels(i,"MENU__1b\0");
	i = update_labels(i,"MENU__1c\0");
	i = update_labels(i,"MENU__1d\0");
	i = update_labels(i,"MENU__2a\0");
	i = update_labels(i,"MENU__2b\0");
	i = update_labels(i,"MENU__2c\0");
	i = update_labels(i,"MENU__3a\0");
	i = update_labels(i,"MENU__3b\0");
	i = update_labels(i,"MENU__3c\0");
	i = update_labels(i,"MENU__4a\0");
	i = update_labels(i,"MENU__4b\0");
	i = update_labels(i,"MENU__4c\0");
	i = update_labels(i,"test1\0");
	i = update_labels(i,"test2\0");
	i = update_labels(i,"test3\0");
	i = update_labels(i,"snmentry\0");
	i = update_labels(i,"numentry\0");
	i = update_labels(i,"aluentry\0");
	i = update_labels(i,"checkbox\0");
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
#else
	printf("no_labels: %d\n",no_labels);
	printf("total_offset: %d\n",total_offset);
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
#else
	printf("no_rtparams: %d\n",no_rtparams);
	printf("total_offset: %d\n",total_offset);
#endif
#if 1
	i = 0;
// still need to put the titles of each menu up somehow - prolly have another array of strings and index on pnum/4
	// main menu
//	i = update_menu_structs(int i, UCHAR enabled, UCHAR key, UCHAR fptr, UCHAR menu, UCHAR label);

// TODO: make these do something!
/*
	UCHAR enabled;		// if alt function will replace generic function
	UCHAR fptr;			// which function to call (menu_types)
	UCHAR menu;			// if fptr == 0 then it means goto this menu
	UCHAR label;		// which label to display in legend (labels)
*/
	UCHAR enabled, fptr, menu, label = 0;
/*
	MAIN_MENU = 1,		// 0
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
	test1,
	test2,
	test3,
*/
	enabled = 1;
	// main menu
	i = update_menu_structs(i, enabled, 0, MENU1A, MENU1A);
	i = update_menu_structs(i, enabled, 0, MENU1B, MENU1B);
	i = update_menu_structs(i, enabled, 0, MENU1C, MENU1C);
	i = update_menu_structs(i, enabled, 0, MENU1D, MENU1D);
	i = update_menu_structs(i, 0, test2, 0, test2);
	i = update_menu_structs(i, 0, test3, 0, test3);

	fptr++;

// load the only ram copy of menu_structs with the first one
//	menu_structs[i].enabled = enabled;
//	menu_structs[i].fptr = fptr;
//	menu_structs[i].menu = menu;
//	menu_structs[i].label = label;

#if 1
	// menu 1a
	i = update_menu_structs(i, 1, 0, MAIN_MENU, MAIN_MENU);
	i = update_menu_structs(i, 1, test1, 0, test1);
	i = update_menu_structs(i, 1, test2, 0, test2);
	i = update_menu_structs(i, 1, test3, 0, test3);
	i = update_menu_structs(i, 0, test1, 0, test1);
	i = update_menu_structs(i, 1, 0, MAIN_MENU, MAIN_MENU);

	fptr++;
	// menu 1b
	i = update_menu_structs(i, enabled, 0, MENU1A, MENU1A);
	i = update_menu_structs(i, enabled, 0, MENU1B, MENU1B);
	i = update_menu_structs(i, 1, test1, 0, test1);
	i = update_menu_structs(i, 1, test2, 0, test2);
	i = update_menu_structs(i, 1, test3, 0, test3);
	i = update_menu_structs(i, 1, 0, MAIN_MENU, MAIN_MENU);

	fptr++;
	// menu 1c
	i = update_menu_structs(i, 0, 2, menu, label);
	i = update_menu_structs(i, 0, 3, menu, label);
	i = update_menu_structs(i, 0, 4, menu, label);
	i = update_menu_structs(i, 0, 5, menu, label);
	i = update_menu_structs(i, 0, 6, menu, label);
	i = update_menu_structs(i, 0, 7, menu, label);

	fptr++;
	// menu 1d
	i = update_menu_structs(i, 0, 8, menu, label);
	i = update_menu_structs(i, 0, 9, menu, label);
	i = update_menu_structs(i, 0, 10, menu, label);
	i = update_menu_structs(i, 0, 11, menu, label);
	i = update_menu_structs(i, 0, 12, menu, label);
	i = update_menu_structs(i, 0, 13, menu, label);

	fptr++;
	// menu 2a
	i = update_menu_structs(i, 0, 14, menu, label);
	i = update_menu_structs(i, 0, 15, menu, label);
	i = update_menu_structs(i, 0, 16, menu, label);
	i = update_menu_structs(i, 0, 17, menu, label);
	i = update_menu_structs(i, 0, 18, menu, label);
	i = update_menu_structs(i, 0, 19, menu, label);

	fptr++;
	// menu 2b
	i = update_menu_structs(i, 0, 20, menu, label);
	i = update_menu_structs(i, 0, 21, menu, label);
	i = update_menu_structs(i, 0, 22, menu, label);
	i = update_menu_structs(i, 0, 23, menu, label);
	i = update_menu_structs(i, 0, 24, menu, label);
	i = update_menu_structs(i, 0, 25, menu, label);

	fptr++;
	// menu 2c
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);

	fptr++;
	// menu 3a
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);

	fptr++;
	// menu 3b
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);

	fptr++;
	// menu 3c
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);
	fptr++;
	// menu 4a
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);

	fptr++;
	// menu 4b
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);

	fptr++;
	// menu 4c
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);
	i = update_menu_structs(i, 0, fptr, menu, label);
#endif
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
#else
	printf("no_menu_structs: %d\n",no_menu_structs);
	printf("total_offset: %d\n",total_offset);
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
#endif
	strncpy(labels[index],ramstr,len);
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
int update_menu_structs(int i, UCHAR enabled, UCHAR fptr, UCHAR menu, UCHAR label)
{
	int len;
/*
	UCHAR enabled;		// if alt function will replace generic function
	UCHAR key;			// which keypress applies
	UCHAR fptr;			// which function to call (menu_types)
	UCHAR menu;			// if fptr == 0 then it means goto a this menu
	UCHAR label;			// which label to display in legend (labels)
*/
	menu_structs[i].enabled = enabled;
	if(enabled == 1)
	{
		menu_structs[i].fptr = fptr;
		menu_structs[i].menu = menu;
		menu_structs[i].label = label;
	}
	else
	{
		menu_structs[i].fptr = 0;
		menu_structs[i].menu = 0;
		menu_structs[i].label = 0;
	}
#ifndef NOAVR
    eeprom_update_block(&menu_structs, eepromString+total_offset, sizeof(MENU_FUNC_STRUCT));
#endif
	total_offset += sizeof(MENU_FUNC_STRUCT);
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

	printString("done reading eeprom\r\n");
	return 0;
}
#endif
