#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <error.h>
#include <unistd.h>

void key_routine(void)
{
	printf("my name is key_routine \n");
	sleep(1);
}
