#ifndef SERDRIVE_H
#define SERDRIVE_H

#define TTY0_NAME "/dev/ttySAC0"
#define TTY1_NAME "/dev/ttySAC1"
#define TTY2_NAME "/dev/ttySAC2"


#define TTYS0	1
#define TTYS1	2
#define TTYS2	3 

#define B2400	2400
#define B4800	4800
#define B9600	9600
#define B115200	115200
#define B460800	460800



#define PARITY_ODD	'O'
#define PARITY_EVEN	'E'
#define PARITY_NONE	'N'

#define STOP_BIT_1	1
#define STOP_BIT_2	2

#define DATA_BIT_7	7
#define DATA_BIT_8	8

int open_port(int fd,int comport);
int set_opt(int fd,int nSpeed,int nBits,char nEvent,int nStop);

int read_datas_tty(int fd,char *rcv_buf,int TimeOut,int Len);
int send_data_tty(int fd,char *send_buf,int Len);


#endif
