// a crude attemp at encrytion of strings
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef unsigned char UCHAR;
int testFunc1(int n, int m);
int testFunc2(int n, int m);
int testFunc3(int n, int m);

typedef struct RT_layout
{
	UCHAR row;
	UCHAR col;
	UCHAR label_list;
} RT_LAYOUT;

RT_LAYOUT rt0 = {4,4,4};
RT_LAYOUT rt1 = {5,5,5};
RT_LAYOUT rt2 = {6,6,6};
RT_LAYOUT rt3 = {7,7,7};
RT_LAYOUT rt4 = {8,8,8};
RT_LAYOUT rt5 = {9,9,9};
RT_LAYOUT rt6 = {10,10,10};
RT_LAYOUT rt7 = {11,11,11};
RT_LAYOUT rt8 = {12,12,12};
RT_LAYOUT rt9 = {13,13,13};
RT_LAYOUT rt10 = {14,14,14};
RT_LAYOUT rt11 = {15,15,15};

RT_LAYOUT *rtarray[12] = {&rt0,&rt1,&rt2,&rt3,&rt4,&rt5,&rt6,&rt7,&rt8,&rt9,&rt10,&rt11};

typedef struct RT_main
{
	char name[10];
	int num_params;
	RT_LAYOUT *ptr_rt_layout;
} RT_MAIN;

void display_RT_screen( RT_MAIN *layout);

int main(int argc, char *argv[])
{
    int i,j;
    UCHAR myStr[20];
    UCHAR myStr2[20];
    UCHAR temp1, temp2;
	char temp[2];
	RT_MAIN *rt_main;
	int no_rtmains = 5;
	RT_LAYOUT t;
	RT_LAYOUT *tptr;
	int show_layout = 0;


    if (argc < 2) 
    {
       fprintf(stderr,"usage %s 0 or 1\n", argv[0]);
       fprintf(stderr,"0 - show pre layout, 1 - show post\n");
       exit(0);
    }
    else
    {
    	show_layout = atoi(argv[1]);
    	if(show_layout)
    		printf("showing post:\n");
    	else
    		printf("showing pre:\n");
    }
    	

	rt_main = (RT_MAIN*)malloc(sizeof(RT_MAIN)*no_rtmains);
	rt_main[0].num_params = 10;
	rt_main[1].num_params = 8;
	rt_main[2].num_params = 6;
	rt_main[3].num_params = 4;
	rt_main[4].num_params = 2;
	for(i = 0;i < no_rtmains;i++)
	{
		rt_main[i].ptr_rt_layout = (RT_LAYOUT*)malloc((sizeof(RT_LAYOUT)*(rt_main[i].num_params)));
	}

	for(i = 0;i < no_rtmains;i++)
	{
		strcpy(rt_main[i].name,"layout\0");
		temp[0] = i+0x30;
		temp[1] = 0;
		strcat(rt_main[i].name,temp);

		for(j = 0;j < rt_main[i].num_params;j++)
		{
			tptr = rt_main[i].ptr_rt_layout;
			// split half of params on either side of screen
			(tptr + j)->row = j<rt_main[i].num_params/2?j:j-rt_main[i].num_params/2;
			(tptr + j)->col = j<rt_main[i].num_params/2?0:15;
			(tptr + j)->label_list = i+j;

		}
	}

	if(show_layout)
	{	
		for(i = 0;i < no_rtmains;i++)
		{
			for(j = 0;j < rt_main[i].num_params;j++)
			{
				memcpy((void*)&rt_main[i].ptr_rt_layout[j],(void*)rtarray[j],sizeof(RT_LAYOUT));
			}
		}	
	}
	for(i = 0;i < no_rtmains;i++)
	{
		printf("\nname:%s num params:%d\n",rt_main[i].name, rt_main[i].num_params);
		tptr = rt_main[i].ptr_rt_layout;
		for(j = 0;j < rt_main[i].num_params;j++)
		{
			printf("%d %d %d\n",rt_main[i].ptr_rt_layout[j].row,\
					rt_main[i].ptr_rt_layout[j].col,rt_main[i].ptr_rt_layout[j].label_list);
		}
	}	
	
	return 0;

	int (*fptr[3])(int,int) = { testFunc1, testFunc2, testFunc3 };

	

/*
	fptr = &testFunc1;
	int sum1 = (*fptr)(2,3);
	fptr = &testFunc2;
	sum1 = (*fptr)(2,3);
*/
	for(i = 0;i < 3;i++)
	{
		(*fptr[i])(i+1,1);
	}


//    test = atoi(argv[1]);
    argv[1][19] = 0;
    memcpy(myStr,argv[1],20);
    printf("string entered:%s\n",myStr);
    for(i = 0;i < 20;i++)
    {
       temp1 = myStr[i];
       printf("%c %x ",(char)temp1,temp1);
       temp2 = temp1;
       temp1 <<= 4;
       temp1 &= 0xF0;
       temp2 >>= 4;
       temp2 &= 0x0F;
//       printf("%x %x\n",temp1,temp2);
       myStr2[i] = temp1 | temp2;
       printf("%x\n",myStr2[i]);
    }
    for(i = 0;i > 20;i++)
    if(myStr2[i])
   return 0;
}
void display_RT_screen( RT_MAIN *layout)
{
#if 0
	int j;
	printf("%s %d\n",layout->name,layout->num_params);
	for(j= 0;j < 10;j++)
	{
		printf("row:%d col:%d label_list:%d offset:%d\n",\
			layout->rt_layout[j].row,\
			layout->rt_layout[j].col,\
			layout->rt_layout[j].label_list,\
			layout->rt_layout[j].offset);
	}
#endif
}

int testFunc1(int n, int m)
{
	int x = n+m;
	printf("testFunc1: %d\n",x);
	return n+m;
}

int testFunc2(int n, int m)
{
	int x = n+m + m;
	printf("testFunc1: %d\n",x);
	return n+m + m;
}

int testFunc3(int n, int m)
{
	int x = n+m + n;
	printf("testFunc1: %d\n",x);
	return n+m + n;
}

