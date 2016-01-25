#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <strings.h>
#include <signal.h>

#define SERV_PORT 60000


uint8_t kepalive[9] = {0x7E,0x00,0x07,0xA6,0x00,0x00,0x00,0x00,0x5A};

int sockfd = 0;

unsigned char  dm9000ae_send(unsigned char *src,unsigned int len)
{
#if 1
	struct sockaddr_in servaddr;
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd == -1){
		printf("socket error \n");
		return 0;
	}
	bzero(&servaddr,sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET,"192.168.30.43",&servaddr.sin_addr);//服务器的IP地址 
#endif
	if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) == -1){
		printf("connect error \n");
		return 0;
	}
	write(sockfd,src,len);
	close(sockfd);
	return 0;
}


void net_routine(void)
{
	signal(SIGPIPE,SIG_IGN);
	for(;;)
	{
		dm9000ae_send(kepalive,9);
		sleep(4);
	}
}


