#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <error.h>
#include <unistd.h> //sleep
#include <stdint.h> //uint8_t 
#include <string.h> //memset等

#include <sys/time.h>//定时器
#include <signal.h>

#include <pthread.h>//线程创建，互斥锁创建

#include "./inc/data_resource.h"
#include "./inc/SerDrive.h"
#include "./inc/crc8.h"
#include "./inc/cmd_analysis.h"
#include "./inc/mobile_routine.h"

UARTS_DATA uart0_data;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//pthread_mutex_t mutex_order = PTHREAD_MUTEX_INITIALIZER;


extern int com1_fd;
extern int com0_fd;
extern ORDER_VAL order_val;

volatile unsigned char other_thing_now = 0;

static void not_0x20_send_0xA6to_uart0(uint8_t *src)
{
	uint8_t r_send_0xa6[9] = {0x7E,0x00,0x07,0xA6,0x00,0x00,0x00,0x00,0x5A};
	r_send_0xa6[4] = src[3];
	r_send_0xa6[5] = src[4];
	r_send_0xa6[6] = order_val.occ_current_frame_num;
	r_send_0xa6[7] = crc8(r_send_0xa6,(r_send_0xa6[1] << 8)|r_send_0xa6[2]);
	usleep(500000);
	send_data_tty(com0_fd,r_send_0xa6,((r_send_0xa6[1] << 8)|r_send_0xa6[2])+2);
}


static void pool_info(int signo)
{
	static uint8_t poll_mesg_cmd[7] = {0x7E,0x00,0x05,0x20,0x00,0x00,0x5A};
	static uint8_t i = 1;


	if((order_val.resource_start == 0) && (other_thing_now == 0)){
		poll_mesg_cmd[4] = i;
		poll_mesg_cmd[5] = crc8(poll_mesg_cmd,(((poll_mesg_cmd[1] << 8) | poll_mesg_cmd[2])));
		send_data_tty(com0_fd,poll_mesg_cmd,(((poll_mesg_cmd[1] << 8) | poll_mesg_cmd[2]) + 2));
		order_val.occ_current_frame_num = i;
		i++;
		if(i > 8) i = 1;
	}
}

static void init_sigaction(void)
{
	struct sigaction tact;
	tact.sa_handler = pool_info;
	tact.sa_flags = 0;
	sigemptyset(&tact.sa_mask);
	sigaction(SIGALRM,&tact,NULL);
}


static void init_time(void)
{
	struct itimerval value;

	value.it_value.tv_sec	= 0;
	value.it_value.tv_usec	= 160000;
	value.it_interval = value.it_value;
	setitimer(ITIMER_REAL,&value,NULL);
}



static void uart0_recv_data(void)
{
 	static uint32_t count_uart0 = 0;
	count_uart0 = read_datas_tty(com0_fd,uart0_data.uartx_rxbuf,100,1024);
	
	if(count_uart0 > 4){
		pthread_mutex_lock(&mutex);
		if(uart0_data.uartx_rxbuf[3] == 0x20){

		}else if(uart0_data.uartx_rxbuf[3] == 0xA6){
			memset(uart0_data.uartx_rxbuf,0,sizeof(uart0_data.uartx_rxbuf));
		}else if(uart0_data.uartx_rxbuf[3] == 0xF6){
			memset(uart0_data.uartx_rxbuf,0,sizeof(uart0_data.uartx_rxbuf));
		}else{
			other_thing_now = 1;
			not_0x20_send_0xA6to_uart0(uart0_data.uartx_rxbuf);
			other_thing_now = 0;
		}
		usleep(160000);
		check_uart_x_data(uart0_data.uartx_rxbuf,COME_UART0);
		usleep(160000);
		pthread_mutex_unlock(&mutex);
	}else{
	 
	}
}

void pool_routine(void)
{
	init_sigaction();
	init_time();

	memset(uart0_data.uartx_rxbuf,0,sizeof(uart0_data.uartx_rxbuf));
	memset(uart0_data.uartx_txbuf,0,sizeof(uart0_data.uartx_txbuf));

	while(1){
		if(order_val.resource_start == 0){
			uart0_recv_data();
		}
	}
	exit(0);
}


