#include <stdio.h>   
#include <sys/types.h>   
#include <sys/ipc.h>   
#include <errno.h>   
#include <stdlib.h>

struct nand{
	int x;
	int (*func)(int, int);
	void (*erase)(void);
};

int add(int a, int b)
{
	return (a+b);
}

void nand_erase(void)
{
	printf("nand_erase !!!!\n");
	return ;
}

int add_1(int a, int b)
{
	return (b-a);
}

int main(int argc,char *argv[])
{
	struct nand *nand_info;

	nand_info = malloc(sizeof(nand_info));
	printf("malloc ok address = %x \n",nand_info);
	nand_info->func = add;
	printf("nand_info->func = %d \n",nand_info->func(4,5));

	nand_info->x = 13;
	printf("nand_info->x = %d \n",nand_info->x);

	nand_info->erase = nand_erase; 
	nand_info->erase();


	nand_info->func = add_1;
	printf("nand_info->func 2= %d \n",nand_info->func(4,5));



	free(nand_info);

	return 0;
}

