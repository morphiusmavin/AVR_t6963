#include "../../avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include "sfr_helper.h"
#include "USART.h"
#include <avr/io.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/eeprom.h>
#include "memdebug.h"
#include "main.h"
 
void shuffle(void **array, int n);
void showmem();
void dotest(UCHAR shuffle);
int freeRam (void);
void check_mem(void); 
int burn_eeprom(void);
int read_eeprom(void);
void update_prompt_struct(UCHAR pnum, UCHAR row, UCHAR col, uint16_t *offset, uint8_t type,char *ramstr);
uint8_t * heapptr, * stackptr;
uint16_t v;
 
// Compatibility stub for undefined pure virtual
 
//extern "C" void __cxa_pure_virtual() { for(;;); }
 
char eepromString[STRING_LEN] EEMEM;
PROMPT_STRUCT prompts[30];

uint8_t promptString[sizeof(PROMPT_STRUCT)];
size_t str_size = sizeof(PROMPT_STRUCT);
uint16_t total_strlen;
UCHAR *test_ptr;
 
int main(void)
{
    uint8_t test1;
	
	int free_ram;
    initUSART();
	printString("\r\n");
	printString("malloc_heap_start = ");
	printHexByte((UCHAR)*__malloc_heap_start);
	printString("\r\n");
	printString("malloc_heap_end = ");
	printHexByte((UCHAR)*__malloc_heap_end);
	printString("\r\n");
/*
 *__malloc_heap_start = 0xA0;
 printString("(new) malloc_heap_start = ");
 printHexByte((UCHAR)*__malloc_heap_start);
 printString("\r\n");
*/
	free_ram = freeRam();
	printString("free ram: ");
	printHexByte(free_ram>>8);
	printHexByte(free_ram);
	printString("\r\n");

	check_mem();
	printHexByte(*heapptr);
	transmitByte(0x20);
	printHexByte(*stackptr);
	printString("\r\n");
	dotest(0);
	dotest(1);
    while (1) 
    {
        test1 = receiveByte();
        switch(test1)
        {
/*        
			case KP_A:
			
				if(burn_eeprom() == 1)
					printString("error in burn_eeprom\r\n");
				break;
*/				
			case KP_B:
				if(read_eeprom() == 1)
					printString("error in read_eeprom\r\n");
				showmem();	
				break;
			default:
				break;	
		}		
 	}
}
 
int freeRam (void) 
{
	extern int __heap_start, *__brkval;
	return (uint16_t) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
} 

void check_mem(void)
{
	stackptr = (uint8_t *)malloc(4);          // use stackptr temporarily
	heapptr = stackptr;                     // save value of heap pointer
	free(stackptr);      // free up the memory again (sets stackptr to 0)
	stackptr =  (uint8_t *)(SP);           // save value of stack pointer
}

void dotest(UCHAR shuffle_)
{
	void *ptrs[20];
	uint16_t i;

	printString("allocating\r\n");
	showmem();
	for(i=0;i<sizeof(ptrs)/sizeof(ptrs[0]);i++)
	{
		ptrs[i]=malloc(20);
		showmem();
	}

	if(shuffle_)
	shuffle(ptrs,sizeof(ptrs)/sizeof(ptrs[0]));

	printString("freeing\r\n");
	for(i=0;i<sizeof(ptrs)/sizeof(ptrs[0]);i++)
	{
		free(ptrs[i]);
		showmem();
	}
}
 
 
void showmem()
{
	char buffer[100];
 
	sprintf(buffer,"%04u %04u %04u : used/free/large",
		getMemoryUsed(),
		getFreeMemory(),
		getLargestAvailableMemoryBlock()
		);
 
	printString(buffer);
	printString("\r\n");
}
 
int rand_int(int n)
{
	int limit = RAND_MAX - RAND_MAX % n;
	int rnd;

	do
	{
		rnd = rand();
	} while (rnd >= limit);
	return rnd % n;
}
 
void shuffle(void **array, int n)
{
	int i, j;
	void *tmp;

	for (i = n - 1; i > 0; i--)
	{
		j = rand_int(i + 1);
		tmp = array[j];
		array[j] = array[i];
		array[i] = tmp;
	}
}
#if 0
// had to comment this out to get malloc to work - too much program or stack space is being used for malloc
// to work - idunno
int burn_eeprom(void)
{
	char temp3[2];
	int i,j;
	uint8_t no_prompts = 0;
	uint16_t prompt_info_offset = 0;
	uint16_t layout_offset = 0;
	uint8_t no_layouts = 0;
	RT_LAYOUT *tptr;
    char ramString[STRING_LEN];
    int row,col;
    UCHAR k;
	uint8_t temp;
	uint16_t temp2;
	
    printString("\r\nwriting to eeprom...\r\n");
    total_strlen = 0;
    i = 0;
    // the row,col elements don't really apply for the RT params
    // because the are displayed according to the row,col elements of the rt_layout
	update_prompt_struct((UCHAR)i,(UCHAR)i,10,&total_strlen,RT_LABEL,"RPM\0");

	i++;
	update_prompt_struct((UCHAR)i,(UCHAR)i,10,&total_strlen,RT_LABEL,"MPH\0");

	i++;
	update_prompt_struct((UCHAR)i,(UCHAR)i,10,&total_strlen,RT_LABEL,"ENG TEMP\0");

	i++;
	update_prompt_struct((UCHAR)i,(UCHAR)i,10,&total_strlen,RT_LABEL,"OIL PRESS\0");

	i++;
	update_prompt_struct((UCHAR)i,(UCHAR)i,10,&total_strlen,RT_LABEL,"OIL TEMP\0");

	i++;
	update_prompt_struct((UCHAR)i,(UCHAR)i,10,&total_strlen,RT_LABEL,"MAP\0");

	i++;
	update_prompt_struct((UCHAR)i,(UCHAR)i,10,&total_strlen,RT_LABEL,"O2\0");

	i++;
	update_prompt_struct((UCHAR)i,(UCHAR)i,10,&total_strlen,RT_LABEL,"AIR TEMP\0");

	i++;
	update_prompt_struct((UCHAR)i,(UCHAR)i,10,&total_strlen,RT_LABEL,"TIME\0");

	i++;
	update_prompt_struct((UCHAR)i,(UCHAR)i,10,&total_strlen,RT_LABEL,"TRIP\0");

	i++;
	// the row, col elements specify where at the bottom of the screen 
	// menu choices will be
	update_prompt_struct((UCHAR)i,15,0,&total_strlen,MENU1,"MENU1a\0");

	i++;
	update_prompt_struct((UCHAR)i,15,8,&total_strlen,MENU1,"MENU2a\0");

	i++;
	update_prompt_struct((UCHAR)i,15,16,&total_strlen,MENU1,"MENU3a\0");

	i++;
	update_prompt_struct((UCHAR)i,15,24,&total_strlen,MENU1,"MENU4a\0");

	i++;
	update_prompt_struct((UCHAR)i,15,0,&total_strlen,MENU2,"MENU1b\0");

	i++;
	update_prompt_struct((UCHAR)i,15,8,&total_strlen,MENU2,"MENU2b\0");

	i++;
	update_prompt_struct((UCHAR)i,15,16,&total_strlen,MENU2,"MENU3b\0");

	i++;
	update_prompt_struct((UCHAR)i,15,24,&total_strlen,MENU2,"MENU4b\0");

	i++;
	update_prompt_struct((UCHAR)i,15,0,&total_strlen,MENU3,"MENU1c\0");

	i++;
	update_prompt_struct((UCHAR)i,15,0,&total_strlen,MENU3,"MENU2c\0");

	i++;
	update_prompt_struct((UCHAR)i,15,0,&total_strlen,MENU3,"MENU3c\0");

	i++;
	update_prompt_struct((UCHAR)i,15,0,&total_strlen,MENU3,"MENU4c\0");

	i++;
	update_prompt_struct((UCHAR)i,15,0,&total_strlen,MENU3,"NUM_ENTRY\0");

    no_prompts = i+1;
    prompt_info_offset = total_strlen;
	printString("\r\n");
	printHexByte((uint8_t)prompt_info_offset>>8);
	transmitByte(0x20);
	printHexByte((uint8_t)prompt_info_offset);
	transmitByte(0x20);
	printHexByte((uint8_t)no_prompts);

	eeprom_update_byte((uint8_t *)NO_PROMPTS_EEPROM_LOCATION,no_prompts);
	eeprom_update_word((uint16_t *)PROMPT_INFO_OFFSET_EEPROM_LOCATION_LSB,prompt_info_offset);

	printString("\r\ndone writing prompts to eeprom\r\n");

    for(i = 0;i < no_prompts;i++)
    {
		// memcpy(dest,src,size)
        memcpy((void*)(promptString),(void *)(&(prompts[i])),str_size);
		// eeprom_block_update(src,dest,size)

        eeprom_update_block(promptString,\
			(eepromString+((i*(uint8_t)str_size))+prompt_info_offset), str_size);
    }

	no_layouts = 5;
	showmem();
	rt_main = (RT_MAIN*)malloc((size_t)(sizeof(RT_MAIN)*no_layouts));
	if(rt_main == NULL)
	{
		printString("malloc returned NULL for rt_main\r\n");
	}
	showmem();
	rt_main[0].num_params = 10;
	rt_main[1].num_params = 8;
	rt_main[2].num_params = 6;
	rt_main[3].num_params = 4;
	rt_main[4].num_params = 2;

	for(i = 0;i < no_layouts;i++)
	{
		rt_main[i].ptr_rt_layout = (RT_LAYOUT*)\
				malloc((sizeof(RT_LAYOUT)*(rt_main[i].num_params)));
		if(rt_main[i].ptr_rt_layout == NULL)
		{
			printString("malloc returned NULL for rt_main[i].ptr_rt_layout\r\n");
			break;
		}
	}
	showmem();

	for(i = 0;i < no_layouts;i++)
	{
		strcpy(rt_main[i].name,"layout\0");
		temp3[0] = i+0x30;
		temp3[1] = 0;
		strcat(rt_main[i].name,temp3);

		for(j = 0;j < rt_main[i].num_params;j++)
		{
			tptr = rt_main[i].ptr_rt_layout;
			// split params to be on either side of screen
			(tptr + j)->row = j<rt_main[i].num_params/2?j:j-rt_main[i].num_params/2;
			(tptr + j)->col = j<rt_main[i].num_params/2?0:15;	// col 15 is the middle of screen
		}
	}		
	// these have to match with where rt_main[n].num_params is assigned above
	rt_main[0].ptr_rt_layout[0].label_list = 0;		// RPM
	rt_main[0].ptr_rt_layout[1].label_list = 1;		// MPH
	rt_main[0].ptr_rt_layout[2].label_list = 2;		// ENGINE TEMP
	rt_main[0].ptr_rt_layout[3].label_list = 3;		// OIL PRESS
	rt_main[0].ptr_rt_layout[4].label_list = 4;		// OIL TEMP
	rt_main[0].ptr_rt_layout[5].label_list = 5;		// MAP 
	rt_main[0].ptr_rt_layout[6].label_list = 6;		// O2
	rt_main[0].ptr_rt_layout[7].label_list = 7;		// AIR TEMP
	rt_main[0].ptr_rt_layout[8].label_list = 8;		// TIME
	rt_main[0].ptr_rt_layout[9].label_list = 9;		// TRIP

	// 2nd type of screen shows everything but MAP & O2
	rt_main[1].ptr_rt_layout[0].label_list = 0;		// RPM	
	rt_main[1].ptr_rt_layout[1].label_list = 1;		// MPH
	rt_main[1].ptr_rt_layout[2].label_list = 2;		// ENGINE TEMP
	rt_main[1].ptr_rt_layout[3].label_list = 3;		// OIL PRESS
	rt_main[1].ptr_rt_layout[4].label_list = 4;		// OIL TEMP
	rt_main[1].ptr_rt_layout[5].label_list = 7;		// AIR TEMP
	rt_main[1].ptr_rt_layout[6].label_list = 8;		// TIME
	rt_main[1].ptr_rt_layout[7].label_list = 9;		// TRIP
	
	// 3rd type of screen shows everything but MAP, O2, OIL TEMP & AIR TEMP
	rt_main[2].ptr_rt_layout[0].label_list = 0;		// RPM
	rt_main[2].ptr_rt_layout[1].label_list = 1;		// MPH
	rt_main[2].ptr_rt_layout[2].label_list = 2;		// ENGINE TEMP
	rt_main[2].ptr_rt_layout[3].label_list = 3;		// OIL PRESS
	rt_main[2].ptr_rt_layout[4].label_list = 8;		// TIME
	rt_main[2].ptr_rt_layout[5].label_list = 9;		// TRIP

	// 4th type shows only these
	rt_main[3].ptr_rt_layout[0].label_list = 1; 	// RPM
	rt_main[3].ptr_rt_layout[1].label_list = 2;		// MPH
	rt_main[3].ptr_rt_layout[2].label_list = 8;		// TIME
	rt_main[3].ptr_rt_layout[3].label_list = 9;		// TRIP

	// 5th type shows only 2
	rt_main[4].ptr_rt_layout[0].label_list = 1;		// RPM
	rt_main[4].ptr_rt_layout[1].label_list = 2;		// MPH

	// the next free memory location in eeprom is going to be:
	// no_prompts*sizeof(PROMPT_STRUCT)+prompt_info_offset
	
	// problem here is that we have to write the RT_MAIN structs
	// and the RT_LAYOUT structs to different locations in eeprom -
	// the pointers in RT_MAIN that point to the RT_LAYOUT structs
	// (ptr_rt_layout) point to what's been malloc'd and __NOT__ 
	// what's in the eeprom !
	// so  first write to eeprom all of the RT_LAYOUT's
	layout_offset = no_prompts*sizeof(PROMPT_STRUCT)+prompt_info_offset;
	printString("layout_offset: ");
	printHexByte((uint8_t)layout_offset>>8);
	printHexByte((uint8_t)layout_offset);
	printString("\r\n");
	// write the rt_layout's to eeprom
	for(i = 0;i < no_layouts;i++)
	{
		// rt_main now has a record of where the rt_layout's are in eeprom
		rt_main[i].offset = layout_offset;
		for(j = 0;j < rt_main[i].num_params;j++)
		{
			// eeprom_update_block(src,dest,size)

			eeprom_update_block((const void*)&rt_main[i].ptr_rt_layout[j],(void*)eepromString+layout_offset,sizeof(RT_LAYOUT));

			layout_offset += sizeof(RT_LAYOUT);
		}	
	}
	printString("layout_offset after writing RT_LAYOUT structs: ");
	printHexByte((uint8_t)layout_offset>>8);
	printHexByte((uint8_t)layout_offset);
	printString("\r\n");
	// record where the rt_layout's stop and the rt_main's start
	eeprom_update_byte((uint8_t *)NO_LAYOUTS_EEPROM_LOCATION,no_layouts);

	// write the rt_main's to eeprom and record at what offset they are
	eeprom_update_word((uint16_t *)LAYOUT_OFFSET_EEPROM_LOCATION_LSB,layout_offset);
	for(i = 0;i < no_layouts;i++)
	{

		eeprom_update_block((const void*)&rt_main[i],(void*)(eepromString+layout_offset),sizeof(RT_MAIN));

		layout_offset += sizeof(RT_MAIN);
	}
	// free up all the malloc'd memory (heap)
	for(i = 0;i < no_layouts;i++)
	{
		for(j = 0;j < rt_main[i].num_params;j++)
		{
			free((void*)&(rt_main[i].ptr_rt_layout[j]));
		}
		free((void*)&(rt_main[i]));
	}
	printString("done writing prompts structs and screen layout to eeprom\r\n");
	showmem();
	return 0;
}
#endif
int read_eeprom(void)
{
	int i,j;
	uint8_t no_prompts = 0;
	uint16_t prompt_info_offset = 0;
	uint16_t layout_offset = 0;
	uint8_t no_layouts = 0;
	PROMPT_STRUCT *prompt_ptr = NULL;
    char ramString[STRING_LEN];
    int row,col;
    UCHAR k;
	uint8_t temp;
	uint16_t temp2;
	
	printString("reading prompt data into prompt structs\r\n");
	no_prompts = eeprom_read_byte((uint8_t*)NO_PROMPTS_EEPROM_LOCATION);
	no_layouts = eeprom_read_byte((uint8_t*)NO_LAYOUTS_EEPROM_LOCATION);
	printString("no_prompts: ");
	printHexByte(no_prompts);
	printString("\r\n");
	prompt_info_offset = (uint16_t)eeprom_read_byte((uint8_t*)PROMPT_INFO_OFFSET_EEPROM_LOCATION_LSB);	// read lsb
	temp = eeprom_read_byte((uint8_t*)PROMPT_INFO_OFFSET_EEPROM_LOCATION_MSB);		// read msb
	temp2 = (uint16_t)temp;
	prompt_info_offset |= (temp2 << 8);		// shift msb over and bit-or with lsb
	printString("prompt_info_offset: ");
	printHexByte((uint8_t)(prompt_info_offset>>8));
	printHexByte((uint8_t)prompt_info_offset);
	printString("\r\n");
	printString("no_layouts: ");
	printHexByte(no_layouts);
	layout_offset = (uint16_t)eeprom_read_byte((uint8_t*)LAYOUT_OFFSET_EEPROM_LOCATION_LSB);
	temp = eeprom_read_byte((uint8_t*)LAYOUT_OFFSET_EEPROM_LOCATION_MSB);
	temp2 = (uint16_t)temp;
	layout_offset |= (temp2 <<= 8);
	printString("\r\nlayout_offset: ");
	printHexByte((uint8_t)(layout_offset>>8));
	printHexByte((uint8_t)layout_offset);
	printString("\r\n");
/*
	for(i = 0;i < no_prompts;i++)
	{
		eeprom_read_block((void*)(&(prompts[i])),(eepromString+(i*str_size)+prompt_info_offset), str_size);
	}
*/
	showmem();
	rt_main = (RT_MAIN*)malloc((size_t)(sizeof(RT_MAIN)*no_layouts));
	if(rt_main == NULL)
	{
		printString("malloc returned NULL for rt_main\r\n");
		return 1;
	}
	showmem();
	printString("rt_main is malloc'd\r\n");
	prompt_ptr = (PROMPT_STRUCT*)malloc(sizeof(PROMPT_STRUCT)*no_prompts);
//					prompt_ptr = (UCHAR*)malloc((size_t)296);

	if(prompt_ptr == NULL)
	{
		printString("malloc returned NULL for prompt_ptr\r\n");
		return 1;
	}
	printString("prompt_ptr is malloc'd\r\n");

	eeprom_read_block(prompt_ptr, eepromString+prompt_info_offset, sizeof(PROMPT_STRUCT)*no_prompts);
	// now display what's been read into the heap (prompt_ptr)
	for(i = 0;i < no_prompts;i++)
	{
		printHexByte((uint8_t)prompt_ptr[i].pnum);
		transmitByte(0x20);
		printHexByte((uint8_t)prompt_ptr[i].row);
		transmitByte(0x20);
		printHexByte((uint8_t)prompt_ptr[i].col);
		transmitByte(0x20);
		printHexByte((uint8_t)(prompt_ptr[i].offset>>8));
		printHexByte((uint8_t)(prompt_ptr[i].offset));
		transmitByte(0x20);
		printHexByte((uint8_t)prompt_ptr[i].len);
		transmitByte(0x20);
		printHexByte((uint8_t)prompt_ptr[i].type);
		printString("\r\n");
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
		printString("offset: ");
		printHexByte((uint8_t)(rt_main[i].offset>>8));
		printHexByte((uint8_t)(rt_main[i].offset));
		printString("\r\n");

		for(j = 0;j < rt_main[i].num_params;j++)
		{
/*
			eeprom_read_block((void*)(&(rt_main[i].ptr_rt_layout[j])),\
					eepromString+(rt_main[i].offset)+(j*sizeof(RT_LAYOUT)),\
							sizeof(RT_LAYOUT));
*/
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
	printString("done\r\n");
	showmem();
	return 0;
}

void update_prompt_struct(UCHAR pnum, UCHAR row, UCHAR col, uint16_t *offset, uint8_t type,char *ramstr)
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

    *offset += len;
}

