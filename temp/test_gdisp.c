
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
#define TIME_DELAY 1000
#define ROWS 16
#define COLUMN 40
#define CHAR_AT_CMD		0
#define STRING_AT_CMD		1
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
	unsigned char ch, ch2, ch3;
	struct termios oldtio,newtio;
	unsigned char buf[LEN];
	char test_str[20];
	memset(&newtio, 0, sizeof newtio);
	unsigned char row, col;

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
	ch = '*';
	j = 1;
	for(j = 0;j < 92;j++)
		printf("*");
	printf("\n");
	j = 0;
//	printf("\n\nwaiting for chars...\n");
	for(i = 0;i < LEN;i++)
	{
		buf[i] = ch;
		if(++ch > 0x7e)
			ch = 0x21;
	}
//	for(i = 0;i < LEN;i++)
//		write(fd,&buf[i],1);
	i = 0;
	ch = 0x21;
	ch3 = 0xfe;

	ch2 = DEBUG_CLRSCR3;
	write(fd,&ch2,1);
	usleep(TIME_DELAY);

	write(fd,&ch3,1);
	usleep(TIME_DELAY*1000);

	ch2 = GOTO_CMD;
	write(fd,&ch2,1);
	usleep(TIME_DELAY);

	ch = 12;	// row
	write(fd,&ch,1);
	usleep(TIME_DELAY);

	ch = 20;	// col
	write(fd,&ch,1);
	usleep(TIME_DELAY);

	write(fd,&ch3,1);
	usleep(TIME_DELAY);

	ch2 = CHAR_CMD;
	write(fd,&ch2,1);
	usleep(TIME_DELAY);
	ch = 0x41;
	write(fd,&ch,1);
	usleep(TIME_DELAY);
	write(fd,&ch3,1);

	usleep(TIME_DELAY*1000);
	ch2 = GOTO_CMD;
	write(fd,&ch2,1);
	usleep(TIME_DELAY);

	ch = 12;	// row
	write(fd,&ch,1);
	usleep(TIME_DELAY);

	ch = 21;	// col
	write(fd,&ch,1);
	usleep(TIME_DELAY);

	write(fd,&ch3,1);
	usleep(TIME_DELAY);

	ch2 = CHAR_CMD;
	write(fd,&ch2,1);
	usleep(TIME_DELAY);
	ch = 0x42;
	write(fd,&ch,1);
	usleep(TIME_DELAY);
	write(fd,&ch3,1);

	usleep(TIME_DELAY*1000);

	ch2 = CHAR_AT_CMD;
	ch = 0x21;	// start with char '!'
	row = col = 0;
	while(1)
	{
		write(fd,&ch2,1);
		usleep(TIME_DELAY);
		write(fd,&row,1);
		usleep(TIME_DELAY);
		write(fd,&col,1);
		usleep(TIME_DELAY);
		write(fd,&ch,1);
		usleep(TIME_DELAY);
		write(fd,&ch3,1);

		if(++ch > 0x7e)
			ch = 0x21;

		if(++col > COLUMN-1)
		{
			col = 0;
			if(++row > ROWS-1)
				row = 0;
		}
		usleep(TIME_DELAY*10);
		printf("%c",ch);
		if(++j > 94)
		{
			j = 0;
			printf("\n");
		}
	}
	ch = 0xff;
	write(fd,(void *)&ch,1);
	tcsetattr(fd,TCSANOW,&oldtio);
	close(fd);
	printf("\ndone\n");
	exit(0);
}

