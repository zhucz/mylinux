#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#define MAXSIZE	20

typedef struct queue 
{
	unsigned char data[MAXSIZE];
	int front;
	int rear;
}QUEUE;


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


int main(int argc,char *argv[])
{
	unsigned char i = 0;
	unsigned char k = 0;
	unsigned char pool_1[7] = {0x7E,0x00,0x05,0x20,0x00,0x01,0x5A};
	unsigned char pool_2[7] = {0x7E,0x00,0x05,0x20,0x00,0x02,0x5A};
	unsigned char pool_3[7] = {0x7E,0x00,0x05,0x20,0x00,0x03,0x5A};
	unsigned char pool_4[7] = {0x7E,0x00,0x05,0x20,0x00,0x04,0x5A};

	unsigned char ttt[8] = {'\0'};

	unsigned char t = 0;

	QUEUE my_queue;

	InitQueue(&my_queue);
	for(i=0;i<7;i++)
		Enqueue(&my_queue,pool_1[i]);
	printf("pool_1 : ");
	Printqueue(&my_queue);

	for(i=0;i<7;i++)
		Enqueue(&my_queue,pool_2[i]);
	printf("pool_2 : ");
	Printqueue(&my_queue);


	printf("out kkk1 : ");
	for(i=0;i<7;i++)
		Dequeue(&my_queue,ttt + i);
	
	for(i=0;i<7;i++)
		printf("k[%d] = 0x%x,",i,ttt[i]);
	printf("\n");


	printf("pool_3 : ");
	for(i=0;i<7;i++)
		Enqueue(&my_queue,pool_3[i]);
	Printqueue(&my_queue);

	printf("pool_4 : ");
	for(i=0;i<7;i++)
		Enqueue(&my_queue,pool_4[i]);
	Printqueue(&my_queue);


	printf("out data : ");
	for(i=0;i<20;i++)
		printf("data[%d] = 0x%x,",i,my_queue.data[i]);
	printf("\n");

	printf("out kkk2 : ");

	t = ((my_queue.data[1]<<8)|my_queue.data[2]) + 2;
	printf("t = %d \n",t);

	for(i=0;i< t;i++)
		Dequeue(&my_queue,ttt + i);

	Printqueue(&my_queue);
	printf("out kkk3 : ");
	for(i=0;i<7;i++)
		Dequeue(&my_queue,ttt + i);

	Printqueue(&my_queue);
	printf("out kkk4 : ");
	for(i=0;i<7;i++)
		Dequeue(&my_queue,ttt + i);

	Printqueue(&my_queue);

	printf("out kkk5 : ");
	for(i=0;i<7;i++)
		Dequeue(&my_queue,ttt + i);

	Printqueue(&my_queue);

	if(IsEmpty(&my_queue)){
		printf("1111\n");
	}else{
		printf("222\n");
	}

	printf("\n");
	return 0;
}




