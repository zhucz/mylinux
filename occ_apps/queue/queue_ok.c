#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#include <pthread.h>

#define MAXSIZE	200

typedef struct queue 
{
	unsigned char data[MAXSIZE];
	int front;
	int rear;
}QUEUE;


QUEUE my_queue;

void InitQueue(QUEUE *q)
{
	q->front = q->rear = 0;
}

void Enqueue(QUEUE *q,unsigned char e)
{
	if(((q->rear + 1)%MAXSIZE) == q->front){
		return;
	}
	q->data[q->rear] = e;
	q->rear = ((q->rear + 1)% MAXSIZE);
}


void Dequeue(QUEUE *q,unsigned char *e)
{
	if(q->rear == q->front){
		return;
	}
	*e = q->data[q->front];
	q->front = ((q->front + 1)% MAXSIZE);
}


int IsEmpty(QUEUE *q)
{
	return q->front == q->rear ?1:0;
}

int GetQuenueLength(QUEUE *q)
{
	return (q->rear - q->front + MAXSIZE)%MAXSIZE;
}

void Clear(QUEUE *q)
{
	q->front = q->rear = 0;
}

void Printqueue(QUEUE *q)
{
	int i = 0;
	if(q->front == q->rear)
		return;
	else if(q->rear < q->front)
	{
		for(i=q->front;i<MAXSIZE;++i)
			printf("0x%x,",q->data[i]);
		for(i=0;i<q->rear;++i)
			printf("0x%x,",q->data[i]);
		printf("\n");
	}
	else
	{
		for(i = q->front;i<q->rear;++i)
			printf("0x%x,",q->data[i]);
		printf("\n");
	}
}



void alram_en_queue(void)
{
	int h = 0;
	int i = 0;

	unsigned char pool_1[7]  = {0x7E,0x00,0x05,0x20,0x00,0x01,0x5A};
	unsigned char pool_2[8]  = {0x7E,0x00,0x06,0x20,0x00,0x00,0x02,0x5A};
	unsigned char pool_3[9]  = {0x7E,0x00,0x07,0x20,0x00,0x00,0x00,0x03,0x5A};
	unsigned char pool_4[10] = {0x7E,0x00,0x08,0x20,0x00,0x00,0x00,0x00,0x04,0x5A};

//	for(;;){
		for(i=0;i<7;i++)
			Enqueue(&my_queue,pool_1[i]);
		printf("第一次入队：");
		Printqueue(&my_queue);


		for(i=0;i<8;i++)
			Enqueue(&my_queue,pool_2[i]);
		printf("第2次入队：");
		Printqueue(&my_queue);

		for(i=0;i<9;i++)
			Enqueue(&my_queue,pool_3[i]);
		printf("第3次入队：");
		Printqueue(&my_queue);


		for(i=0;i<10;i++)
			Enqueue(&my_queue,pool_4[i]);
		printf("第4次入队：");
		Printqueue(&my_queue);
//	}

}


void alram_out_queue(void)
{
	unsigned char ttt[100] = {'\0'};
	int t = 0;
	int i = 0;

	if(IsEmpty(my_queue))
	{
		printf("1111\n");
	}
	else
	{
		t = ((my_queue.data[my_queue.front + 1]<<8)|my_queue.data[my_queue.front + 2]) + 2;
		printf("t1 = %d \n",t);

		for(i=0;i< t;i++)
		Dequeue(&my_queue,ttt + i);
		printf("第一次出队：");
		for(i=0;i<t;i++)
			printf("k[%d] = 0x%x,",i,ttt[i]);
		printf("\n");
	}
}



int main(int argc,char *argv[])
{
	unsigned char i = 0;
	unsigned char k = 0;

	pthread_t en_queue,out_queue;

	int ret;

//	QUEUE my_queue;

	InitQueue(&my_queue);

	ret = pthread_create(&en_queue,NULL,(void *)alram_en_queue,NULL);
	if(ret != 0){
		perror("create pthread  mobile_routine failed !\n");
		return 1;
	}

	ret = pthread_create(&out_queue,NULL,(void *)alram_out_queue,NULL);
	if(ret != 0){
		perror("create pthread  mobile_routine failed !\n");
		return 1;
	}

	while(1);
	
#if 0
/*-----------------------------------------------------------*/
	for(i=0;i<7;i++)
		Enqueue(&my_queue,pool_1[i]);
	printf("第一次入队：");
	Printqueue(&my_queue);

	t = ((my_queue.data[my_queue.front + 1]<<8)|my_queue.data[my_queue.front + 2]) + 2;
	printf("t1 = %d \n",t);

#if 1
	for(i=0;i< t;i++)
		Dequeue(&my_queue,ttt + i);

	printf("第一次出队：");
	for(i=0;i<t;i++)
		printf("k[%d] = 0x%x,",i,ttt[i]);
	printf("\n");

	for(i=0;i<40;i++)
		printf("data[%d] = 0x%x,",i,my_queue.data[i]);
	printf("\n");
/*-----------------------------------------------------------*/
	for(i=0;i<8;i++)
		Enqueue(&my_queue,pool_2[i]);
	printf("第2次入队：");
	Printqueue(&my_queue);

	t = ((my_queue.data[my_queue.front + 1]<<8)|my_queue.data[my_queue.front + 2]) + 2;
	printf("t2 = %d \n",t);
	for(i=0;i< t;i++)
		Dequeue(&my_queue,ttt + i);

	printf("第2次出队：");
	for(i=0;i<t;i++)
		printf("k[%d] = 0x%x,",i,ttt[i]);
	printf("\n");

	for(i=0;i<40;i++)
		printf("data[%d] = 0x%x,",i,my_queue.data[i]);
	printf("\n");
/*-----------------------------------------------------------*/
	for(i=0;i<9;i++)
		Enqueue(&my_queue,pool_3[i]);
	printf("第3次入队：");
	Printqueue(&my_queue);

	t = ((my_queue.data[my_queue.front + 1]<<8)|my_queue.data[my_queue.front + 2]) + 2;
	printf("t3 = %d \n",t);
	for(i=0;i< t;i++)
		Dequeue(&my_queue,ttt + i);

	printf("第3次出队：");
	for(i=0;i<t;i++)
		printf("k[%d] = 0x%x,",i,ttt[i]);
	printf("\n");


	for(i=0;i<40;i++)
		printf("data[%d] = 0x%x,",i,my_queue.data[i]);
	printf("\n");
/*-----------------------------------------------------------*/
	for(i=0;i<10;i++)
		Enqueue(&my_queue,pool_4[i]);
	printf("第4次入队：");
	Printqueue(&my_queue);

	t = ((my_queue.data[my_queue.front + 1]<<8)|my_queue.data[my_queue.front + 2]) + 2;
	printf("t4 = %d \n",t);
	for(i=0;i< t;i++)
		Dequeue(&my_queue,ttt + i);

	printf("第4次出队：");
	for(i=0;i<t;i++)
		printf("k[%d] = 0x%x,",i,ttt[i]);
	printf("\n");

	for(i=0;i<40;i++)
		printf("data[%d] = 0x%x,",i,my_queue.data[i]);
	printf("\n");
#endif
	printf("\n");
	return 0;
#endif
}




