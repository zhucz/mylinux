#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>



int global = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *thread_function(void *arg)
{
	int j = 0;;

	for(j=0;j<5;j++){
		pthread_mutex_lock(&mutex);
		global +=1;
		printf("global += %d\n",global);
		sleep(1);
		pthread_mutex_unlock(&mutex);
	}
}

void *thread_function_1(void *arg)
{
	int j = 0;;

	for(j=0;j<5;j++){
		pthread_mutex_lock(&mutex);
		global -=1;
		printf("global -= %d\n",global);
		sleep(1);
		pthread_mutex_unlock(&mutex);
	}
}


int main(void)
{
	pthread_t th_new,th_new_1;
	int i = 0;

	if(pthread_create(&th_new,NULL,thread_function,NULL)){
		perror("error creating thread \n");
		abort();
	}


	if(pthread_create(&th_new_1,NULL,thread_function_1,NULL)){
		perror("error creating thread \n");
		abort();
	}


	while(1);
	exit(0);
}
















