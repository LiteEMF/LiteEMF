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


#ifndef _app_adb_h
#define _app_adb_h
#include "emf_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/

#ifndef ADB_AUTH_SIGN_ENABLE
#define ADB_AUTH_SIGN_ENABLE 	1
#endif


#define MAX_PAYLOAD 1024


#define A_SYNC 0x434e5953
#define A_CNXN 0x4e584e43
#define A_OPEN 0x4e45504f
#define A_OKAY 0x59414b4f
#define A_CLSE 0x45534c43
#define A_WRTE 0x45545257
#define A_AUTH 0x48545541




#define A_VERSION 0x01000000        /* ADB protocol version */

#define ADB_VERSION_MAJOR 1         /* Used for help/version information */
#define ADB_VERSION_MINOR 0         /* Used for help/version information */

#define ADB_SERVER_VERSION    	31    /* Increment this when we want to force users to start a new adb server */


#define ADB_AUTH_TOKEN 			1
#define ADB_AUTH_SIGNATURE 		2
#define ADB_AUTH_RSAPUBLICKEY 	3
#define TOKEN_SIZE 				20


#define X_HOST "host::emf"
#define X_TCPIP "tcpip:3035"
#define X_ADB_SHELL_CMD \
"shell:pkg=me.piebridge.emf;\
 path=/data/data/$pkg/emf.sh;\
 comp=$pkg/.ui.BreventActivity;\
 url=https://br.jianyu.io;\
 start=0;\
 tcpip=`getprop service.adb.tcp.port`;\
 setprop log.tag.emf.dongle 1;\
 if [ ! -r $path ]; then\
 output=`pm path $pkg 2>/dev/null`;\
 if [ -z \"$output\" ]; then\
 am start -a android.intent.action.VIEW -d $url >/dev/null 2>&1;\
 if [ \"$tcpip\" == \"3035\" ]; then echo server_open; else echo server_close; fi;\
 exit 0;\
 fi;\
 for x in 0 1; do\
 am start -n $comp >/dev/null 2>&1;\
 start=1;\
 for y in 0 1 2 3 4; do\
 if [ -r $path ];then\
 break;\
 fi;\
 sleep 1;\
 done;\
 if [ -r $path ];then\
 break;\
 fi;\
 am force-stop $pkg >/dev/null 2>&1;\
 done;\
 fi;\
 if [ -r $path ];then\
 /system/bin/sh $path >/sdcard/emf.log 2>&1;\
 if [ $? -eq 0 ];then\
 rm -rf /sdcard/emf.log >/dev/null 2>&1;\
 if [ \"$start\" -eq 0 ];then\
 am start -n $comp >/dev/null 2>&1;\
 fi;\
 echo server_open;\
 exit 0;\
 fi;\
 fi;\
 if [ \"$tcpip\" == \"3035\" ]; then echo server_open; else echo zk_server_close; fi;\
 exit 1"
 
 


/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef struct {
    uint32_t command;       /* command identifier constant      */
    uint32_t arg0;          /* first argument                   */
    uint32_t arg1;          /* second argument                  */
    uint32_t length;        /* length of payload (0 is allowed) */
    uint32_t check;         /* checksum of data payload         */
    uint32_t magic;         /* command ^ 0xffffffff             */
}adb_msg_t;



/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool app_adb_run(void);
bool app_adb_init(void);
bool app_adb_deinit(void);
void app_adb_task(void* pa);
void app_adb_handler(uint32_t period_10us);

#ifdef __cplusplus
}
#endif
#endif





