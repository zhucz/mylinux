#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <error.h>
#include <fcntl.h>
#include <unistd.h> //sleep
#include <stdint.h> //uint8_t 
#include <string.h> //memset等

#include "./inc/data_resource.h"
#include "./inc/SerDrive.h"
#include "./inc/crc8.h"
#include "./inc/order.h"
#include "./inc/queue.h"


extern int com1_fd;
extern int com0_fd;
extern UARTS_DATA uart485_data;
extern ORDER_VAL order_val;

DEV_INFO board_info = {{'\0'},{'\0'},0x00,0x00,0x00,0x00};

/*==============================================
 *
 * 函数名称:static void occ_read_device_info_cmd(void)
 * 函数功能:
 * 入口参数:
 * 出口参数:
 * 修改日期:2015-12-08
 * 修改作者:zhuchengzhi
 * 
 * ============================================*/
static void occ_read_device_info_cmd(void)
{
	int uuid_fd = 0,cfg_fd = 0,size = 0;
	
	uuid_fd = open("/yaffs/uuid.bin",O_RDONLY);
	if(uuid_fd < 0){
		printf("open file error \n");
	}
	size = read(uuid_fd,board_info.uuid,16);
	close(uuid_fd);

	cfg_fd = open("/yaffs/dev_config.bin",O_RDONLY);
	if(cfg_fd < 0){
		printf("open file error \n");
	}
	size = read(cfg_fd,board_info.name,104);
	close(cfg_fd);


	memcpy(&uart485_data.uartx_txbuf[5],&board_info.uuid[0],16);
	memcpy(&uart485_data.uartx_txbuf[21],&board_info.name[1],103);

	uart485_data.uartx_txbuf[0] = 0x7E;
	uart485_data.uartx_txbuf[1] = 0x00;
	uart485_data.uartx_txbuf[2] = 0x7C;
	uart485_data.uartx_txbuf[3] = 0x01;
	uart485_data.uartx_txbuf[4] = board_info.name[0];
	uart485_data.uartx_txbuf[124] = crc8(uart485_data.uartx_txbuf,(uart485_data.uartx_txbuf[1] << 8) | uart485_data.uartx_txbuf[2]);
	uart485_data.uartx_txbuf[125] = 0x5A;
	send_data_tty(com1_fd,uart485_data.uartx_txbuf,(((uart485_data.uartx_txbuf[1] << 8) | uart485_data.uartx_txbuf[2]) + 2));
}


/*==============================================
 *
 * 函数名称:static void occ_write_device_config_info(uint8_t *src)
 * 函数功能:
 * 入口参数:
 * 出口参数:
 * 修改日期:2015-12-04
 * 修改作者:zhuchengzhi
 * 
 * ============================================*/
static void occ_write_device_config_info(uint8_t *src)
{
	int uuid_fd = 0,size = 0,ret = 0;
	int cfg_fd = 0;
	uint8_t  buffer[200];

	uuid_fd = open("/yaffs/uuid.bin",O_RDONLY);
	if(uuid_fd < 0){
		printf("open file error \n");
	}
	size = read(uuid_fd,&uart485_data.uartx_txbuf[4],16);
	close(uuid_fd);

	cfg_fd = open("/yaffs/dev_config.bin",O_RDWR | O_CREAT);
	if(uuid_fd < 0){
		printf("open file error \n");
	}
	write(cfg_fd,&src[4],104);
	close(cfg_fd);

	cfg_fd = open("/yaffs/dev_config.bin",O_RDONLY);
	if(cfg_fd < 0){
		printf("open file error \n");
	}
	size = read(uuid_fd,buffer,104);
	close(cfg_fd);

	ret = memcmp(buffer,&src[4],104);
	if(!ret){
		uart485_data.uartx_txbuf[20]  = 0x00;
	}else{
		uart485_data.uartx_txbuf[20]  = 0x01;
	}

	uart485_data.uartx_txbuf[0]  = 0x7E;
	uart485_data.uartx_txbuf[1]  = 0x00;
	uart485_data.uartx_txbuf[2]  = 0x15;
	uart485_data.uartx_txbuf[3]  = 0x0c;
	uart485_data.uartx_txbuf[21] = crc8(uart485_data.uartx_txbuf,(((uart485_data.uartx_txbuf[1] << 8) | uart485_data.uartx_txbuf[2])));
	uart485_data.uartx_txbuf[22] = 0x5A;
	send_data_tty(com1_fd,uart485_data.uartx_txbuf,(((uart485_data.uartx_txbuf[1] << 8) | uart485_data.uartx_txbuf[2]) + 2));
			
}


/*==============================================
 *
 * 函数名称:static void occ_write_device_uuid_info(uint8_t *src)
 * 函数功能:
 * 入口参数:
 * 出口参数:
 * 修改日期:2015-12-04
 * 修改作者:zhuchengzhi
 * 
 * ============================================*/
static void occ_write_device_uuid_info(uint8_t *src)
{
	int uuid_fd = 0 ,size = 0,ret = 0;
	uint8_t  buffer[80];
	
	uuid_fd = open("/yaffs/uuid.bin",O_RDWR | O_CREAT);
	if(uuid_fd < 0){
		printf("open file error \n");
	}
	write(uuid_fd,&src[4],16);
	close(uuid_fd);

	uuid_fd = open("/yaffs/uuid.bin",O_RDONLY);
	if(uuid_fd < 0){
		printf("open file error \n");
	}
	size = read(uuid_fd,buffer,sizeof(buffer));
	close(uuid_fd);


	ret = memcmp(buffer,&src[4],16);
	if(!ret){
		uart485_data.uartx_txbuf[4]  = 0x00;
	}else{
		uart485_data.uartx_txbuf[4]  = 0x01;
	}
	uart485_data.uartx_txbuf[0]  = 0x7E;
	uart485_data.uartx_txbuf[1]  = 0x00;
	uart485_data.uartx_txbuf[2]  = 0x05;
	uart485_data.uartx_txbuf[3]  = 0x14;
	uart485_data.uartx_txbuf[5] = crc8(uart485_data.uartx_txbuf,(((uart485_data.uartx_txbuf[1] << 8) | uart485_data.uartx_txbuf[2])));
	uart485_data.uartx_txbuf[6]  = 0x5A;
	send_data_tty(com1_fd,uart485_data.uartx_txbuf,(((uart485_data.uartx_txbuf[1] << 8) | uart485_data.uartx_txbuf[2]) + 2));
}
 
/*==============================================
 *
 * 函数名称:void occ_main_order_data_processing(uint8_t *src)
 * 函数功能:
 * 入口参数:
 * 出口参数:
 * 修改日期:2015-12-04
 * 修改作者:zhuchengzhi
 * 
 * ============================================*/
void occ_main_order_data_processing(uint8_t *src,uint8_t come_from)
{
	uint8_t cmd_major = 0;
//	uint8_t cmd_minor = 0;

	cmd_major = src[3];

	switch(cmd_major){
		case READ_DEVICE_CONFIG_INFO:        //读取设备的配置信息 0x01
			occ_read_device_info_cmd();
			break;
		case READ_DEVICE_FRAME_INFO:		 //读取机框信息       0x02 
			break;
		case  READ_TRAY_INFO:				 //读取盘信息         0x03 
			break;
		case READ_HW_SW_INFO:			     //读取软硬件版本号   0x05 
			if(come_from == COME_RS485){
			
			}

			if(come_from == COME_UART0){
			
			}
			break;
		case SOFTWARE_UPDATA:	             //软件升级 		  0x06 
			break;
		case WRITE_ELECTORN_LABLE:			 //待写入电子标签信息 0x07 
			break;
		case READ_DEVICE_ALARM:				 //读取设备告警信息   0x09 
			occ_alram_process(src,come_from);
			break;
		case ALARM_CLEAN:					 //告警确认和清除     0x0a 
			break;
		case WRITE_DEVICE_CONFIG_INFO:		 //写入设备配置信息   0x0C 
			occ_write_device_config_info(src);
			break;
		case WRITE_DEVICE_UUID:				 //写入设备UUID       0x14 
			occ_write_device_uuid_info(src);
			break;
		case ORDERS_CMD:					 //工单总命令         0x0D 
			processing_order_cmd(src,come_from);
			break;
		case CONTROL_WRITE_ELECTORN_LABLE: 	 //受控写入电子标签   0x0F 
			break;
		case TEST_LED:						 //LED灯测试          0x10 
			break;
		case CHANGE_TRAY:		             //更换托盘           0x13
			break;
		case POOL_CMD:						 //巡检命令           0x20
			order_val.act_frame_ing |= (1 << (src[4] - 1));
			break;
		default:
			break;
	}
}



