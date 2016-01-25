#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>



static unsigned char tx_buf[10] = {0x7e,0x00,0x06,0x04,0x07,0x5a};
static unsigned char rx_buf[10] = {'\0'};



int main(int argc,char **argv)
{
	int uart0fd = 0;
	
	uart0fd = open("/dev/ttySAC0",O_RDWR);
	if(uart0fd < 0){
		perror("open uart0 device failed \n");
		exit(1);
	}

	printf("open uart0 device is ok \n");

	for(;;){
		write(uart0fd,tx_buf,6);
		sleep(2);
	}

	close(uart0fd);
	return 0;
}
