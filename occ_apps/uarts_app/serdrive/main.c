#include "./inc/SerDrive.h"
#include <fcntl.h>
#include <stdio.h>

int SerFd = -1;
int SerFd_1 = -1;

void PorcessInit(void)
{
	SerFd_1 = open("/dev/ttySAC0",O_RDWR|O_NOCTTY|O_NDELAY);
	if(0 < SerFd_1){
		set_opt(SerFd_1,B115200,DATA_BIT_8,PARITY_NONE,STOP_BIT_1);
	}else{
		printf("open port error \n");
	}


	SerFd = open("/dev/ttySAC1",O_RDWR|O_NOCTTY|O_NDELAY);
	if(0 < SerFd){
		set_opt(SerFd,B115200,DATA_BIT_8,PARITY_NONE,STOP_BIT_1);
	}else{
		printf("open port error \n");
	}
}

int main(int argc,char *argv[])
{
	int nTmp = 0,nTmp_1;
	char Buf[1024];
	char Buf_1[1024];
	PorcessInit();
	send_data_tty(SerFd,"hello serial\n",sizeof("hello serial\n"));
	send_data_tty(SerFd_1,"hello serial\n",sizeof("hello serial\n"));
	while(1){
		nTmp_1 = read_datas_tty(SerFd_1,Buf_1,100,1024);
		if(nTmp){
			printf("%s",Buf_1);
		}

		if(0 < nTmp_1){
			send_data_tty(SerFd_1,Buf_1,nTmp_1);
		}

		nTmp = read_datas_tty(SerFd,Buf,100,1024);
		if(nTmp){
			printf("%s",Buf);
		}

		if(0 < nTmp){
			send_data_tty(SerFd,Buf,nTmp);
		}
	}
}
