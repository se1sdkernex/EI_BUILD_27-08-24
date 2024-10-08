/*
 * User_UDP.h
 *
 *  Created on: May 15, 2023
 *      Author: Ravi Teja P
 */

#ifndef INC_USER_UDP_H_
#define INC_USER_UDP_H_

#include "Headers.h"

#define ETH_MAX_SIZE	1536



typedef struct
{
	uint8_t Ethernet_Buffers[ETH_MAX_SIZE];
	uint16_t Read_end;
	uint16_t Write_end;
}ETHERNET_Circle_t;

extern ETHERNET_Circle_t	Ethernet_Circle_buff_st;

extern ip_addr_t Source_IP;


void udpServer_init(void);
void UDP_Receive_Callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);
void udpClient_connect(void);
void udpClient_send(uint8_t *data,uint8_t len,const ip_addr_t IP_addr);

uint8_t Is_There_EI_Data(void);
uint8_t Get_Eth_Byte(void);
void Udp_Send_SigInfo(uint8_t *data,uint8_t len,const ip_addr_t IP_addr);
#endif /* INC_USER_UDP_H_ */
