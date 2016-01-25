#ifndef _DATA_RESOURCE_H
#define _DATA_RESOURCE_H

#include <stdint.h>
#include <pthread.h>


#define MAX_UARTX_RX_BUF	1024
#define MAX_UARTX_TX_BUF	1024 
#define MAX_FRAME			8
#define MAX_RESOURCE_LEN	2490

typedef struct _uarts_data {
	uint8_t uartx_rxbuf[MAX_UARTX_RX_BUF];
	uint8_t uartx_txbuf[MAX_UARTX_TX_BUF];

}UARTS_DATA;



typedef struct _dev_info {
	uint8_t name[104];
	uint8_t uuid[17];
	uint8_t hardware_h;
	uint8_t hardware_l;
	uint8_t software_h;
	uint8_t software_l;
}DEV_INFO;


typedef struct _order_val{
	uint8_t resource_buf[MAX_FRAME][MAX_RESOURCE_LEN];
	uint8_t work_order_id[76];	
	uint8_t send_0xA6_mobile[25];
	uint8_t occ_current_frame_num;
	uint8_t act_frame_ing;
	uint8_t resource_timeout;
	uint8_t resource_start;
	uint8_t resource_comper_flag;
}ORDER_VAL;


#define	READ_DEVICE_CONFIG_INFO			0x01 //读取设备的配置信息
#define READ_DEVICE_FRAME_INFO			0x02 //读取机框信息
#define READ_TRAY_INFO					0x03 //读取盘信息
#define READ_HW_SW_INFO					0x05 //读取软硬件版本号
#define SOFTWARE_UPDATA					0x06 //软件升级
#define WRITE_ELECTORN_LABLE			0x07 //待写入电子标签信息
#define READ_DEVICE_ALARM				0x09 //读取设备告警信息
#define ALARM_CLEAN						0x0A //告警确认和清除
#define WRITE_DEVICE_CONFIG_INFO		0x0C //写入设备配置信息
#define WRITE_DEVICE_UUID				0x14 //写入设备UUID 

#define ORDERS_CMD						0x0D //工单总命令
#define RESORCE_SEARCH					0x01 //资源采集
#define RESORCE_POOL					0x11 //资源巡检
#define RESORCE_COVER					0x0C //二次采集覆盖端口
#define SEND_RESOURCE_MOBILE			0xA3 //发送资源采集数据给手机
#define CLEAN_ORDER						0xA2 //取消工单
#define CLEAN_CHANGE_LINE				0x9A //取消换线
#define CHANGE_ORDER					0x03 //一个改跳工单
#define DEL_ONE_PORT					0x04 //一个单端删除
#define BUILD_TWO_PORT					0x05 //新建一个双端跳接
#define DEL_TWO_PORT					0x06 //双端删除
#define BATCH_BUILD_PORT				0x07 //最大新建5对双端跳接
#define DIV_ONE_PORT					0xB1 //新建一个单端光分路器
#define BATCH_DIV_PORT					0xB2 //批量新建光分路器





#define CONTROL_WRITE_ELECTORN_LABLE 	0x0F //受控写入电子标签
#define TEST_LED						0x10 //LED灯测试
#define CHANGE_TRAY						0x13 //更换托盘

#define DATA_ERROR						0xF6 //接受的数据错误
#define DATA_OK							0xA6 //数据接受正确
#define POOL_CMD						0x20 //巡检命令

#define COME_RS485						0x01 //数据来源与485
#define COME_UART0						0x02 //数据来源与uart0 



#endif


