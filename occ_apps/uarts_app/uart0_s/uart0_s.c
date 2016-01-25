#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>



//static unsigned char tx_buf[10] = {0x7e,0x00,0x06,0x04,0x07,0x5a};
//static unsigned char rx_buf[10] = {'\0'};



static int open_port(int fd,int comport)
{
	int fd;
	char *dev[]={"/dev/ttySAC0","/dev/ttySAC1"};
	if(comport == 1){
		fd = open("/dev/ttySAC0",O_RDWR|O_NOCTTY|O_NDELAY);
		if(fd < 0){
			perror("Can't Open Serial Port \n");
			return -1;
		}
	}else {
		fd = open("/dev/ttySAC1",O_RDWR|O_NOCTTY|O_NDELAY);
		if(fd < 0){
			perror("Can't Open Serial Port \n");
			return -1;
		}
	}
	return 0;
}

static int set_opt(int fd,int nSpeed,int nBits,char nEvent,int nStop)
{
	struct termios newtio,oldtio;
	if(tcgetattr(fd,&oldtio) != 0){
		perror("Setup serial 1");
		return -1;
	}
	bzero(&newtio,sizeof(newtio));
	newtio.c_cflag |= CLOCAL | CREAD;
	newtio.c_cflag &= ~CSIZE;
	switch(nBits){
		case 7:
			newtio.c_cflag |= CS7;
			break;
		case 8:
			newtio.c_cflag |= CS8;
			break;
	}

	switch(nEvent){
		case 'O':
			newtio.c_cflag |= PARENB;
			newtio.c_cflag |= PARODD;
			newtio.c_iflag |= (INPCK | STRIP);
			break;
		case 'E':
			newtio.c_iflag |= (INPCK | STRIP);
			newtio.c_cflag |= PARENB;
			newtio.c_cflag &= ~PARODD;
			break;
		case 'N':
			newtio.c_cflag &= ~PARENB;
			break;
	}

	switch(nSpeed){
		case 2400:
			cfsetispeed(&newtio,B2400);
			cfsetospeed(&newtio,B2400);
			break;
		case 4800:
			cfsetispeed(&newtio,B4800);
			cfsetospeed(&newtio,B4800);
			break;


		case 9600:
			cfsetispeed(&newtio,B9600);
			cfsetospeed(&newtio,B9600);
			break;

		case 115200:
			cfsetispeed(&newtio,B115200);
			cfsetospeed(&newtio,B115200);
			break;
	}

	if(nStop == 1){
		newtio.c_cflag &= ~CSTOPB;
	}else if(nSpeed == 2){
		newtio.c_cflag |= CSTOPB;
	}

	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VIMIN] = 0;
	
	tcflush(fd,TCIFLUSH);

	if(tcsetattr(fd,TCSANOW,&newtio) != 0){
		perror("com set error \n");
		return -1;
	}
	printf("set done \n");

	return 0;

}


int main(int argc,char **argv)
{
	int fd;
	int nwrite,i;
	char buff[]="hello\n";
	if((fd = open_port(fd,1)) < 0){
		perror("open error");
		return -1;
	}

	if( (i = set_opt(fd,115200,8,'N',1)) < 0){
		perror("set error \n");
		return -1;
	}

	printf("fd = %d \n",fd);

	for(i=0;i<2;i++){
		printf("nwrite=%d,%s\n",nwrite,buff);
		sleep(1);
	}
	close(fd);
	return 0;
}
