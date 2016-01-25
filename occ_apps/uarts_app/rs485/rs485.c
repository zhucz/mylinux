#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>


//通过修改 drivers/tty/serial/serial_core.c 和
///drivers/tty/serial/samsung.c中加延时 可以是485的发送


static unsigned char tx_buf[10] = {0x7e,0x00,0x06,0x04,0x07,0x5a};
static unsigned char rx_buf[10] = {'\0'};



int main(int argc,char **argv)
{
	int uart485fd = 0;
	
	uart485fd = open("/dev/ttySAC1",O_RDWR);
	if(uart485fd < 0){
		perror("open rs485 device failed \n");
		exit(1);
	}

	printf("open rs485 device is ok \n");

	for(;;){
		write(uart485fd,tx_buf,6);
		sleep(2);
	}

	close(uart485fd);
	return 0;
}

















