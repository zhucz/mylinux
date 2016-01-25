#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <error.h>
#include <unistd.h> //sleep
#include <stdint.h> //uint8_t 
#include <string.h> //memset等

#include "./inc/data_resource.h"
#include "./inc/SerDrive.h"
#include "./inc/crc8.h"
#include "./inc/cmd_analysis.h"

UARTS_DATA uart485_data;

extern int com1_fd;
extern int com0_fd;
extern pthread_mutex_t mutex;


/*==============================================
 *
 * 函数名称:void check_uart_x_data(uint8_t *src)
 * 函数功能:
 * 入口参数:
 * 出口参数:
 * 修改日期:2015-12-04
 * 修改作者:zhuchengzhi
 * 
 * ============================================*/
void check_uart_x_data(uint8_t *src,uint8_t come_from)
{
	uint8_t check_data = 0;
	check_data = OCC_Use_CRC8_Check_Data(src);
	if((check_data == 1) && (src[0] == 0x7e)){
		occ_main_order_data_processing(src,come_from);
	}else{
		printf("data error \n");
	}
}

/*==============================================
 *
 * 函数名称:void mobile_routine(void)
 * 函数功能:
 * 入口参数:
 * 出口参数:
 * 修改日期:2015-12-04
 * 修改作者:zhuchengzhi
 * 
 * ============================================*/
void mobile_routine(void)
{
	static uint32_t count_485 = 0;
	memset(uart485_data.uartx_rxbuf,0,sizeof(uart485_data.uartx_rxbuf));
	memset(uart485_data.uartx_txbuf,0,sizeof(uart485_data.uartx_txbuf));

	for(;;){
		count_485 = read_datas_tty(com1_fd,uart485_data.uartx_rxbuf,100,1024);
		if(count_485 > 6){//通过CRC分析数据是否正确 
			pthread_mutex_lock(&mutex);
			check_uart_x_data(uart485_data.uartx_rxbuf,COME_RS485);
			pthread_mutex_unlock(&mutex);
		}else{
		
		}
	}
}



