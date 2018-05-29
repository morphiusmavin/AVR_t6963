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
#define LEN 100
#define TIME_DELAY 1000000

typedef unsigned char UCHAR;
typedef unsigned int UINT;


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
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

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
	UCHAR pwm_on;
	UCHAR pwm_off;
	UCHAR high_delay;
	UCHAR ch, ch2;
	struct termios oldtio,newtio;
	UCHAR buf[LEN];
	memset(&newtio, 0, sizeof newtio);

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
	ch = '*';

	for(i = 0;i < 92;i++)
		printf("*");
	printf("\n");

	res2 = 0;
	ch = 0x21;
	j = 1;
	i = 0;
	ch2 = 0xfe;
//	for(j = 0;j < 300;j++)

	pwm_off = 0;
	pwm_on = 10;
	high_delay = 0xFB;
	while(1)
	{
		ch = 9;
		res2 = write(fd,(void*)&ch,1);
		usleep(2000);

		ch = (UCHAR)(pwm_on);
		res2 = write(fd,(void*)&ch,1);
		usleep(2000);

		ch = (UCHAR)(pwm_off);
		res2 = write(fd,(void*)&ch,1);
		usleep(2000);

		ch = (UCHAR)(high_delay);
		res2 = write(fd,(void*)&ch,1);
		usleep(2000);

		pwm_off++;
		pwm_on--;
//		high_delay++;

		if(pwm_on < 2)
		{
			pwm_off = 0;
			pwm_on = 10;
//			high_delay = 0xEA;
		}

		res2 = write(fd,(void*)&ch2,1);
		usleep(2000);

		usleep(TIME_DELAY);
		printf("on: %2d off: %2d\n",pwm_on,pwm_off);
	}
	ch = 0xff;
	write(fd,(void *)&ch,1);
	tcsetattr(fd,TCSANOW,&oldtio);
	close(fd);
	printf("\ndone\n");
	exit(0);
}

