#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <pthread.h>




int  run_leds(void)
{
	int led_fd = 0;
	led_fd = open("/dev/leds",O_RDWR);
	if(led_fd < 0){
		printf("open leds file error \n");
		return 0;	
	}
	printf("open leds file ok \n");
	for(;;){
		ioctl(led_fd,0,3);
		sleep(1);
		ioctl(led_fd,1,3);
		sleep(1);
	}
	close(led_fd);
	return 0;
}

int main(int argc,char **argv)
{
	pthread_t id;
	int ret = 0;
	ret = pthread_create(&id,NULL,(void *)run_leds,NULL);
	if(ret != 0){
		printf("pthread_create error \n");
		exit(1);
	}else{
		printf("pthread_create ok \n");
	}
	pthread_join(id,NULL);
}


#if 0
int main(int argc,char **argv)
{
	int led_fd = 0;

	led_fd = open("/dev/leds",O_RDWR);
	if(led_fd < 0){
		printf("open leds file error \n");
		return 0;
	}

	printf("open leds file ok \n");

	for(;;){
		ioctl(led_fd,0,3);
		sleep(1);
		ioctl(led_fd,1,3);
		sleep(1);
	}
	close(led_fd);
	return 0;
}
#endif


