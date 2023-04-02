/* 
*   BSD 2-Clause License
*   Copyright (c) 2022, LiteEMF
*   All rights reserved.
*   This software component is licensed by LiteEMF under BSD 2-Clause license,
*   the "License"; You may not use this file except in compliance with the
*   License. You may obtain a copy of the License at:
*       opensource.org/licenses/BSD-2-Clause
* 
*/

/************************************************************************************************************
**	Description:	
************************************************************************************************************/
#include  "api/api_commander.h"
#include  "api/api_transport.h"
#include  "utils/emf_utils.h"

#include  "api/api_log.h"

void api_commander_test(void)
{	
	uint8_t i;
	uint32_t cmd = 0xcc;
	uint8c_t mtu = 10;
	uint8_t test_buf[] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c};
	uint8_t packet[30];


	//pack
	memset(&packet,0,sizeof(packet));
	logd("\ncommander_test:\n");
	logd("buf:"); dumpd(test_buf,sizeof(test_buf));
	api_command_pack(cmd, mtu, test_buf,sizeof(test_buf),packet, sizeof(packet));
	logd("pack:");dumpd(packet,sizeof(packet));

	//unpack
	command_stream_rx_t stream;
	memset(&stream,0,sizeof(stream));
	for(i=0; i<sizeof(packet); i++){
		if(api_command_byte_unpack(&stream, mtu, packet[i])){
			logd("unpack %d:",stream.rx.max_len);
			dumpd(stream.rx.buf, stream.rx.len);
			command_rx_free(&stream.rx);
		}
	}

	//pack
	memset(&packet,0,sizeof(packet));
	logd("\ncommander arg test:\n");
	logd("buf:"); dumpd(test_buf,sizeof(test_buf));
	api_command_arg_pack(cmd, 0xaabb, mtu, test_buf,sizeof(test_buf),packet, sizeof(packet));
	// packet[20] = 0xa5;
	// packet[22] = 0x01;
	logd("pack:");dumpd(packet,sizeof(packet));

	//unpack
	for(i=0; i<sizeof(packet); i++){
		if(api_command_byte_unpack(&stream, mtu, packet[i])){
			logd("unpack %d:",stream.rx.max_len);
			dumpd(stream.rx.buf, stream.rx.len);
			command_rx_free(&stream.rx);
		}
	}
}






