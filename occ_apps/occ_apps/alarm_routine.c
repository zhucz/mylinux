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
#include "./inc/queue.h"


extern ORDER_VAL order_val;
extern DEV_INFO board_info;
extern UARTS_DATA uart0_data;
extern int com0_fd;
extern int com1_fd;
extern uint8_t other_thing_now;

unsigned char down_data[1024] = {'\0'};
unsigned char up_alarm[2048] = {'\0'};
extern unsigned char alarm_buf[48][14];

unsigned int alarm_num = 0;//告警条数


static void clean_alarm_buf(void)
{
	unsigned char i = 0;	 // 48行
	unsigned char j = 0;	 // 14列 

	for(i=0;i<48;i++){
		for(j=0;j<14;j++){
			alarm_buf[i][j] = 0;
		}
	}
}


void up_load_alarm_to_mobile(void)
{
	unsigned char i = 0;	 // 48行
	unsigned char j = 0;	 // 14列
	unsigned char frame = 0; // 框号
	unsigned char tray = 0;  // 盘号
	unsigned char port = 0;  // 端口号
	unsigned char vstat = 0; // 状态


	for(i=0;i<48;i++){
		for(j=0;j<14;j++){
			if(alarm_buf[i][j] != 0){
				frame = ((i/6)+1);
				tray  = ((i%6)+1);
				port  = (j+1);
				vstat = alarm_buf[i][j];

				up_alarm[22 +(4*alarm_num)] = frame;
				if(port == 13){
					up_alarm[23 +(4*alarm_num)] = tray;
					up_alarm[24 +(4*alarm_num)] = 0x00;
				}else if(port == 14){
					up_alarm[23 +(4*alarm_num)] = 0x00;
					up_alarm[24 +(4*alarm_num)] = 0x00;
				}else{
					up_alarm[23 +(4*alarm_num)] = tray;
					up_alarm[24 +(4*alarm_num)] = port;
				}
				up_alarm[25 +(4*alarm_num)] = vstat;
				alarm_num += 1;
				up_alarm[4] = alarm_num;
				//return 1;//至少有一条
			}
		}
	}

	if(alarm_num == 0){
	
	}else{
		printf("alarm_num = %d \n",alarm_num);
		up_alarm[1] = (22 + (4*alarm_num))/256;
		up_alarm[2] = (22 + (4*alarm_num))%256;
		up_alarm[22 + (4*alarm_num)] = crc8(up_alarm,((up_alarm[1]<<8)|up_alarm[2]));
		up_alarm[23 + (4*alarm_num)] = 0x5a;
			
		if((order_val.resource_start == 0) && (other_thing_now == 0)){
			send_data_tty(com1_fd,up_alarm,(((up_alarm[1] << 8) | up_alarm[2]) + 2));
		}
	//	sleep(1);
		clean_alarm_buf();
		alarm_num = 0;
	}
//	return 0;//没有一个告警
}


void alarm_routine(void)
{
	unsigned char f_index = 0;// 框
	unsigned char t_index = 0;// 盘
	unsigned char p_index = 0;// 口
	unsigned char s_val = 0;  // 状态
	unsigned char len = 0;    // 有几个告警
	unsigned char index = 0;  //alarm_buf 中的下标
	unsigned int  i = 0;

	up_alarm[0]  = 0x7E;
	up_alarm[1]  = 0x00;  //告警长度高字节
	up_alarm[2]  = 0x00;  //告警长度低字节
	up_alarm[3]  = 0x09;  //告警命令子 
	up_alarm[4]  = 0x00;  //告警条数
	memcpy(&up_alarm[5],&board_info.uuid[0],16);
	up_alarm[21] = 0x02;  //OCC设备类型

	for(;;){
		if(get_data_from_queue(down_data)){
			len = ((((down_data[1] << 8)|down_data[2]) - 4)/4);
			for(i = 0;i < len;i++){
				f_index = down_data[4+(4*i)];
				t_index = down_data[5+(4*i)];
				p_index = down_data[6+(4*i)];
				s_val	= down_data[7+(4*i)];
				index = (((f_index - 1)*6)+(t_index - 1));
				if(p_index != 0x00){
					alarm_buf[index][p_index - 1] = s_val;
				}else{
					alarm_buf[index][12] = s_val;
				}
			//	printf(" alarm_buf[%x][%x] = 0x%x \n",index,(p_index - 1),alarm_buf[index][p_index - 1]);
			}
		}
		memset(down_data,0,sizeof(down_data));

		up_load_alarm_to_mobile();		
	}
}


