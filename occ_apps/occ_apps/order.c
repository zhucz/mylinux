#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <error.h>
#include <unistd.h> //sleep
#include <stdint.h> //uint8_t 
#include <string.h> //memset等

#include <sys/time.h>//定时器
#include <signal.h>

#include <pthread.h>//线程创建，互斥锁创建

#include "./inc/data_resource.h"
#include "./inc/SerDrive.h"
#include "./inc/crc8.h"
#include "./inc/cmd_analysis.h"
#include "./inc/mobile_routine.h"

ORDER_VAL order_val;// = {{'\0'},{'\0'},0x00,0x00,0x00,0x00,0x00};

uint8_t special_send_buf[8][600];//专门用来发送数据到sim3u146


extern int com0_fd;
extern int com1_fd;
extern DEV_INFO board_info;
extern UARTS_DATA uart485_data;
extern UARTS_DATA uart0_data;
extern uint8_t other_thing_now;

static void resource_send_0xa6_to_uart0(uint8_t *src)
{
	uint8_t r_send_0xa6[9] = {0x7E,0x00,0x07,0xA6,0x00,0x00,0x00,0x00,0x5A};
	r_send_0xa6[4] = src[3];
	r_send_0xa6[5] = src[4];
	r_send_0xa6[6] = order_val.occ_current_frame_num;
	r_send_0xa6[7] = crc8(r_send_0xa6,(r_send_0xa6[1] << 8)|r_send_0xa6[2]);
	send_data_tty(com0_fd,r_send_0xa6,((r_send_0xa6[1] << 8)|r_send_0xa6[2])+2);
	usleep(500000);
}


static void occ_resources_collection(uint8_t *src,uint8_t come_from)
{
	static uint16_t elapsed_time = 0;
	static uint8_t resource_cunt = 0;
	static uint8_t f_index = 0;
	static uint8_t r_146_send_buf[8] = {'\0'};
	static uint8_t r_146_recv_buf[8] = {'\0'};
	static uint8_t r_send_buf[8] = {0x7E,0x00,0x06,0x0D,0x01,0x00,0x00,0x5A};
	
	static uint8_t r_poll_mesg_cmd[7] = {0x7E,0x00,0x05,0x20,0x00,0x00,0x5A};

	uint8_t read_r[8] = {'\0'};
	uint8_t resource_i = 0;
	uint16_t read_0xa6 = 0;

	switch(come_from)
	{
		case COME_RS485:
			printf("starting resource....\n");
			memcpy(&order_val.work_order_id[0],&src[5],33);//保存一下工单号和UUID 
			r_send_buf[4] = src[4];//发送的可能是资源采集也可能是资源巡检 
			memset(r_146_send_buf,0x00,8);
			memset(r_146_recv_buf,0x00,8);
			resource_cunt = 0;
			order_val.occ_current_frame_num = 1;
			order_val.resource_start = 1;
			other_thing_now = 1;

			printf("act_frame_ing = 0x %x\n",order_val.act_frame_ing);
			for(;;){
				if(r_146_send_buf[order_val.occ_current_frame_num - 1] == 0x00){
					r_146_send_buf[order_val.occ_current_frame_num - 1] = 0x01;
					r_send_buf[5] = order_val.occ_current_frame_num;
					r_send_buf[6] = crc8(r_send_buf,(r_send_buf[1] << 8) | r_send_buf[2]);
					r_send_buf[7] = 0x5A;
					send_data_tty(com0_fd,r_send_buf,((r_send_buf[1] << 8) | r_send_buf[2]) + 2);
					usleep(500000);
				}

				read_0xa6 = read_datas_tty(com0_fd,uart0_data.uartx_rxbuf,100,1024);
				
				if(uart0_data.uartx_rxbuf[3] == 0xA6){
					uart0_data.uartx_rxbuf[3] = 0x00;
					r_146_recv_buf[order_val.occ_current_frame_num - 1] = 0x01;
					resource_cunt |= (1 << (uart0_data.uartx_rxbuf[4] - 1));
					printf("0xA6 Coming ...frame = 0x%x\n",uart0_data.uartx_rxbuf[4]);

					if(resource_cunt == order_val.act_frame_ing){
						resource_cunt = 0;
						order_val.occ_current_frame_num = 1;
						elapsed_time = 0;
						break;
					}
				}
				if(r_146_recv_buf[order_val.occ_current_frame_num - 1] == 0x00){
					r_146_send_buf[order_val.occ_current_frame_num - 1] = 0x00;
				}
				
				order_val.occ_current_frame_num++;
				if(order_val.occ_current_frame_num > 8) order_val.occ_current_frame_num = 1;
				
				elapsed_time++;
				if(elapsed_time > 32){
					elapsed_time = 0;
					order_val.occ_current_frame_num = 1;
					//return;
					break;
				}
			}
/**********************************************************************************************************/
			printf("shou ji wan bi .. \n");

			usleep(500000);
			memset(r_146_send_buf,0x00,8);
			memset(r_146_recv_buf,0x00,8);
			order_val.occ_current_frame_num = 1;
			for(;;){

				if(r_146_send_buf[order_val.occ_current_frame_num - 1] == 0x00){
					r_146_send_buf[order_val.occ_current_frame_num - 1] = 0x01;
					r_poll_mesg_cmd[4] = order_val.occ_current_frame_num;
					r_poll_mesg_cmd[5] = crc8(r_poll_mesg_cmd,(r_poll_mesg_cmd[1] << 8) | r_poll_mesg_cmd[2]);
					r_poll_mesg_cmd[6] = 0x5A;
					send_data_tty(com0_fd,r_poll_mesg_cmd,((r_poll_mesg_cmd[1] << 8) | r_poll_mesg_cmd[2]) + 2);
					usleep(500000);

					printf("send 0x20 frame = 0x%x....\n",order_val.occ_current_frame_num);
				}
				read_0xa6 = read_datas_tty(com0_fd,uart0_data.uartx_rxbuf,100,3000);
				printf("read_0xa6 = %d\n",read_0xa6);
				if(read_0xa6 > 4){
					if(uart0_data.uartx_rxbuf[3] == 0x0D){
						f_index = (uart0_data.uartx_rxbuf[5] - 1);
						printf("f_index =0x%x \n",f_index);
						order_val.resource_comper_flag |= (1 << f_index);
						printf("uartx_rxbuf[5] =0x%x \n",uart0_data.uartx_rxbuf[5]);

						memcpy(&order_val.resource_buf[f_index][128],&uart0_data.uartx_rxbuf[6],2356);
						printf("resource_comper_flag = 0x%x \n",order_val.resource_comper_flag);
						resource_send_0xa6_to_uart0(uart0_data.uartx_rxbuf);//发送0XA6

				//		printf("f_index =0x%x \n",uart0_data.uartx_rxbuf[5]);
						printf("act_frame_ing=0x%x\n",order_val.act_frame_ing);
						if(order_val.resource_comper_flag == order_val.act_frame_ing){
							order_val.resource_comper_flag = 0;
							f_index = 0;
							for(resource_i = 0;resource_i < 8;resource_i++){
								order_val.resource_buf[resource_i][0]   = 0x7E; 
								order_val.resource_buf[resource_i][1]   = 0x09;
								order_val.resource_buf[resource_i][2]   = 0xB4;
								order_val.resource_buf[resource_i][3]   = 0x0D;
								order_val.resource_buf[resource_i][4]   = 0x01;
								memcpy(&order_val.resource_buf[resource_i][5],&order_val.work_order_id[0],17);
								order_val.resource_buf[resource_i][22]  = 0x02;
								memcpy(&order_val.resource_buf[resource_i][23],&board_info.uuid[0],16);
								order_val.resource_buf[resource_i][39]  = board_info.name[5];
								memcpy(&order_val.resource_buf[resource_i][44],&board_info.name[18],80);
								order_val.resource_buf[resource_i][124] = board_info.hardware_h;
								order_val.resource_buf[resource_i][125] = board_info.hardware_l;
								order_val.resource_buf[resource_i][126] = board_info.software_h;
								order_val.resource_buf[resource_i][127] = board_info.software_l;
							}
				
							read_r[0] = 0x7E;
							read_r[1] = 0x00;
							read_r[2] = 0x06;
							read_r[3] = 0x0D;
							read_r[4] = 0xA1;
							read_r[5] = 0x00;
							read_r[6] = crc8(read_r,(read_r[1] << 8) | read_r[2]);
							read_r[7] = 0x5A;
							send_data_tty(com1_fd,read_r,((read_r[1] << 8) | read_r[2]) + 2);
							printf("rsource data ready.... \n");
							return;
						}
					}else{
						r_146_send_buf[order_val.occ_current_frame_num - 1] = 0x00;
					}

				}else{
						printf("resource_comper_flag = 0x%x \n",order_val.resource_comper_flag);
						if(order_val.resource_comper_flag == order_val.act_frame_ing){
							order_val.resource_comper_flag = 0;

							for(resource_i = 0;resource_i < 8;resource_i++){
								order_val.resource_buf[resource_i][0]   = 0x7E; 
								order_val.resource_buf[resource_i][1]   = 0x09;
								order_val.resource_buf[resource_i][2]   = 0xB4;
								order_val.resource_buf[resource_i][3]   = 0x0D;
								order_val.resource_buf[resource_i][4]   = 0x01;
								memcpy(&order_val.resource_buf[resource_i][5],&order_val.work_order_id[0],17);
								order_val.resource_buf[resource_i][22]  = 0x02;
								memcpy(&order_val.resource_buf[resource_i][23],&board_info.uuid[0],16);
								order_val.resource_buf[resource_i][39]  = board_info.name[5];
								memcpy(&order_val.resource_buf[resource_i][44],&board_info.name[18],80);
								order_val.resource_buf[resource_i][124] = board_info.hardware_h;
								order_val.resource_buf[resource_i][125] = board_info.hardware_l;
								order_val.resource_buf[resource_i][126] = board_info.software_h;
								order_val.resource_buf[resource_i][127] = board_info.software_l;
							}
				
							read_r[0] = 0x7E;
							read_r[1] = 0x00;
							read_r[2] = 0x06;
							read_r[3] = 0x0D;
							read_r[4] = 0xA1;
							read_r[5] = 0x00;
							read_r[6] = crc8(read_r,(read_r[1] << 8) | read_r[2]);
							read_r[7] = 0x5A;
							send_data_tty(com1_fd,read_r,((read_r[1] << 8) | read_r[2]) + 2);
							printf("rsource data ready.... \n");
							return;
					}
				}
					order_val.occ_current_frame_num++;
					if(order_val.occ_current_frame_num > 8) order_val.occ_current_frame_num = 1;
				}
			break;
		default:
			break;
	}
}


static void send_resource_data_to_mobile(uint8_t *src)
{
	static uint8_t resource_succed_or_not = 0;

	order_val.resource_buf[src[5] - 1][136] = src[5];
	order_val.resource_buf[src[5] - 1][2484] = crc8(&order_val.resource_buf[src[5] - 1][0],((order_val.resource_buf[src[5] - 1][1] << 8) | order_val.resource_buf[src[5] - 1][2]));
	order_val.resource_buf[src[5] - 1][2485] = 0x5A;
	send_data_tty(com1_fd,&order_val.resource_buf[src[5] - 1][0],((order_val.resource_buf[src[5] - 1][1] << 8) | order_val.resource_buf[src[5] - 1][2]) + 2);
	
	resource_succed_or_not++;
	if(resource_succed_or_not >= 8){
		resource_succed_or_not = 0;
		order_val.resource_start = 0;
		other_thing_now = 0;
	}
}



void occ_send_data_to_sim3u146(void) //special_send_buf[][]
{
	uint8_t i = 0;
	uint8_t j = 0;
//	uint8_t send_count[8] = {'\0'};
	uint16_t len = 0;

	for(j=0;j<4;j++){
		for(i = 0;i < 8;i++){
			if(special_send_buf[i][3] != 0x00){
				printf("special_send_buf[%d][3] = %x \n",i,special_send_buf[i][3]);
				send_data_tty(com0_fd,&special_send_buf[i][0],((special_send_buf[i][1] << 8) | special_send_buf[i][2]) + 2);
//				usleep(500000);
				len = read_datas_tty(com0_fd,uart0_data.uartx_rxbuf,100,1024);
				if(len > 4){
					if(uart0_data.uartx_rxbuf[3] == 0xA6){
						memset(&special_send_buf[i][0],0,600);
						memset(uart0_data.uartx_rxbuf,0,sizeof(uart0_data.uartx_rxbuf));
						printf("recv 0xa6..... \n");
						printf("special_send_buf[%d][3] = %x \n",i,special_send_buf[i][3]);
					}
				}

//				if(j == 3){
//					memset(&special_send_buf[i][0],0,600);
//				}
				usleep(200000);
			}
		}
	}
}




/*
 *双端新建
 *
 * */
void occ_new_build_double_port_order(uint8_t *src,uint8_t come_from)
{
	static uint8_t back_to_mobile_order[100] = {'\0'};
	static uint8_t occ_new_double[48] = {0x7E,0x00,0x2E,0x0D,0x05,0x41};
	static uint8_t buf_0x8c[16] = {0x7E,0x00,0x0E,0x0D,0x8C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x5A};
	static uint8_t port_a[4] = {'\0'};
	static uint8_t port_b[4] = {'\0'};
	static uint8_t is_double_new_ok = 0;
	uint8_t zerro[32] = {'\0'};
	uint8_t len_cont[8] = {'\0'};
	uint8_t uuid_aary = 0;
	uint8_t Farme_num = 0;
	uint8_t index_i = 0;
	uint16_t data_len = 0;
	uint8_t arry = 0;

	memset(&special_send_buf[0][0],0,600);
	memset(&special_send_buf[1][0],0,600);
	memset(&special_send_buf[2][0],0,600);
	memset(&special_send_buf[3][0],0,600);
	memset(&special_send_buf[4][0],0,600);
	memset(&special_send_buf[5][0],0,600);
	memset(&special_send_buf[6][0],0,600);
	memset(&special_send_buf[7][0],0,600);

	switch(come_from){
		case COME_RS485:
			if((src[4] == 0x05) && (src[5] != 0xA2)){//双端的新建
			uuid_aary = memcmp(&board_info.uuid[0],&src[22],16);
			if(!uuid_aary){
				other_thing_now = 1;
				memset(&occ_new_double[6],0,42);
				memcpy(back_to_mobile_order,src,((src[1]<<8)|src[2]));
				occ_new_double[4] = 0x05;
				memcpy(&occ_new_double[6],&src[38],3);
				memcpy(&occ_new_double[10],&src[41],3);
				memcpy(&port_a[0],&occ_new_double[6],4);
				memcpy(&port_b[0],&occ_new_double[10],4);
				Farme_num = port_a[0];//得到框号
				memset(&special_send_buf[Farme_num - 1][0],0,600);
				memcpy(&special_send_buf[Farme_num - 1][0],&occ_new_double[0],48);
				special_send_buf[Farme_num - 1][46] = crc8(&special_send_buf[Farme_num - 1][0],((special_send_buf[Farme_num - 1][1] << 8) | special_send_buf[Farme_num - 1][2]));
				special_send_buf[Farme_num - 1][47] = 0x5A;
				if(occ_new_double[6] != occ_new_double[10]){//不同框发送两遍
					Farme_num = port_b[0];//得到框号
					memset(&special_send_buf[Farme_num - 1][0],0,600);
					memcpy(&occ_new_double[6],&port_b[0],4);
					memcpy(&occ_new_double[10],&port_a[0],4);
					memcpy(&special_send_buf[Farme_num - 1][0],&occ_new_double[0],48);
					special_send_buf[Farme_num - 1][46] = crc8(&special_send_buf[Farme_num - 1][0],((special_send_buf[Farme_num - 1][1] << 8) | special_send_buf[Farme_num -1][2]));
					special_send_buf[Farme_num - 1][47] = 0x5A;
				}
				occ_send_data_to_sim3u146();
				usleep(200000);
				other_thing_now = 0;
				}else{
					printf("uuid error\n");
				}
			}else if(src[5] == 0xA2){//双端的取消
				other_thing_now = 1;
				data_len = src[2];//一共有几个端口需要取消 数据的长度
				for(index_i = 7;index_i < data_len;index_i += 4){
					memcpy(&special_send_buf[src[index_i]-1][len_cont[src[index_i] - 1] + 6],&src[index_i],4);
					len_cont[src[index_i] - 1] += 4;

					special_send_buf[src[index_i] - 1][0] = 0x7E;
					special_send_buf[src[index_i] - 1][1] = 0x00;
					special_send_buf[src[index_i] - 1][2] = (len_cont[src[index_i] - 1] + 6);
					special_send_buf[src[index_i] - 1][3] = 0x0D;
					special_send_buf[src[index_i] - 1][4] = 0xA2;
					special_send_buf[src[index_i] - 1][5] = 0x05;
				}

				for(index_i = 0;index_i<8;index_i++){
					if(special_send_buf[index_i][3] != 0x00){
						special_send_buf[index_i][special_send_buf[index_i][2]] = crc8(&special_send_buf[index_i][0],((special_send_buf[index_i][1] << 8) | special_send_buf[index_i][2]));
						special_send_buf[index_i][special_send_buf[index_i][2] + 1] = 0x5A;
					}
				}
				occ_send_data_to_sim3u146();
				usleep(200000);
				other_thing_now = 0;
				memset(back_to_mobile_order,0,sizeof(back_to_mobile_order));
				memset(port_a,0,4);
				memset(port_b,0,4);
			}else{
			//....do nothing
			}
			break;
		case COME_UART0:
			other_thing_now = 1;
			switch(src[5]){
				case 0x03:		
					arry = memcmp(&occ_new_double[14],&zerro[0],32);	
					if(!arry){ //如果EID是32个0那么就直接拷贝下来
							memcpy(&occ_new_double[14],&src[10],32);
							memcpy(&back_to_mobile_order[45],&src[10],32);
						//	send_data_tty(com0_fd,occ_new_double,(((occ_new_double[1]<<8)|occ_new_double[2]) + 2));
						//	sleep(1);
							if(!memcmp(&port_a[0],&src[7],3)){   //判断是port_a[]还是port_b[]
								memcpy(&occ_new_double[6],&port_b[0],3);
								occ_new_double[10] = 0x00;
								occ_new_double[11] = 0x00;
								occ_new_double[12] = 0x00;
								occ_new_double[46] = crc8(occ_new_double,((occ_new_double[1]<<8) | occ_new_double[2]));
								occ_new_double[47] = 0x5A;
								is_double_new_ok |= (1 << 0);
								Farme_num = occ_new_double[6];
								memset(&special_send_buf[Farme_num - 1][0],0,600);
								memcpy(&special_send_buf[Farme_num - 1][0],&occ_new_double[0],48);
								occ_send_data_to_sim3u146();
							}
							if(!memcmp(&port_b[0],&src[7],3)){   //判断是port_a[]还是port_b[]						
								memcpy(&occ_new_double[6],&port_a[0],3);
								occ_new_double[10] = 0x00;
								occ_new_double[11] = 0x00;
								occ_new_double[12] = 0x00;
								occ_new_double[46] = crc8(occ_new_double,((occ_new_double[1]<<8) | occ_new_double[2]));
								occ_new_double[47] = 0x5A;
								is_double_new_ok |= (1 << 1);
								Farme_num = occ_new_double[6];
								memset(&special_send_buf[Farme_num - 1][0],0,600);
								memcpy(&special_send_buf[Farme_num - 1][0],&occ_new_double[0],48);
								occ_send_data_to_sim3u146();
							}
						}else{//有一个端口已经被插入了
							if(!memcmp(&port_a[0],&src[7],3)){   //判断是port_a[]还是port_b[]
								is_double_new_ok |= (1 << 0);
							}

							if(!memcmp(&port_b[0],&src[7],3)){   //判断是port_a[]还是port_b[]
								is_double_new_ok |= (1 << 1);
							}	
					}

					if(is_double_new_ok == 0x03){
						is_double_new_ok = 0;
						occ_new_double[4] = 0x0A;
						memcpy(&occ_new_double[6],&port_a[0],3);
						memcpy(&occ_new_double[10],&port_b[0],3);
						Farme_num = port_a[0];
						memset(&special_send_buf[Farme_num - 1][0],0,600);
						memcpy(&special_send_buf[Farme_num - 1][0],&occ_new_double[0],48);
						special_send_buf[Farme_num - 1][46] = crc8(&special_send_buf[Farme_num - 1][0],((special_send_buf[Farme_num - 1][1]<<8) | special_send_buf[Farme_num - 1][2]));
						special_send_buf[Farme_num - 1][47] = 0x5A;
						if(port_a[0] != port_b[0]){
							Farme_num = port_b[0];
							memset(&special_send_buf[Farme_num - 1][0],0,600);
							memcpy(&occ_new_double[6],&port_b[0],3);
							memcpy(&occ_new_double[10],&port_a[0],3);
							memcpy(&special_send_buf[Farme_num - 1][0],&occ_new_double[0],48);
							special_send_buf[Farme_num - 1][46] = crc8(&special_send_buf[Farme_num - 1][0],((special_send_buf[Farme_num - 1][1]<<8) | special_send_buf[Farme_num - 1][2]));
							special_send_buf[Farme_num - 1][47] = 0x5A;
						}
						occ_send_data_to_sim3u146();
					}	
					break;
				case 0x0A:
					printf("0x0a coming ....\n");
					if(!memcmp(&port_a[0],&src[7],3)){   //判断是port_a[]还是port_b[]
						is_double_new_ok |= (1 << 0);
					}	

					if(!memcmp(&port_b[0],&src[7],3)){   //判断是port_a[]还是port_b[]
						is_double_new_ok |= (1 << 1);
					}
					
					if(is_double_new_ok == 0x03){//收集到两个端口的0x0A 
						is_double_new_ok = 0;
						back_to_mobile_order[0] = 0x7E;
						back_to_mobile_order[1] = 0x00;
						back_to_mobile_order[2] = 0x4D;
						back_to_mobile_order[44] = 0x00;//执行结果
						back_to_mobile_order[77] = crc8(back_to_mobile_order,((back_to_mobile_order[1]<<8) | back_to_mobile_order[2]));
						back_to_mobile_order[78] = 0x5A;
						send_data_tty(com1_fd,back_to_mobile_order,(((back_to_mobile_order[1] << 8)| back_to_mobile_order[2]) + 2));
					}

					break;
				case 0x8C:
					if(!memcmp(&port_a[0],&src[7],3)){   //判断是port_a[]还是port_b[]
						is_double_new_ok &= ~(1 << 0);
					}	

					if(!memcmp(&port_b[0],&src[7],3)){   //判断是port_a[]还是port_b[]
						is_double_new_ok &= ~(1 << 1);
					}
					if(is_double_new_ok == 0x00){
					printf("0x8c coming ....\n");
						memcpy(&buf_0x8c[6],&port_a[0],3);
						memcpy(&buf_0x8c[10],&port_a[0],3);
						memset(&occ_new_double[14],0,32);
						Farme_num = port_a[0];
						memset(&special_send_buf[Farme_num - 1][0],0,600);
						memcpy(&special_send_buf[Farme_num - 1][0],&buf_0x8c[0],16);
						special_send_buf[Farme_num - 1][14] = crc8(&special_send_buf[Farme_num - 1][0],((special_send_buf[Farme_num - 1][1]<<8) | special_send_buf[Farme_num - 1][2]));
						special_send_buf[Farme_num - 1][15] = 0x5A;
						if(port_a[0] != port_b[0]){
							Farme_num = port_b[0];
							memset(&special_send_buf[Farme_num - 1][0],0,600);
							memcpy(&occ_new_double[6],&port_b[0],3);
							memcpy(&occ_new_double[10],&port_a[0],3);
							memcpy(&special_send_buf[Farme_num - 1][0],&buf_0x8c[0],16);
							special_send_buf[Farme_num - 1][14] = crc8(&special_send_buf[Farme_num - 1][0],((special_send_buf[Farme_num - 1][1]<<8) | special_send_buf[Farme_num - 1][2]));
							special_send_buf[Farme_num - 1][15] = 0x5A;
						}
						occ_send_data_to_sim3u146();
					}
					break;
				default:
					break;
			}

			usleep(200000);
			other_thing_now = 0;
			break;
		default:
			break;
	}
}


/*
 *双端拆除
 *
 * */
void occ_remove_double_port_order(uint8_t *src,uint8_t come_from)
{

	static uint8_t back_to_mobile_order[100] = {'\0'};
//	static uint8_t buf_0x8c[16] = {0x7E,0x00,0x0E,0x0D,0x8C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x5A};
	static uint8_t cmd_remove_double[16] = {0x7E,0x00,0x0E,0x0D,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x5A};
	static uint8_t port_a[4] = {'\0'};
	static uint8_t port_b[4] = {'\0'};
	static uint8_t is_double_remove_ok = 0;
	uint8_t len_cont[8] = {'\0'};
	uint8_t uuid_aary = 0;
	uint8_t Farme_num = 0;
	uint8_t index_i = 0;
	uint16_t data_len = 0;

	switch(come_from){
		case COME_RS485:
			other_thing_now = 1;
			if(src[5] == 0xA2){//取消拆除工单
				other_thing_now = 1;
				data_len = src[2];//一共有几个端口需要取消 数据的长度
				for(index_i = 7;index_i < data_len;index_i += 4){
					memcpy(&special_send_buf[src[index_i]-1][len_cont[src[index_i] - 1] + 6],&src[index_i],4);
					len_cont[src[index_i] - 1] += 4;
					special_send_buf[src[index_i] - 1][0] = 0x7E;
					special_send_buf[src[index_i] - 1][1] = 0x00;
					special_send_buf[src[index_i] - 1][2] = (len_cont[src[index_i] - 1] + 6);
					special_send_buf[src[index_i] - 1][3] = 0x0D;
					special_send_buf[src[index_i] - 1][4] = 0xA2;
					special_send_buf[src[index_i] - 1][5] = 0x05;
				}
				for(index_i = 0;index_i<8;index_i++){
					if(special_send_buf[index_i][3] != 0x00){
						special_send_buf[index_i][special_send_buf[index_i][2]] = crc8(&special_send_buf[index_i][0],((special_send_buf[index_i][1] << 8) | special_send_buf[index_i][2]));
						special_send_buf[index_i][special_send_buf[index_i][2] + 1] = 0x5A;
					}
				}
				occ_send_data_to_sim3u146();
				memset(back_to_mobile_order,0,sizeof(back_to_mobile_order));
				memset(port_a,0,4);
				memset(port_b,0,4);
			}else{//是一个双端拆除
				uuid_aary = memcmp(board_info.uuid,&src[22],16);
				if(!uuid_aary){
					cmd_remove_double[4] = 0x06;
					memcpy(back_to_mobile_order,&src[0],((src[1]<<8)|src[2]));
					memcpy(&port_a[0],&src[38],3);
					memcpy(&port_b[0],&src[41],3);
					memcpy(&cmd_remove_double[6],&port_a[0],3);
					memcpy(&cmd_remove_double[10],&port_b[0],3);
					Farme_num = port_a[0];
					memset(&special_send_buf[Farme_num - 1][0],0,600);
					memcpy(&special_send_buf[Farme_num - 1][0],&cmd_remove_double[0],16);
					special_send_buf[Farme_num - 1][14] = crc8(&special_send_buf[Farme_num - 1][0],((special_send_buf[Farme_num - 1][1]<<8) | special_send_buf[Farme_num - 1][2]));
					special_send_buf[Farme_num - 1][15] = 0x5A;
					if(port_a[0] != port_b[0]){
						Farme_num = port_b[0];
						memset(&special_send_buf[Farme_num - 1][0],0,600);
						memcpy(&cmd_remove_double[6],&port_b[0],3);
						memcpy(&cmd_remove_double[10],&port_a[0],3);
						memcpy(&special_send_buf[Farme_num - 1][0],&cmd_remove_double[0],16);
						special_send_buf[Farme_num - 1][14] = crc8(&special_send_buf[Farme_num - 1][0],((special_send_buf[Farme_num - 1][1]<<8) | special_send_buf[Farme_num - 1][2]));
						special_send_buf[Farme_num - 1][15] = 0x5A;
					}
					occ_send_data_to_sim3u146();
				}else{
					printf("uuid error \n");
				}
			}
			usleep(200000);
			other_thing_now = 0;
			break;
		case COME_UART0:
			other_thing_now = 1;
			if(src[5] == 0x0A){
				if(!memcmp(&port_a[0],&src[7],3)){
					is_double_remove_ok |= (1 << 0);
				}
			
				if(!memcmp(&port_b[0],&src[7],3)){
					is_double_remove_ok |= (1 << 1);
				}

				if(is_double_remove_ok == 0x03){
					is_double_remove_ok = 0;
					back_to_mobile_order[0]  = 0x7E;
					back_to_mobile_order[1]  = 0x00;
					back_to_mobile_order[2]  = 0x2D;
					back_to_mobile_order[44] = 0x00;//执行结果
					back_to_mobile_order[45] = crc8(back_to_mobile_order,((back_to_mobile_order[1]<<8) | back_to_mobile_order[2]));
					back_to_mobile_order[46] = 0x5A;
					send_data_tty(com1_fd,back_to_mobile_order,(((back_to_mobile_order[1] << 8)| back_to_mobile_order[2]) + 2));
				}
			}else{
			
				if(!memcmp(&port_a[0],&src[7],3)){
					if(src[10] == 0x01){
						is_double_remove_ok |= (1 << 0);
					}else if(src[10] == 0x02){
						is_double_remove_ok &= ~(1 << 0);
					}else{
				
					}
				}

				if(!memcmp(&port_b[0],&src[7],3)){
					if(src[10] == 0x01){
						is_double_remove_ok |= (1 << 1);
					}else if(src[10] == 0x02){
						is_double_remove_ok &= ~(1 << 1);
					}else{
					
					}
				}

				if(is_double_remove_ok == 0x03){
					is_double_remove_ok = 0;
					cmd_remove_double[4] = 0x0A;
					Farme_num = port_a[0];
					memset(&special_send_buf[Farme_num - 1][0],0,600);
					memcpy(&special_send_buf[Farme_num - 1][0],&cmd_remove_double[0],16);
					special_send_buf[Farme_num - 1][14] = crc8(&special_send_buf[Farme_num - 1][0],((special_send_buf[Farme_num - 1][1]<<8) | special_send_buf[Farme_num - 1][2]));
					special_send_buf[Farme_num - 1][15] = 0x5A;
					if(port_a[0] != port_b[0]){
						Farme_num = port_b[0];
						memset(&special_send_buf[Farme_num - 1][0],0,600);
						memcpy(&cmd_remove_double[6],&port_b[0],3);
						memcpy(&cmd_remove_double[10],&port_a[0],3);
						memcpy(&special_send_buf[Farme_num - 1][0],&cmd_remove_double[0],16);
						special_send_buf[Farme_num - 1][14] = crc8(&special_send_buf[Farme_num - 1][0],((special_send_buf[Farme_num - 1][1]<<8) | special_send_buf[Farme_num - 1][2]));
						special_send_buf[Farme_num - 1][15] = 0x5A;
					}
					occ_send_data_to_sim3u146();
				}
			}
			usleep(200000);
			other_thing_now = 0;
			break;
		default:
			break;
	}

}







/*
 *工单命令解析
 *
 * */
void processing_order_cmd(uint8_t *src,uint8_t come_from)
{
	uint8_t cmd_minor = 0;

	if(come_from == COME_RS485){
		if(src[4] != 0xA6){
			if(src[5] != 0xA2){
				if((src[5] == 0xA4) || (src[5] == 0x9A)){
					memcpy(&order_val.send_0xA6_mobile[5],&src[7],17);
					order_val.send_0xA6_mobile[22] = src[5];
				}else{
					memcpy(&order_val.send_0xA6_mobile[5],&src[5],17);
					order_val.send_0xA6_mobile[22] = src[4];
				}
				order_val.send_0xA6_mobile[0] = 0x7E;
				order_val.send_0xA6_mobile[1] = 0x00;
				order_val.send_0xA6_mobile[2] = 0x17;
				order_val.send_0xA6_mobile[3] = 0x0D;
				order_val.send_0xA6_mobile[4] = 0xA6;
				order_val.send_0xA6_mobile[23] = crc8(order_val.send_0xA6_mobile,(order_val.send_0xA6_mobile[1] << 8)|order_val.send_0xA6_mobile[2]);
				order_val.send_0xA6_mobile[24] = 0x5A;
				send_data_tty(com1_fd,order_val.send_0xA6_mobile,((order_val.send_0xA6_mobile[1] << 8) | order_val.send_0xA6_mobile[2]) + 2);
			}
		}
	}


	cmd_minor = src[4];

	switch(cmd_minor){	
		case  RESORCE_SEARCH:		 // 资源采集 0x01 
			occ_resources_collection(src,come_from);
			break;
		case  RESORCE_POOL:			 // 资源巡检 0x11
			break;
		case  RESORCE_COVER:		 // 二次采集覆盖端口 0x0C
			break;
		case  SEND_RESOURCE_MOBILE:	 // 发送资源采集数据给手机 0xA3 
			send_resource_data_to_mobile(src);
			break;
		case  CLEAN_ORDER:			 // 取消工单 0xA2 
//			occ_clean_processing_order(src,come_from);
			break;
		case  CLEAN_CHANGE_LINE:	 // 取消换线 0x9A
			break;
		case  CHANGE_ORDER:			 // 一个改跳工单 0x03
			break;
		case  DEL_ONE_PORT:			 // 一个单端删除 0x04
			break;
		case  BUILD_TWO_PORT:	     // 新建一个双端跳接 0x05 
			occ_new_build_double_port_order(src,come_from);
			break;
		case  DEL_TWO_PORT:			 // 双端删除 0x06 
			occ_remove_double_port_order(src,come_from);
			break;
		case  BATCH_BUILD_PORT:		 // 最大新建5对双端跳接 0x07
			break;
		case  DIV_ONE_PORT:			 // 新建一个单端光分路器 0xB1
			break;
		case  BATCH_DIV_PORT:		 // 批量新建光分路器 0B2
			break;
		default:
			break;
	}
}
