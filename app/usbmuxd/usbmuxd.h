/*
 * usbmuxd.h
 *
 * Copyright (C) 2009 Hector Martin <hector@marcansoft.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 or version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef USBMUXD_H
#define USBMUXD_H
#include "emf_typedef.h"
#include "usbmuxd-proto.h"

#define TCP_SPORT				0X1026
#define TCP_APP_DPORT			0X0001
#define TCP_SYS_DPORT			0XF27E

#define DEVICE_ID			0





#define DEV_MRU 512				//设备最大接收数据长度
#define USB_MTU 362				//usb mount tx packet ,最大TX包大小
#define USB_MRU (64)			//usb mount rx packet

enum client_state {
	CLIENT_COMMAND,		// waiting for command
	CLIENT_LISTEN,		// listening for devices
	CLIENT_CONNECTING1,	// issued connection request
	CLIENT_CONNECTING2,	// connection established, but waiting for response message to get sent
	CLIENT_CONNECTED,	// connected
	CLIENT_DEAD
};
/*--------------------------------------------------------------*/




#define IPPROTO_TCP 6 /* tcp */


struct mux_client {
	int fd;
	uint32_t ob_size;
	uint32_t ob_capacity;
	uint32_t ib_capacity;
	short events, devents;
	uint32_t connect_tag;
	int connect_device;
	enum client_state state;
	uint32_t proto_version;
	uint32_t number;
	char* info;		//plist_t
};

struct device_info {
	int id;
	const char *serial;
	uint32_t location;
	uint16_t pid;
	uint64_t speed;
};

extern bool usbmuxd_config_ready;


void usbmuxd_init(void);
void usbmuxd_handler(uint32_t period_10us);
void usbmuxd_data_process(uint8_t* buf,uint16_t len);
bool usbmuxd_send_data(uint8_t index,uint8_t* buf,uint16_t len);


#endif
