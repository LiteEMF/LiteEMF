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

// @ref: https://android.googlesource.com/platform/packages/modules/adb/+/HEAD/protocol.txt
/************************************************************************************************************
**	Description:	
************************************************************************************************************/
#include  "hw_config.h"
#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_ADB)

#include "app/adb/app_adb.h"
#include "adb_auth_sign.h"
#include "api/usb/host/usbh.h"

#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/


/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/

enum {
    TRIED_NONE,
    TRIED_SIGNED,
    TRIED_PUBKEY
} auth;
/*****************************************************************************************************
**	static Function
******************************************************************************************************/

/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool adb_write(const void *buf, uint32_t length)
{
    if(ERROR_SUCCESS == usbh_class_out(DEV_TYPE_ADB, 0, (uint8_t*)buf, length)){
        return true;
    }
    return false;
}

bool adb_recv(void *buf, uint32_t length) 
{
    uint16_t len = length;

    if(ERROR_SUCCESS == usbh_class_in(DEV_TYPE_ADB,0, buf, &len,2)){  //delay 2ms
        return true;
    }

    return false;
}



uint32_t send_message(uint32_t command, uint32_t arg0, uint32_t arg1, uint8_t *buf, uint32_t length) {
    adb_msg_t message;

    uint32_t sum = 0;
    uint32_t count = length;
    uint8_t *x = buf;
    while (count-- > 0) {
        sum += *x++;
    }

    message.command = command;
    message.arg0 = arg0;
    message.arg1 = arg1;
    message.length = length;
    message.check = sum;
    message.magic = command ^ 0xffffffff;

    adb_write( &message, sizeof(message));
    if (length) {
        adb_write( buf, length);
    }
    return 0;
}

int read_message_data(adb_msg_t *message, void *buf) 
{
    adb_recv(message, sizeof(adb_msg_t));
    if (message->length > 0) {
        memset(buf, 0, MAX_PAYLOAD);
        adb_recv(buf, message->length);
        logd("arg0: %d, arg1: %d, buf: %s (", message->arg0, message->arg1, (char *) buf);
		dumpd(buf,message->length);
        logd(")\n");
    } else {
        logd("arg0: %d, arg1: %d\n", message->arg0, message->arg1);
    }
    return 0;
}




static void exec(adb_msg_t message, char *buf, uint32_t length, uint8_t *command) 
{
    static uint32_t local_id = 0;

    local_id++;
    logd("local_id: %d\n", local_id);
    send_message(A_OPEN, local_id, 0, command, length);
    read_message_data(&message, buf);
    if (message.command == A_OKAY) {
        uint32_t remote = message.arg0;
        logd("okay\n");
        while (1) {
            read_message_data(&message, buf);
            if (message.command == A_CLSE) {
                logd("close\n");
                send_message(A_CLSE, local_id, remote, NULL, 0);
                read_message_data(&message, buf);
                break;
            } else {
                send_message(A_OKAY, local_id, remote, NULL, 0);
            }
        }
    } else if (message.command == A_CLSE) {
        // shouldn't happen
        logd("close\n");
        send_message(A_CLSE, local_id, message.arg0, NULL, 0);
        read_message_data(&message, buf);
    } else {
        logd("unknown command: %08x\n", message.command);
    }
}


bool send_auth_response(uint8_t *token)
{
	
#if defined(ADB_RSA_ENABLED)
	uint8_t buf[0x100];
    uint32d_t len;
	#if ADB_DEBUG
	dumpd(token,TOKEN_SIZE);
	#endif
	len = adb_auth_sign((const unsigned char *)token, TOKEN_SIZE, (unsigned char *)buf);
	#if ADB_DEBUG
    D("signed size: %d %d\n", len,sizeof(KEY_ADB));
	#endif
    if (len == 0) {
        return 0;
    } else {
        return send_message(A_AUTH, ADB_AUTH_SIGNATURE, 0, (uint8_t *)buf, len);
    }
#else
	token = token;
	
#endif
	
}
bool app_adb_run(void)
{
    adb_msg_t message;
    char buf[MAX_PAYLOAD];
	uint8_t auth = TRIED_NONE;

    send_message(A_CNXN, A_VERSION, MAX_PAYLOAD, (uint8_t *)X_HOST, sizeof(X_HOST));
    read_message_data(&message, buf);
    while (message.command != A_CNXN) {

        if (message.command != A_AUTH) {
            logd("unknown command: %d\n", message.command);
            return 0;
        } else if (auth == TRIED_NONE) {
            #if ADB_AUTH_SIGN_ENABLE
            auth = TRIED_SIGNED;
            int len = adb_auth_sign((const uint8_t *)buf, message.length, (uint8_t *)buf);
            logd("signed size: %d\n", len);
            if (len < 0) {
                continue;
            } else {
                send_message(A_AUTH, ADB_AUTH_SIGNATURE, 0, (uint8_t *)buf, (uint32_t) len);
                read_message_data(&message, buf);
            }
            #else
            send_message(A_AUTH, ADB_AUTH_SIGNATURE, 0, (uint8_t*)KEY_ADB,sizeof(KEY_ADB));	
            read_message_data(&message, buf);
            #endif

        } else if (auth == TRIED_SIGNED) {
            auth = TRIED_PUBKEY;
            send_message(A_AUTH, ADB_AUTH_RSAPUBLICKEY, 0, (uint8_t *)KEY_ADB, sizeof(KEY_ADB));
            read_message_data(&message, buf);
        } else {
            logd("sent pubkey\n");
            return 0;
        }
    }
    logd("connected\n");

    exec(message, buf, sizeof(X_ADB_SHELL_CMD), (uint8_t *)X_ADB_SHELL_CMD);
    if (buf[1] == 'o') {
        exec(message, buf, sizeof(X_TCPIP), (uint8_t *)X_TCPIP);
    }

    return true;
}




/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_adb_init(void)
{
	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_adb_deinit(void)
{
	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void app_adb_handler(uint32_t period_10us)
{
    static timer_t adb_timer;

	if(m_task_tick10us - adb_timer >= period_10us){
    }
}

#endif






