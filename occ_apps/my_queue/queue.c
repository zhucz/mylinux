#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char queue[40] = {'\0'};

unsigned char *head = queue;
unsigned char *current = queue;
unsigned char *tail = queue;



unsigned char pool_1[7]  = {0x7E,0x00,0x05,0x20,0x00,0x01,0x5A};
unsigned char down_data[10] = {'\0'};


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

void get_data_from_queue(unsigned char *dst)
{
	static unsigned int  get_remain_count = 0; // queue还剩下的元素个数
	static unsigned int  get_data_len = 0;     // 接受数据的长度
	static unsigned int  get_toal_len = 0;     // queue总的长度
	static unsigned int  get_used_count = 0;   // queue已经使用完的长度 

	get_toal_len = sizeof(queue);

	get_remain_count = (get_toal_len - get_used_count);
	if(get_remain_count < 3)
	{
		if(get_remain_count == 2)
		{
			get_data_len = ((*(head + 1) << 8) | (queue[0]) + 2);
		}
		if(get_remain_count == 1)
		{
			get_data_len = (((queue[0]) << 8) | (queue[1]) + 2);
		}
	}
	else
	{
		get_data_len = ((*(head + 1) << 8) | *(head + 2) + 2);
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
//		if(get_used_count == )
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
}

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



