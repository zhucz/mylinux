#include "./inc/SerDrive.h"
#include "./inc/data_resource.h"

#include "./inc/alarm_routine.h"
#include "./inc/key_routine.h"
#include "./inc/mobile_routine.h"
#include "./inc/pool_routine.h"
#include "./inc/network.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pthread.h>


int com0_fd = -1;
int com1_fd = -1;

extern DEV_INFO board_info;


void Serial_Init(void)
{

	com0_fd = open("/dev/ttySAC0",O_RDWR|O_NOCTTY|O_NDELAY);
	if(0 < com0_fd){
		set_opt(com0_fd,B115200,DATA_BIT_8,PARITY_NONE,STOP_BIT_1);
		printf("open /dev/ttySAC0 is ok \n");
	}else{
		printf("open /dev/ttySAC0 error \n");
	}

	com1_fd = open("/dev/ttySAC1",O_RDWR|O_NOCTTY|O_NDELAY);
	if(0 < com1_fd){
		set_opt(com1_fd,B115200,DATA_BIT_8,PARITY_NONE,STOP_BIT_1);
		printf("open /dev/ttySAC1 is ok \n");
	}else{
		printf("open /dev/ttySAC1 error \n");
	}

}

int main(int argc,char *argv[])
{
	pthread_t pool_id,mobile_id,alarm_id,key_id,net_id;

	int ret;

	int uuid_fd = 0,cfg_fd = 0,size = 0;

	uuid_fd = open("/yaffs/uuid.bin",O_RDONLY);
	if(uuid_fd < 0){
		printf("open uuid file error \n");
	}
	size = read(uuid_fd,board_info.uuid,16);
	close(uuid_fd);

	cfg_fd = open("/yaffs/dev_config.bin",O_RDONLY);
	if(cfg_fd < 0){
		printf("open cfg file error \n");
	}
	size = read(cfg_fd,board_info.name,104);
	close(cfg_fd);


	Serial_Init();

/*==================创建手机线程======================*/
	ret = pthread_create(&mobile_id,NULL,(void *)mobile_routine,NULL);
	if(ret != 0){
		perror("create pthread  mobile_routine failed !\n");
		return 1;
	}
/*==================创建巡检线程======================*/
	ret = pthread_create(&pool_id,NULL,(void *)pool_routine,NULL);
	if(ret != 0){
		perror("create pthread  pool_routine failed !\n");
		return 1;
	}


/*==================创建告警线程=====================*/
	ret = pthread_create(&alarm_id,NULL,(void *)alarm_routine,NULL);
	if(ret != 0){
		perror("create pthread  alarm_routine failed !\n");
		return 1;
	}

/*==================创建按键线程=====================*/
	ret = pthread_create(&key_id,NULL,(void *)key_routine,NULL);
	if(ret != 0){
		perror("create pthread  key_routine failed !\n");
		return 1;
	}

/*==================创建网络线程=====================*/
	ret = pthread_create(&net_id,NULL,(void *)net_routine,NULL);
	if(ret != 0){
		perror("create pthread  key_routine failed !\n");
		return 1;
	}

	while(1);
	return 0;
#if 0
/*----------------------------------------------------------*/
/*==================等待手机线程结束=================*/
	ret= pthread_join(mobile_id,NULL);
	if(ret != 0){
		perror("joining mobile_id thread error !\n");
		return 1;
	}

/*==================等待巡检线程结束================*/
	ret= pthread_join(pool_id,NULL); 
	if(ret != 0){
		perror("joining pool_id thread error !\n");
		return 1;
	}

/*==================等待告警线程结束================*/
	ret= pthread_join(alarm_id,NULL);
	if(ret != 0){
		perror("joining alarm_id thread error !\n");
		return 1;
	}

/*==================等待按键线程结束================*/
	ret= pthread_join(key_id,NULL);
	if(ret != 0){
		perror("joining key_id thread error !\n");
		return 1;
	}
#endif

}



