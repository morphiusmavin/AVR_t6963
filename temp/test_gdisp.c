// test_gdisp.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/time.h>

typedef unsigned char UCHAR;
typedef unsigned int UINT;

typedef struct dimmer
{
	UCHAR on;
	UCHAR off;
	UCHAR delay;
}DIMMER;

//#define BAUDRATE B9600
#define BAUDRATE B19200
//#define BAUDRATE B115200
#ifdef MAKE_TARGET
#define MODEMDEVICE "/dev/ttyAM1"
#warning "MAKE_TARGET defined......."
#else
#define MODEMDEVICE "/dev/ttyS0"
#endif
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define LEN 200
#define TIME_DELAY 200
#define ROWS 16
#define COLUMN 40
#define CHAR_AT_CMD		0
#define STRING_AT_CMD		1
// these are the only 2 commands we need to write to the screen
// CHAR_CMD sends a char and the firmware will advance the cursor
// otherwise send GOTO_CMD to set a new cursor postion
#define CHAR_CMD		2
#define GOTO_CMD		3
#define SET_MODE_CMD 		4
#define DEBUG_CLRSCR1 		5
#define DEBUG_CLRSCR2 		6
#define DEBUG_CLRSCR3 		7
#define DEBUG_MSG1 		8
#define SET_PWM_CMD		9

int set_interface_attribs (int fd, int speed, int parity);
void set_blocking (int fd, int should_block);

//volatile int STOP=FALSE;

int set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                printf ("error %d from tcgetattr", errno);
				perror(" ");
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 1;
        tty.c_cc[VTIME] = 20;

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                printf ("error %d from tcsetattr", errno);
				perror(" ");
                return -1;
        }
        return 0;
}

void set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                printf ("error %d from tggetattr", errno);
                perror(" ");
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 50;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                printf ("error %d setting term attributes", errno);
}


int main(void)
{
	int fd,c, res, res2;
	int i,j;
	int ret;
	UCHAR ch, ch2, ch3;
	struct termios oldtio,newtio;
	UCHAR buf[LEN];
	char test_str[20];
	memset(&newtio, 0, sizeof newtio);
	UCHAR row, col;
	UCHAR pwm_on;
	UCHAR pwm_off;
	UCHAR high_delay;
	DIMMER dim[20];
	int dim_ptr;

	for(i = 0;i < 10;i++)
	{
		dim[i].on  = 10-i;
		dim[i].off = 1;
		dim[i].delay = 0xF5-i;
		printf("%2d: on: %2d off: %2d delay %2x\n",i,dim[i].on,dim[i].off,dim[i].delay);
	}

	for(i = 10;i < 20;i++)
	{
		dim[i].on  = 2;
		dim[i].off = i-9;
		dim[i].delay = 0xF0+i-4;
//		dim[i].delay = 0xFC;
		printf("%2d: off: %2d on: %2d delay %2x\n",i,dim[i].off,dim[i].on,dim[i].delay);
	}

	fd = open (MODEMDEVICE, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd <0) {perror(MODEMDEVICE);
		exit(-1); }

#ifdef MAKE_TARGET
	printf("\nopened ttyAM1\n");
#else
	printf("\nopened ttyS0\n");
#endif

	if(tcgetattr(fd,&oldtio) != 0) /* save current port settings */
	{
		printf("Error from tcgetattr: %d ",errno);
		close(fd);
		exit(1);
	}

	set_interface_attribs (fd, BAUDRATE, 0);
	set_blocking (fd, 1);

	res2 = 0;
	j = 0;
	j = 0;
	i = 0;
	ch = 0x21;
	ch3 = 0xfe;

	ch = SET_PWM_CMD;
	res2 = write(fd,(void*)&ch,1);
	usleep(TIME_DELAY);

//	ch = dim[dim_ptr].on;
	ch = 0;
	res2 = write(fd,(void*)&ch,1);
	usleep(TIME_DELAY);

//	ch = dim[dim_ptr].off;
	res2 = write(fd,(void*)&ch,1);
	usleep(TIME_DELAY);

//	ch = dim[dim_ptr].delay;
	ch = 0x10;
	res2 = write(fd,(void*)&ch,1);
	usleep(TIME_DELAY);

	res2 = write(fd,(void*)&ch3,1);
	usleep(TIME_DELAY);

	ch2 = DEBUG_CLRSCR3;
	write(fd,&ch2,1);
	usleep(TIME_DELAY);

	write(fd,&ch3,1);
	usleep(TIME_DELAY*300);

	ch2 = GOTO_CMD;
	write(fd,&ch2,1);
	usleep(TIME_DELAY);

	ch = 0;
	write(fd,&ch,1);
	usleep(TIME_DELAY);

	write(fd,&ch,1);
	usleep(TIME_DELAY);

	write(fd,&ch3,1);
	usleep(TIME_DELAY*300);


	ch2 = CHAR_CMD;
	ch = 0x21;	// start with char '!'
	row = col = 0;
	dim_ptr = 0;

	while(1)
	{
		write(fd,&ch2,1);
		usleep(TIME_DELAY);

		write(fd,&ch,1);
		usleep(TIME_DELAY);
		write(fd,&ch3,1);

		if(++ch > 0x7e)
			ch = 0x21;

		usleep(TIME_DELAY);

		if(++j > 1000)
		{
			j = 0;
			ch = SET_PWM_CMD;
			res2 = write(fd,(void*)&ch,1);
			usleep(TIME_DELAY);

			ch = dim[dim_ptr].on;
			res2 = write(fd,(void*)&ch,1);
			usleep(TIME_DELAY);

			ch = dim[dim_ptr].off;
			res2 = write(fd,(void*)&ch,1);
			usleep(TIME_DELAY);

			ch = dim[dim_ptr].delay;
			res2 = write(fd,(void*)&ch,1);
			usleep(TIME_DELAY);

			res2 = write(fd,(void*)&ch3,1);

			usleep(TIME_DELAY);
			printf("dim_ptr: %d\n",dim_ptr);

			if(++dim_ptr > 19)
				dim_ptr = 0;
		}

	}
	ch = 0xff;
	write(fd,(void *)&ch,1);
	tcsetattr(fd,TCSANOW,&oldtio);
	close(fd);
	printf("\ndone\n");
	exit(0);
}

