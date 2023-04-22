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
	uint8_t test_buf[] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c};
	uint8_t packet[30] = {			//text MTU = 10
		0xa5, 0x0a, 0x03, 0xcc, 0x01, 0x02, 0x03, 0x04, 0x05, 0x8d,
		0xa4, 0x0a, 0x02, 0xcc, 0x06, 0x07, 0x08, 0x09, 0x0a, 0xa4,
		0xa4, 0x07, 0x01, 0xcc, 0x0b, 0x0c, 0x8f,
	};
	trp_handle_t handle={TR_UART, 0 ,0};


	//tx
	logd("\ncommander_tx test:\n");
	logd("buf:"); dumpd(test_buf,sizeof(test_buf));
	api_command_tx(&handle, cmd, test_buf,sizeof(test_buf));
	
	logd("\ncommander arg tx test:\n");
	api_command_arg_tx(&handle, cmd, 0xaabb, test_buf,sizeof(test_buf));

	//rx
	command_rx_t rx;
	static uint8_t s_cmd_buf[UART_CMD_MTU];
	static uint8_t s_cmd_len = 0;
	memset(&rx, 0, sizeof(rx));
	
	logd("\ncommander_rx test:\n");
	for(i=0; i<sizeof(packet); i++){
		if(api_command_rx_byte(&rx, UART_CMD_MTU, packet[i], s_cmd_buf, &s_cmd_len)){
			logd("unpack %d:",rx.len); dumpd(rx.pcmd, rx.len);
			command_rx_free(&rx);
		}
	}

}




