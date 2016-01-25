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

extern UARTS_DATA uart0_data;
extern int com0_fd;
extern int com1_fd;



#if 0
uint8_t save_alarm[2048];
uint8_t *head = save_alarm;
uint8_t *tail = save_alarm;
uint16_t alarm_len = 0;
uint16_t h_cun_len = 0;//从头到当前位置的长度



//循环存放告警的数据 
void alarm_queue(uint8_t *src)
{
	alarm_len = (((src[1] << 8) | src[2]) + 2);

//	if(){//能够放下一帧完整的数据
		memcpy(tail,src,alarm_len);
		tail += alarm_len;
		printf("head = 0x%ld \n",&head);
		printf("tail = 0x%ld \n",&tail);
	
		printf("================\n");
//	}else{//剩下的空间不够存放接受到的数据 
	
//	}

//	send_data_tty(com0_fd,save_alarm,50);

//	printf("src[3] = 0x%x\n",src[3]);
}
#endif



unsigned char alarm_buf[48][14];

unsigned char queue[1024] = {'\0'};
unsigned char *head = queue;
unsigned char *current = queue;
unsigned char *tail = queue;


extern unsigned char up_alarm[2048];
extern unsigned int alarm_num;


void add_data_to_queue(unsigned char *src)
{
	static unsigned int  remain_count = 0; // queue还剩下的元素个数
	static unsigned int  data_len = 0;     // 接受数据的长度
	static unsigned int  toal_len = 0;     // queue总的长度
	static unsigned int  used_count = 0;   // queue已经使用完的长度

	toal_len = sizeof(queue);
	data_len = (((src[1] << 8)|src[2])+2);
	remain_count = (toal_len - used_count);
//	printf("remain_count = %d \n",remain_count);
	if(remain_count >= data_len)//说明queue剩余的长度能一次性存放该数据那么直接拷贝
	{
		memcpy(tail,&src[0],data_len);
		used_count += data_len;
		tail += data_len;
	}
	else
	{
		memcpy(tail,&src[0],remain_count);
		tail = queue;
		memcpy(tail,&src[remain_count],(data_len - remain_count));
		tail += (data_len - remain_count);
		used_count = (data_len - remain_count);
	}
}

unsigned char get_data_from_queue(unsigned char *dst)
{
	static unsigned int  get_remain_count = 0; // queue还剩下的元素个数
	static unsigned int  get_data_len = 0;     // 接受数据的长度
	static unsigned int  get_toal_len = 0;     // queue总的长度
	static unsigned int  get_used_count = 0;   // queue已经使用完的长度 

	get_toal_len = sizeof(queue);

	if(*head == 0x7E){
		get_remain_count = (get_toal_len - get_used_count);
		if(get_remain_count < 3)
		{
			if(get_remain_count == 2)
			{
				get_data_len = (((*(head + 1) << 8) | queue[0]) + 2);
			}
			if(get_remain_count == 1)
			{
				get_data_len = ((((queue[0]) << 8) | queue[1]) + 2);
			}
		}
		else
		{
			get_data_len = (((*(head + 1) << 8) | *(head + 2)) + 2);
		}

	//	printf("get_data_len = %d \n",get_data_len);
	//	printf("get_remain_count = %d \n",get_remain_count);
		if(get_remain_count >= get_data_len)			// 说明能取出一帧完整的数据
		{
			memcpy(&dst[0],head,get_data_len);
			head += get_data_len;
			memset(current,0,get_data_len);
			current = head;
			get_used_count += get_data_len;
	//		printf("11111\n");
		}
		else
		{
			memcpy(&dst[0],head,get_remain_count);
			memset(current,0,get_remain_count);
			head = current = queue;
			memcpy(&dst[get_remain_count],head,(get_data_len - get_remain_count));
			head += (get_data_len - get_remain_count);
			memset(current,0,(get_data_len - get_remain_count));
			current = head;
			get_used_count = (get_data_len - get_remain_count);
	//		printf("22222\n");
		}
		return 1;//正确
	}else{
		return 0;//错误
	}
}

#if 0
void my_printf_down_data(void)
{
	unsigned int i = 0;
	for(i=0;i<10;i++)
	{
		printf("%x",down_data[i]);
	}
	printf("\n");
}

void my_printf_queue(void)
{
	unsigned int i = 0;
	for(i=0;i<40;i++)
	{
		printf("%x",queue[i]);
	}
	printf("\n");
}
#endif

#if 0
int main(int argc,char *argv[])
{
	while(1)
	{
//		printf("add....................\n");
		add_data_to_queue(pool_1);
//		my_printf_down_data();
		my_printf_queue();
//		sleep(2);
//		printf("get....................\n");
		get_data_from_queue(down_data);
//		my_printf_down_data();
//		my_printf_queue();
		sleep(1);
	}
	return 0;
}
#endif




void occ_alram_process(uint8_t *src,uint8_t come_from)
{
	switch(come_from){
		case COME_RS485:

			printf("mobile_xuyao alarm....\n");
			up_alarm[1] = (22 + (4*alarm_num))/256;
			up_alarm[2] = (22 + (4*alarm_num))%256;
			up_alarm[22 + (4*alarm_num)] = crc8(up_alarm,((up_alarm[1]<<8)|up_alarm[2]));
			up_alarm[23 + (4*alarm_num)] = 0x5a;
			send_data_tty(com1_fd,up_alarm,(((up_alarm[1] << 8) | up_alarm[2]) + 2));
			sleep(1);
			alarm_num = 0;

			break;
		case COME_UART0:
			add_data_to_queue(src);
			break;
		default:
			break;
	}
}


































































