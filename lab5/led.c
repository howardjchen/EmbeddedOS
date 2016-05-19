#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"

int main(int argc, char *argv[])
{
	int fd;
	int retval;
 	unsigned short data;

	if( (fd = open("/dev/lcd", O_RDWR)) < 0 )
	{
		printf("Open_/dev/lcd_faild. \n");
		exit(-1);
	}

	data = LED_ALL_ON;
	ioctl(fd, LED_IOCTL_SET	, &data);
	printf("turn on all led lamps\n");
	sleep(3);

	data = LED_ALL_OFF;
	ioctl(fd,LED_IOCTL_SET, &data);
	printf("turn off all led\n");
	sleep(3);

	close(fd);
	return 0;
}

