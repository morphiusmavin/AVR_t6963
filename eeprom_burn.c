#include <avr/io.h>
#include "../avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include "sfr_helper.h"
#include <avr/eeprom.h>
#include "USART.h"
#include "t6963.h"
#include "macros.h"
#include "main.h"
#include <string.h>
#include <stdlib.h>
void update_prompt_struct(UCHAR pnum, UCHAR row, UCHAR col, uint16_t *offset, uint8_t type,char *ramstr);
void printMenu(void);

PROMPT_STRUCT prompts[30];
							
int main(void) 
{
    uint8_t test1;
    char ramString[STRING_LEN];
	int i,j;
    int row,col;
    UCHAR k;
	uint8_t temp;
	uint16_t temp2;
	
	RT_LAYOUT *tptr;
	char temp3[2];
	UCHAR rt_main_is_mallocd = 0;
	UCHAR prompt_is_mallocd = 0;

	uint8_t no_prompts = 0;
	uint16_t prompt_info_offset = 0;
	uint16_t layout_offset = 0;
	uint8_t no_layouts = 0;

    uint8_t promptString[sizeof(PROMPT_STRUCT)];
    size_t str_size = sizeof(PROMPT_STRUCT);
    uint16_t total_strlen;
	prompt_ptr = NULL;
	rt_main = NULL;

    initUSART();

	GDispInit();
	_delay_us(10);
	GDispSetMode(XOR_MODE);
	_delay_us(10);
	GDispSetMode(TEXT_ON);
	_delay_us(10);
	GDispClrTxt();    
	printMenu();
    no_prompts = eeprom_read_byte((uint8_t*)NO_PROMPTS_EEPROM_LOCATION);

	if(no_prompts == 0xff)
		printString("no_prompts is blank\r\n");
	if(no_layouts == 0xff)
		printString("no_layouts is blank\r\n");
		
    while (1) 
    {
//		printMenu();
        test1 = receiveByte();
        switch(test1)
        {
            case 'a':
#if 1		
#if 1		
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

                no_prompts = i+1;
                prompt_info_offset = total_strlen;
				printString("\r\n");
				printHexByte((uint8_t)prompt_info_offset>>8);
				transmitByte(0x20);
				printHexByte((uint8_t)prompt_info_offset);
				transmitByte(0x20);
				printHexByte((uint8_t)no_prompts);
#ifndef SKIP_EEPROM
				eeprom_update_byte((uint8_t *)NO_PROMPTS_EEPROM_LOCATION,no_prompts);
				eeprom_update_word((uint16_t *)PROMPT_INFO_OFFSET_EEPROM_LOCATION_LSB,prompt_info_offset);
#endif
				printString("\r\ndone writing prompts to eeprom\r\n");

                for(i = 0;i < no_prompts;i++)
                {
					// memcpy(dest,src,size)
                    memcpy((void*)(promptString),(void *)(&(prompts[i])),str_size);
					// eeprom_block_update(src,dest,size)
#ifndef SKIP_EEPROM
                    eeprom_update_block(promptString,\
						(eepromString+((i*(uint8_t)str_size))+prompt_info_offset), str_size);
#endif
                }
#endif
				no_layouts = 5;
				rt_main = (RT_MAIN*)malloc(sizeof(RT_MAIN)*no_layouts);
				if(rt_main == NULL)
				{
					printString("malloc returned NULL for rt_main\r\n");
					break;
				}
				rt_main_is_mallocd = 1;
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
#if 1				
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
#endif
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
#ifndef SKIP_EEPROM
						eeprom_update_block((const void*)&rt_main[i].ptr_rt_layout[j],(void*)eepromString+layout_offset,sizeof(RT_LAYOUT));
#endif
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
#ifndef SKIP_EEPROM
					eeprom_update_block((const void*)&rt_main[i],(void*)(eepromString+layout_offset),sizeof(RT_MAIN));
#endif
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
                break;
			case 'b':	
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
				if(rt_main_is_mallocd == 0)
				{
					rt_main = (RT_MAIN*)malloc(sizeof(RT_MAIN)*no_layouts);
					if(rt_main == NULL)
					{
						printString("malloc returned NULL for rt_main\r\n");
						return 1;
					}
					rt_main_is_mallocd = 1;
					printString("rt_main is malloc'd\r\n");
				}
				if(prompt_is_mallocd == 0)
				{
					prompt_ptr = (PROMPT_STRUCT*)malloc(sizeof(PROMPT_STRUCT)*no_prompts);
//					prompt_ptr = (UCHAR*)malloc((size_t)296);

					if(prompt_ptr == NULL)
					{
						printString("malloc returned NULL for prompt_ptr\r\n");
						break;
					}
					printString("prompt_ptr is malloc'd\r\n");
					prompt_is_mallocd = 1;
				}	

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
				break;
            case 'c':
#if 1			
                printString("displaying RT labels\r\n");
                GDispClrTxt();
                for(i = 0;i < no_prompts;i++)
                {
                    if(prompts[i].type == RT_LABEL)
                    {
                        eeprom_read_block(ramString, eepromString+prompts[i].offset,prompts[i].len+1);
                        GDispStringAt(prompts[i].row,prompts[i].col,ramString);

                        printString(ramString);
                        printString("\r\n");
                    }
                }
//               printString("\r\ndone displaying RT labels\r\n");
#endif			   
                break;
            case 'd':
#if 1			
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
#endif				
				break;
            case 'e':
#if 1			
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
#endif				
#endif				
				break;
            case 'f':
#if 1			
                printString(" displaying menu 3\r\n");
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
#endif				
				break;
			case 'g':
				GDispClrTxt();
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
				break;
			case 'h':
				printMenu();
				break;
			case 'i':
#if 1		
				if(no_prompts == 0xff)
					printString("no_prompts is blank\r\n");
				else
				{
					printString("displaying prompt data currently in ram...\r\n");
/*
					for(i = 0;i < no_prompts;i++)
					{
						printHexByte((uint8_t)prompts[i].pnum);
						transmitByte(0x20);
						printHexByte((uint8_t)prompts[i].row);
						transmitByte(0x20);
						printHexByte((uint8_t)prompts[i].col);
						transmitByte(0x20);
						printHexByte((uint8_t)(prompts[i].offset>>8));
						printHexByte((uint8_t)(prompts[i].offset));
						transmitByte(0x20);
						printHexByte((uint8_t)prompts[i].len);
						transmitByte(0x20);
						printHexByte((uint8_t)prompts[i].type);
						printString("\r\n");
					}
*/
					printHexByte((uint8_t)(prompt_info_offset>>8));
					printHexByte((uint8_t)prompt_info_offset);
					printString("\r\n");
					layout_offset = (uint16_t)eeprom_read_byte((uint8_t*)LAYOUT_OFFSET_EEPROM_LOCATION_LSB);
					temp = eeprom_read_byte((uint8_t*)LAYOUT_OFFSET_EEPROM_LOCATION_MSB);
					temp2 = (uint16_t)temp;
					layout_offset |= (temp2 <<= 8);
					printHexByte((uint8_t)(layout_offset>>8));
					printHexByte((uint8_t)layout_offset);
					printString("\r\n");

					temp2 = (uint16_t)(layout_offset - prompt_info_offset);
//					prompt_ptr = (void*)malloc((size_t)(layout_offset-prompt_info_offset));
					if(prompt_is_mallocd == 0)
					{
						prompt_ptr = (PROMPT_STRUCT*)malloc(sizeof(PROMPT_STRUCT)*no_prompts);
	//					prompt_ptr = (UCHAR*)malloc((size_t)296);

						if(prompt_ptr == NULL)
						{
							printString("malloc returned NULL for prompt_ptr\r\n");
							printString("size = ");
							printHexByte((uint8_t)temp2>>8);
							printHexByte((uint8_t)temp2);
							printString("\r\n");
							break;
						}
						prompt_is_mallocd = 1;
					}	
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
//						memcpy((void*)ramString,(prompt_ptr[i].offset),(void*)prompt_ptr[i].len+1);
//						printString(ramString);
	/*
						transmitByte(0x20);
						for(j = 0;j < prompts[i].len;j++)
						{
							printHexByte((UCHAR)(*(ramString+j)));
							transmitByte(0x20);
						}
	*/
					}
	//				free(prompt_ptr);
				}
#endif				
				break;
			case 'j':
				printString("displaying layout info...\r\n");

				no_layouts = eeprom_read_byte((uint8_t*)NO_LAYOUTS_EEPROM_LOCATION);
				if(no_layouts == 0xff)
					printString("no_layouts is blank\r\n");
				else
				{
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

					if(rt_main_is_mallocd == 0)
					{
						rt_main = (RT_MAIN*)malloc(sizeof(RT_MAIN)*no_layouts);
						if(rt_main == NULL)
						{
							printString("malloc returned NULL for rt_main\r\n");
							return 1;
						}
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
						if(rt_main_is_mallocd == 0)
						{
							rt_main[i].ptr_rt_layout = (RT_LAYOUT*)malloc((sizeof(RT_LAYOUT)*(rt_main[i].num_params)));
							if(rt_main[i].ptr_rt_layout == NULL)
							{
								printString("malloc returned NULL for rt_main[i].ptr_rt_layout\r\n");
								return 1;
							}
							rt_main_is_mallocd = 1;
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
				}
				break;
#if 1				
			case '1':
				GDispClrTxt();    
				break;
			case '2':
				printString("tesing pattern 1\r\n");
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
				printString("done\r\n");
				break;
			case '3':
				printString("tesing pattern 2\r\n");
				GDispClrTxt();
				k = 0x30;
				for(row = 0;row < ROWS;row++)
				{
					for(col = 0;col < COLUMN;col++)
					{
						dispCharAt(row,col,k);
						_delay_ms(10);
						transmitByte(k);
						if(++k > 0x39)
							k = 0x30;
					}
				}
				printString("done\r\n");
				break;
			case '4':	
				printString("tesing pattern 3\r\n");
				GDispClrTxt();
				k = 0x41;
				for(row = 0;row < ROWS;row++)
				{
					for(col = 0;col < COLUMN;col++)
					{
						dispCharAt(row,col,k);
						_delay_ms(10);
						transmitByte(k);
						if(++k > 0x5A)
							k = 0x41;
					}
				}
				printString("done\r\n");
				break;
			case '5':	
				printString("tesing pattern 4\r\n");
				GDispClrTxt();
				k = 0x61;
				for(row = 0;row < ROWS;row++)
				{
					for(col = 0;col < COLUMN;col++)
					{
						dispCharAt(row,col,k);
						_delay_ms(10);
						transmitByte(k);
						if(++k > 0x7A)
							k = 0x61;
					}
				}
				printString("done\r\n");
				break;
			case '6':
				printString("tesing pattern 1 (fast)\r\n");
				GDispClrTxt();
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
				printString("done\r\n");
				break;
			case '7':
				printString("tesing pattern 2 (fast)\r\n");
				GDispClrTxt();
				k = 0x30;
				for(row = 0;row < ROWS;row++)
				{
					for(col = 0;col < COLUMN;col++)
					{
						dispCharAt(row,col,k);
						if(++k > 0x39)
							k = 0x30;
					}
				}
				printString("done\r\n");
				break;
			case '8':	
				printString("tesing pattern 3 (fast)\r\n");
				GDispClrTxt();
				k = 0x41;
				for(row = 0;row < ROWS;row++)
				{
					for(col = 0;col < COLUMN;col++)
					{
						dispCharAt(row,col,k);
						if(++k > 0x5A)
							k = 0x41;
					}
				}
				printString("done\r\n");
				break;
			case '9':	
				printString("tesing pattern 4 (fast)\r\n");
				GDispClrTxt();
				k = 0x61;
				for(row = 0;row < ROWS;row++)
				{
					for(col = 0;col < COLUMN;col++)
					{
						dispCharAt(row,col,k);
						if(++k > 0x7A)
							k = 0x61;
					}
				}
				printString("done\r\n");
				break;
            case '*':
                row++;
                k++;
                CheckRC(&row,&col,&k);
                dispSetCursor(TEXT_ON | CURSOR_BLINK_ON,row,col,LINE_8_CURSOR);
                dispCharAt(row,col,k);
                dispRC(row,col);
				break;
            case '0':
                row--;
                k++;
                CheckRC(&row,&col,&k);
                dispSetCursor(TEXT_ON | CURSOR_BLINK_ON,row,col,LINE_8_CURSOR);
                dispCharAt(row,col,k);
                dispRC(row,col);
				break;
            case '#':
                col++;
                k++;
                CheckRC(&row,&col,&k);
                dispSetCursor(TEXT_ON | CURSOR_BLINK_ON,row,col,LINE_8_CURSOR);
                dispCharAt(row,col,k);
                dispRC(row,col);
				break;
            case 'D':
                col--;
                k++;
                CheckRC(&row,&col,&k);
                dispSetCursor(TEXT_ON | CURSOR_BLINK_ON,row,col,LINE_8_CURSOR);
                dispCharAt(row,col,k);
                dispRC(row,col);
				break;
			default:
				break;
        }
#endif		
    }
    return (0);
}
#if 1
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


void printMenu(void)
{
	printString("\r\na - burn eeprom\r\n");
	printString("b - read data from eeprom into ram\r\n");
	printString("c - display rt labels\r\n");
	printString("d - display menu 1\r\n");
	printString("e - display menu 2\r\n");
	printString("f - display menu 3\r\n");
	printString("g - test LCD\r\n");
	printString("h - help (menu)\r\n");
	printString("i - display prompt data currently in ram\r\n");
	printString("j - print layout info\r\n");
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
#ifndef SKIP_EEPROM
    eeprom_update_block(ramstr, eepromString+*offset, len);
#endif
    *offset += len;
}
/*
	for(i = 0;i < no_layouts;i++)
	{
		strcpy(rt_main[i].name,"layout\0");
		temp[0] = i+0x30;
		temp[1] = 0;
		strcat(rt_main[i].name,temp);
		rt_main[i].num_params = 5;
		for(j = 0;j < 10;j++)
		{
			rt_main[i].rt_layout[j].row = j<5?j:j-5;
			rt_main[i].rt_layout[j].col = j<5?0:16;
			rt_main[i].rt_layout[j].label_list = j;
			rt_main[i].rt_layout[j].offset = j;
		}
	}
	eeprom_update_block(rt_main[0],(eepromString+(prompt_info_offset+(8*str_size))),sizeof(RT_MAIN));
*/

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
#endif
