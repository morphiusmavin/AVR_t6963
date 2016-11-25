#include <avr/io.h>
#include "../avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include "sfr_helper.h"
#include <avr/eeprom.h>
#include <stdlib.h>
#include "USART.h"
#include "t6963.h"
#include "macros.h"
#include <string.h>
#define TIME_DELAY1 1
#define RT_LABEL 0x00
#define MENU1 0x01
#define MENU2 0x02
#define MENU3 0x03
#define STRING_LEN   255
#define ROWS 16
#define NUM_FPTS 10
#define NO_PROMPTS_EEPROM_LOCATION 0x03f0
#define PROMPT_INFO_OFFSET_EEPROM_LOCATION_LSB 0x03f2
#define PROMPT_INFO_OFFSET_EEPROM_LOCATION_MSB 0x03f3
#define NO_LAYOUTS_EEPROM_LOCATION 0x03f4
#define LAYOUT_OFFSET_EEPROM_LOCATION_LSB 0x03f6
#define LAYOUT_OFFSET_EEPROM_LOCATION_MSB 0x03f7
#define dispCharAt(_row,_col,_char) GDispCharAt((uint16_t)_row,(uint16_t)_col,(UCHAR)_char)
#define dispSetCursor(_mode,_row,_col,_type) GDispSetCursor ((UCHAR)_mode, (uint16_t)_row, (uint16_t)_col, (UCHAR)_type)

/* #include <stdio.h> */
/* #include <stdlib.h> */

// Define EEMEM variables

void update_prompt_struct(uint8_t pnum, UCHAR row, UCHAR col, uint16_t *offset, uint8_t type,char *ramstr);
void dispRC(int row, int col);
void CheckRC(int *row, int *col, UCHAR *k);
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

void printMenu(void);

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
} RT_LAYOUT_STRUCT;

typedef struct RT_main
{
	char name[10];	// name of layout style
	int num_params;		// 
	RT_LAYOUT_STRUCT rt_layout[15];		// up to 15 possible parameters that can be listed
	uint16_t offset;		// used by malloc after read from eeprom
} RT_MAIN;


char eepromString[STRING_LEN] EEMEM;

PROMPT_STRUCT prompts[30];
uint8_t no_prompts = 0;
uint16_t prompt_info_offset = 0;

char cur_global_number[10];
char new_global_number[10];
UCHAR cur_row, cur_col;	// used by the current menu/dialog function to keep track of the current row,col


void update_prompt_struct(uint8_t pnum, UCHAR row, UCHAR col, uint16_t *offset, uint8_t type,char* ramstr);

int main(void) 
{
    uint8_t test1;
    char ramString[STRING_LEN];
	int i;
    int row,col;
    UCHAR k;
	uint8_t temp;
	uint16_t temp2;
	int current_fptr = 0;
	uint16_t limit16;
	uint8_t limit8;
	UCHAR ret_char;
	int curr_rt_layout = 0;
	int label_list_size;
	RT_MAIN *rt_main;
	RT_LAYOUT *tptr;
	char temp3[2];
	void *malloc_ptr;

	uint8_t no_prompts = 0;
	uint16_t prompt_info_offset = 0;
	uint16_t layout_offset = 0;
	uint8_t no_layouts = 0;

    uint8_t promptString[sizeof(PROMPT_STRUCT)];
    size_t str_size = sizeof(PROMPT_STRUCT);
    uint16_t total_strlen;
	
	UCHAR (*fptr[NUM_FPTS])(UCHAR, uint8_t, uint16_t, UCHAR, UCHAR) = { default_func,\
		 main_menu_func,\
		 menu1a, menu1b,\
		 menu1c, menu1d,\
		 menu2a, menu2b,\
		 menu2c, menu2d };
#if 1
    initUSART();

	GDispInit();
	_delay_us(10);
	GDispSetMode(XOR_MODE);
	_delay_us(10);
	GDispSetMode(TEXT_ON);
	_delay_us(10);
	GDispClrTxt();    
	GDispStringAt(1,1,"LCD is on!");
	printString("LCD is on!.\r\n");

	printString("tesing LCD\r\n");
	k = 0x20;
	for(row = 0;row < ROWS;row++)
	{
		for(col = 0;col < COLUMN;col++)
		{
			dispCharAt(row,col,k);
			if(++k > 0x7e)
				k = 0x20;
		}
	}
    no_prompts = eeprom_read_byte((uint8_t*)0x03f0);

	if(no_prompts != 0xff)
	{
		printString("reading prompt data into prompt structs\r\n");
		no_prompts = eeprom_read_byte((uint8_t*)NO_PROMPTS_EEPROM_LOCATION);
		no_layouts = eeprom_read_byte((uint8_t*)NO_LAYOUTS_EEPROM_LOCATION);
		prompt_info_offset = (uint16_t)eeprom_read_byte((uint8_t*)PROMPT_INFO_OFFSET_EEPROM_LOCATION_LSB);	// read lsb
		temp = eeprom_read_byte((uint8_t*)PROMPT_INFO_OFFSET_EEPROM_LOCATION_MSB);		// read msb
		temp2 = (uint16_t)temp;
		prompt_info_offset |= (temp2 << 8);		// shift msb over and bit-or with lsb
		malloc_ptr = malloc((size_t)prompt_info_offset);
		printString("prompt_info_offset: ");
		printHexByte((uint8_t)prompt_info_offset);
		printHexByte((uint8_t)(prompt_info_offset>>8));
		printString("\r\n");
		for(i = 0;i < no_prompts;i++)
		{
			eeprom_read_block((void*)(&(prompts[i])),(eepromString+(i*str_size)+prompt_info_offset), str_size);
		}
		eeprom_read_block(malloc_ptr, eepromString, prompt_info_offset);
		for(i = 0;i < no_prompts;i++)
		{
			memcpy((void*)ramString,(malloc_ptr+prompts[i].offset),prompts[i].len+1);
//			printHexByte(ramString+prompts[i].len);
//			printHexByte(ramString+prompts[i].len+1);
//			printString(ramString);
//			printString("\r\n");
		}
		// now read into the heap all the real-time display format data
		rt_main = (RT_MAIN*)malloc(sizeof(RT_MAIN)*no_layouts);
		if(rt_main == NULL)
		{
			printString("malloc returned NULL for rt_main\r\n");
			return 1;
		}
		for(i = 0;i < no_layouts;i++)
		{
			eeprom_read_block((void*)(&(rt_main[i])),eepromString+((i*sizeof(RT_MAIN))+layout_offset),sizeof(RT_MAIN));
			strcpy(ramString,rt_main[i].name);
			printString(ramString);
			transmitByte(0x20);
			printString("num_params: ");
			printHexByte(rt_main[i].num_params);
			printString("\r\n");
		}

		for(i = 0;i < no_layouts;i++)
		{
			rt_main[i].ptr_rt_layout = (RT_LAYOUT*)malloc((sizeof(RT_LAYOUT)*(rt_main[i].num_params)));
			if(rt_main[i].ptr_rt_layout == NULL)
			{
				printString("malloc returned NULL for rt_main[i].ptr_rt_layout\r\n");
				return 1;
			}
		}

		// now all the new memory is malloc'd for the rt_layout and rt_main structs 

		for(i = 0;i < no_layouts;i++)
		{
			printHexByte(rt_main[i].offset);
			printHexByte(rt_main[i].offset>>8);
			printString("\r\n");

			for(j = 0;j < rt_main[i].num_params;j++)
			{
				eeprom_read_block((void*)(&(rt_main[i].ptr_rt_layout[j])),\
						eepromString+(rt_main[i].offset)+(j*sizeof(RT_LAYOUT)),\
								sizeof(RT_LAYOUT));

				printHexByte(rt_main[i].ptr_rt_layout[j].row);
				transmitByte(0x20);
				printHexByte(rt_main[i].ptr_rt_layout[j].col);
				transmitByte(0x20);
				printHexByte(rt_main[i].ptr_rt_layout[j].label_list);
				printString("\r\n");
			}	

		}
		
	}
	else
	{
		printString("prompts not set - run eeprom_burn.c to burn eeprom\r\n");
	}
#endif
    while (1) 
    {
#if 0
		for(i = 0;i < 5;i++)
		{
			strcpy(rt_main[i].name,"test\0");
			temp[0] = i+0x30;
			temp[1] = 0;
			strcat(rt_main[i].name,temp);
			rt_main[i].num_params = 5;
			for(j = 0;j < 10;j++)
			{
				rt_main[i].rt_layout[j].row = i+j+1;
				rt_main[i].rt_layout[j].col = i+j+2;
				rt_main[i].rt_layout[j].label_list = i+j+3;
				rt_main[i].rt_layout[j].offset = i+j+4;
			}
		}		
#endif	
		printMenu();
        test1 = receiveByte();
		
		ret_char = (*fptr[current_fptr])(test1, limit8, limit16, cur_row, cur_col);
		if(ret_char == 0xFF)
		{
			hide_menu = 1;
			current_fptr = 0;
		}
		else if(ret_char == 0xFE)
		{
			display_RT_screen(&rt_main[curr_rt_layout]);
		}
#if 0
        switch(test1)
        {
            case 'a':
                printString("displaying disclaimer\r\n");
                GDispClrTxt();
                for(i = 0;i < no_prompts;i++)
                {
                    if(prompts[i].type == DISCLAIMER)
                    {
                        eeprom_read_block(ramString, eepromString+prompts[i].offset,prompts[i].len+1);
                        GDispStringAt(prompts[i].row,prompts[i].col,ramString);
                        printString(ramString);
                        printString("\r\n");
                    }
                }
               printString("\r\ndone displaying disclaimer\r\n");
                break;
            case 'b':
                printString(" displaying menu 1\r\n");
                GDispClrTxt();
                for(i = 0;i < no_prompts;i++)
                {
                    if(prompts[i].type == MENU1)
                    {
                        eeprom_read_block(ramString, eepromString+prompts[i].offset,prompts[i].len+1);
                        GDispStringAt(prompts[i].row,prompts[i].col,ramString);
                        printString(ramString);
                        printString("\r\n");
                    }
                }
				break;
            case 'c':
                printString(" displaying menu 2\r\n");
                GDispClrTxt();
                for(i = 0;i < no_prompts;i++)
                {
                    if(prompts[i].type == MENU2)
                    {
                        eeprom_read_block(ramString, eepromString+prompts[i].offset,prompts[i].len+1);
                        GDispStringAt(prompts[i].row,prompts[i].col,ramString);
                        printString(ramString);
                        printString("\r\n");
                    }
                }
				break;
            case 'd':
                printString(" displaying menu 3\r\n");
                GDispClrTxt();
                for(i = 0;i < no_prompts;i++)
                {
                    if(prompts[i].type == MENU3)
                    {
                        eeprom_read_block(ramString, eepromString+prompts[i].offset,prompts[i].len+1);
                        GDispStringAt(prompts[i].row,prompts[i].col,ramString);
                        printString(ramString);
                        printString("\r\n");
                    }
                }
				break;
			case 'e':
				printString("tesing LCD!\r\n");
				GDispClrTxt();
				k = 0x20;
				for(row = 0;row < ROWS;row++)
				{
					for(col = 0;col < COLUMN;col++)
					{
						dispCharAt(row,col,k);
						_delay_ms(10);
						transmitByte(k);
						if(++k > 0x7e)
							k = 0x20;
					}
				}
				break;
			case 'f':
				GDispClrTxt();
				printString("turning off LCD. Enter '0' to turn back on\r\n");
				GDispStringAt(1,1,"Exiting...");
				_delay_ms(500);
				GDispSetMode(DISPLAY_OFF);
				return (0);
				break;
			case 'g':
                for(i = 0;i < no_prompts;i++)
                {
					printHexByte((uint8_t)prompts[i].pnum);
					transmitByte(0x20);
					printHexByte((uint8_t)prompts[i].row);
					transmitByte(0x20);
					printHexByte((uint8_t)prompts[i].col);
					transmitByte(0x20);
					printHexByte((uint8_t)prompts[i].offset);
					transmitByte(0x20);
					printHexByte((uint8_t)prompts[i].len);
					transmitByte(0x20);
					printHexByte((uint8_t)prompts[i].type);
                    printString("\r\n");
				}
				transmitByte(0x20);
				break;
            case '*':
                col++;
                row++;
                k++;
                CheckRC(&row,&col,&k);
                dispSetCursor(TEXT_ON | CURSOR_BLINK_ON,row,col,LINE_8_CURSOR);
                dispCharAt(row,col,k);
                dispRC(row,col);
				break;
            case 'D':
                col++;
                row++;
                k++;
                CheckRC(&row,&col,&k);
                dispSetCursor(TEXT_ON | CURSOR_BLINK_ON,row,col,LINE_8_CURSOR);
                dispCharAt(row,col,k);
                dispRC(row,col);
				break;
			default:
				break;
        }
    }
#endif	

	}
    return (0);		// this should never happen
}

void dispRC(int row, int col)
{
    printString("col=");
    printByte(col);
    printString("\r\n");
    printString("row=");
    printByte(row);
    printString("\r\n");
    printString("\r\n");
}

void CheckRC(int *row, int *col, UCHAR *k)
{
    if(*row >= ROWS)
        *row = 0;

    if(*col >= COLUMN)
        *col = 0;

    if(*row < 0)
        *row = ROWS-1;

    if(*col < 0)
        *col = COLUMN-1;

    if(*k > 0x50)
        *k = 0x41;
}
void printMenu()
{
	printString("a - display disclaimer\r\n");
	printString("b - display menu 1\r\n");
	printString("d - display menu 2\r\n");
	printString("c - display menu 3\r\n");
	printString("e - test LCD\r\n");
	printString("f - turn off LCD\r\n");
	printString("g - print prompt info\r\n");
}
void update_prompt_struct(uint8_t pnum, UCHAR row, UCHAR col, uint16_t *offset, uint8_t type,char *ramstr)
{
	int len;	
	printString(ramstr);
	len = strlen(ramstr) + 1;
	prompts[pnum].len = len;
	prompts[pnum].pnum = pnum;
	prompts[pnum].row = row;
    prompts[pnum].offset = *offset;
	prompts[pnum].col = col;
    prompts[pnum].type = type;
    eeprom_update_block(ramstr, eepromString+*offset, len);
    *offset += len+1;
}
/*
    printHexByte(pnum);
    transmitByte(0x20);
    printHexByte(row);
    transmitByte(0x20);
    printHexByte(col);
    transmitByte(0x20);
    printHexByte((uint8_t)(offset>>8));
    printHexByte((uint8_t)offset);
    transmitByte(0x20);
    printHexByte((uint8_t)(len>>8));
    printHexByte((uint8_t)len);
    transmitByte(0x20);
    printHexByte(type);
    transmitByte(0x20);
    transmitByte(0x20);
*/
UCHAR default_func(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char;
	switch (ch)
	{
		case 0xE0:
			ret_char = '#';
			break;
		case 0xE1:
			ret_char = '*';
			break;
		case 0xE2:
			ret_char = '0';
			break;
		default:
			ret_char = ch;
			break;
	}
	return ret_char;
			
}

void display_RT_screen( RT_MAIN *main)
{
	int j;
/*	
	printf("%s %d\n",layout->name,layout->num_params);
	for(j= 0;j < 10;j++)
	{
		printf("row:%d col:%d label_list:%d offset:%d\n",\
			layout->rt_layout[j].row,\
			layout->rt_layout[j].col,\
			layout->rt_layout[j].label_list,\
			layout->rt_layout[j].offset);
	}
*/	
	GDispStringAt(0,0,main->name);
	for(j= 0;j < main-> ;j++)
	{
		GDispStringAt(main->rt_layout[j].row, main->rt_layout[j].col,main->rt_layout[j].label_list);
	}
}

UCHAR main_menu_func(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char;
	switch (ch)
	{
		case 0xE0:
			ret_char = '#';
			break;
		case 0xE1:
			ret_char = '*';
			break;
		case 0xE2:
			ret_char = '0';
			break;
		default:
			ret_char = ch;
			break;
	}
	return ret_char;
}
UCHAR menu1a(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char;
	switch (ch)
	{
		case 0xE0:
			ret_char = '#';
			break;
		case 0xE1:
			ret_char = '*';
			break;
		case 0xE2:
			ret_char = '0';
			break;
		default:
			ret_char = ch;
			break;
	}
	return ret_char;
}
UCHAR menu1b(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char;
	switch (ch)
	{
		case 0xE0:
			ret_char = '#';
			break;
		case 0xE1:
			ret_char = '*';
			break;
		case 0xE2:
			ret_char = '0';
			break;
		default:
			ret_char = ch;
			break;
	}
	return ret_char;
}
UCHAR menu1c(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char;
	switch (ch)
	{
		case 0xE0:
			ret_char = '#';
			break;
		case 0xE1:
			ret_char = '*';
			break;
		case 0xE2:
			ret_char = '0';
			break;
		default:
			ret_char = ch;
			break;
	}
	return ret_char;
}
UCHAR menu1d(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char;
	switch (ch)
	{
		case 0xE0:
			ret_char = '#';
			break;
		case 0xE1:
			ret_char = '*';
			break;
		case 0xE2:
			ret_char = '0';
			break;
		default:
			ret_char = ch;
			break;
	}
	return ret_char;
}
UCHAR menu2a(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char;
	switch (ch)
	{
		case 0xE0:
			ret_char = '#';
			break;
		case 0xE1:
			ret_char = '*';
			break;
		case 0xE2:
			ret_char = '0';
			break;
		default:
			ret_char = ch;
			break;
	}
	return ret_char;
}
UCHAR menu2b(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char;
	switch (ch)
	{
		case 0xE0:
			ret_char = '#';
			break;
		case 0xE1:
			ret_char = '*';
			break;
		case 0xE2:
			ret_char = '0';
			break;
		default:
			ret_char = ch;
			break;
	}
	return ret_char;
}
UCHAR menu2c(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char;
	switch (ch)
	{
		case 0xE0:
			ret_char = '#';
			break;
		case 0xE1:
			ret_char = '*';
			break;
		case 0xE2:
			ret_char = '0';
			break;
		default:
			ret_char = ch;
			break;
	}
	return ret_char;
}
UCHAR menu2d(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col)
{
	UCHAR ret_char;
	switch (ch)
	{
		case 0xE0:
			ret_char = '#';
			break;
		case 0xE1:
			ret_char = '*';
			break;
		case 0xE2:
			ret_char = '0';
			break;
		default:
			ret_char = ch;
			break;
	}
	return ret_char;
}
/*
UCHAR menu3a(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col)
{
}
UCHAR menu3b(UCHAR ch, uint8_t limit8, uint16_t limit16, UCHAR row, UCHAR col)
{
}
*/
 
