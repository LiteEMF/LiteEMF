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

#ifndef	_bt_typedef_h
#define	_bt_typedef_h

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************************************
** Defined
*******************************************************************************************************/

//refence:https://btprodspecificationrefs.blob.core.windows.net/assigned-numbers/Assigned%20Number%20Types/Assigned%20Numbers.pdf
#ifndef BLE_APPEARANCE_UNKNOWN
#define BLE_APPEARANCE_UNKNOWN                                0 /**< Unknown. */
#define BLE_APPEARANCE_GENERIC_PHONE                         64 /* Generic Phone. */
#define BLE_APPEARANCE_GENERIC_COMPUTER                     128 /* Generic Computer. */
#define BLE_APPEARANCE_GENERIC_WATCH                        192 /* Generic Watch. */
#define BLE_APPEARANCE_WATCH_SPORTS_WATCH                   193 /* Watch: Sports Watch. */
#define BLE_APPEARANCE_GENERIC_CLOCK                        256 /* Generic Clock. */
#define BLE_APPEARANCE_GENERIC_DISPLAY                      320 /* Generic Display. */
#define BLE_APPEARANCE_GENERIC_REMOTE_CONTROL               384 /* Generic Remote Control. */
#define BLE_APPEARANCE_GENERIC_EYE_GLASSES                  448 /* Generic Eye-glasses. */
#define BLE_APPEARANCE_GENERIC_TAG                          512 /* Generic Tag. */
#define BLE_APPEARANCE_GENERIC_KEYRING                      576 /* Generic Keyring. */
#define BLE_APPEARANCE_GENERIC_MEDIA_PLAYER                 640 /* Generic Media Player. */
#define BLE_APPEARANCE_GENERIC_BARCODE_SCANNER              704 /* Generic Barcode Scanner. */
#define BLE_APPEARANCE_GENERIC_THERMOMETER                  768 /* Generic Thermometer. */
#define BLE_APPEARANCE_THERMOMETER_EAR                      769 /* Thermometer: Ear. */
#define BLE_APPEARANCE_GENERIC_HEART_RATE_SENSOR            832 /* Generic Heart rate Sensor. */
#define BLE_APPEARANCE_HEART_RATE_SENSOR_HEART_RATE_BELT    833 /* Heart Rate Sensor: Heart Rate Belt. */
#define BLE_APPEARANCE_GENERIC_BLOOD_PRESSURE               896 /* Generic Blood Pressure. */
#define BLE_APPEARANCE_BLOOD_PRESSURE_ARM                   897 /* Blood Pressure: Arm. */
#define BLE_APPEARANCE_BLOOD_PRESSURE_WRIST                 898 /* Blood Pressure: Wrist. */
#define BLE_APPEARANCE_GENERIC_HID                          960 /* Human Interface Device (HID). */
#define BLE_APPEARANCE_HID_KEYBOARD                         961 /* Keyboard (HID Subtype). */
#define BLE_APPEARANCE_HID_MOUSE                            962 /* Mouse (HID Subtype). */
#define BLE_APPEARANCE_HID_JOYSTICK                         963 /* Joystick (HID Subtype). */
#define BLE_APPEARANCE_HID_GAMEPAD                          964 /* Gamepad (HID Subtype). */
#define BLE_APPEARANCE_HID_DIGITIZERSUBTYPE                 965 /* Digitizer Tablet (HID Subtype). */
#define BLE_APPEARANCE_HID_CARD_READER                      966 /* Card Reader (HID Subtype). */
#define BLE_APPEARANCE_HID_DIGITAL_PEN                      967 /* Digital Pen (HID Subtype). */
#define BLE_APPEARANCE_HID_BARCODE                          968 /* Barcode Scanner (HID Subtype). */
#define BLE_APPEARANCE_GENERIC_GLUCOSE_METER               1024 /* Generic Glucose Meter. */
#define BLE_APPEARANCE_GENERIC_RUNNING_WALKING_SENSOR      1088 /* Generic Running Walking Sensor. */
#define BLE_APPEARANCE_RUNNING_WALKING_SENSOR_IN_SHOE      1089 /* Running Walking Sensor: In-Shoe. */
#define BLE_APPEARANCE_RUNNING_WALKING_SENSOR_ON_SHOE      1090 /* Running Walking Sensor: On-Shoe. */
#define BLE_APPEARANCE_RUNNING_WALKING_SENSOR_ON_HIP       1091 /* Running Walking Sensor: On-Hip. */
#define BLE_APPEARANCE_GENERIC_CYCLING                     1152 /* Generic Cycling. */
#define BLE_APPEARANCE_CYCLING_CYCLING_COMPUTER            1153 /* Cycling: Cycling Computer. */
#define BLE_APPEARANCE_CYCLING_SPEED_SENSOR                1154 /* Cycling: Speed Sensor. */
#define BLE_APPEARANCE_CYCLING_CADENCE_SENSOR              1155 /* Cycling: Cadence Sensor. */
#define BLE_APPEARANCE_CYCLING_POWER_SENSOR                1156 /* Cycling: Power Sensor. */
#define BLE_APPEARANCE_CYCLING_SPEED_CADENCE_SENSOR        1157 /* Cycling: Speed and Cadence Sensor. */
#define BLE_APPEARANCE_GENERIC_PULSE_OXIMETER              3136 /* Generic Pulse Oximeter. */
#define BLE_APPEARANCE_PULSE_OXIMETER_FINGERTIP            3137 /* Fingertip (Pulse Oximeter subtype). */
#define BLE_APPEARANCE_PULSE_OXIMETER_WRIST_WORN           3138 /* Wrist Worn(Pulse Oximeter subtype). */
#define BLE_APPEARANCE_GENERIC_WEIGHT_SCALE                3200 /* Generic Weight Scale. */
#define BLE_APPEARANCE_GENERIC_OUTDOOR_SPORTS_ACT          5184 /* Generic Outdoor Sports Activity. */
#define BLE_APPEARANCE_OUTDOOR_SPORTS_ACT_LOC_DISP         5185 /* Location Display Device (Outdoor Sports Activity subtype). */
#define BLE_APPEARANCE_OUTDOOR_SPORTS_ACT_LOC_AND_NAV_DISP 5186 /* Location and Navigation Display Device (Outdoor Sports Activity subtype). */
#define BLE_APPEARANCE_OUTDOOR_SPORTS_ACT_LOC_POD          5187 /* Location Pod (Outdoor Sports Activity subtype). */
#define BLE_APPEARANCE_OUTDOOR_SPORTS_ACT_LOC_AND_NAV_POD  5188 /* Location and Navigation Pod (Outdoor Sports Activity subtype). */
#endif


//edr class of device(COD)
//refence:http://bluetooth-pentest.narod.ru/software/bluetooth_class_of_device-service_generator.html
#ifndef BD_CLASS_MOUSE
#define BD_CLASS_WEARABLE_HEADSET	0x240404/*ios10.2 display headset icon*/
#define BD_CLASS_HANDS_FREE			0x240408/*ios10.2 display bluetooth icon*/
#define BD_CLASS_MICROPHONE			0x240410
#define BD_CLASS_LOUDSPEAKER		0x240414
#define BD_CLASS_HEADPHONES			0x240418
#define BD_CLASS_CAR_AUDIO			0x240420
#define BD_CLASS_HIFI_AUDIO			0x240428
#define BD_CLASS_PHONEBOOK			0x340404
#define BD_CLASS_PAN_DEV            0X020118
#define BD_CLASS_MOUSE              0x002580
#define BD_CLASS_KEYBOARD           0x002540
#define BD_CLASS_KEYBOARD_MOUSE     0x0025C0
#define BD_CLASS_REMOTE_CONTROL     0x00254C
#define BD_CLASS_GAMEPAD            0x002508
#define BD_CLASS_TRANSFER_HEALTH    0x10091C
#endif


// #define BLE_HIDS_REP_TYPE_INPUT     1
// #define BLE_HIDS_REP_TYPE_OUTPUT    2
// #define BLE_HIDS_REP_TYPE_FEATURE   3

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
#define BT_TR_NUM			8
#define BT_TRS_MASK			(BT_BLE | BT_EDR | BT_BLEC | BT_EDRC | BT_RF | BT_RFC | BT_BLE_RF | BT_BLEC_RF)
typedef enum {
	BT_NULL = 0,
	BT_BLE 	= BIT(TR_BLE),		
	BT_EDR 	= BIT(TR_EDR),		
	BT_BLEC = BIT(TR_BLEC),		
	BT_EDRC = BIT(TR_EDRC),		
	BT_BLE_RF = BIT(TR_BLE_RF),					//BLE模拟2.4G
	BT_BLEC_RF = BIT(TR_BLEC_RF),	
	BT_RF = BIT(TR_RF),			
	BT_RFC = BIT(TR_RFC),	
} bt_t;

typedef enum {
	BT_SERVER_NULL = 0,
	BT_UART = BIT(0), 
	BT_HID = BIT(1),		
	BT_HID_DESC = BIT(2),		
} bt_server_t;



//bt state machine
typedef enum  {
	BT_STA_UNKNOW	=	0,
	BT_STA_IDLE,
	BT_STA_ADV,
	BT_STA_DIR_ADV,
	BT_STA_SCAN		=	BT_STA_ADV,
	BT_STA_DIR_SCAN	=	BT_STA_DIR_ADV,
	BT_STA_CONN,
	BT_STA_READY,
} bt_sta_t;


typedef enum {
    BT_EVT_INIT=0, 								//	(pa)
	BT_EVT_IDLE,								//idle
	
	BT_EVT_ADV,									//广播中(edr进入通用/限制可发现状态)
    BT_EVT_ADV_DIR,                     		//广播(edr 处于定向回连)
	BT_EVT_ADV_TIMOUT,							//广播超时
	BT_EVT_SCAN = BT_EVT_ADV,					//扫描	(pa)
	BT_EVT_SCAN_DIR = BT_EVT_ADV_DIR,			//定向扫描
	BT_EVT_SCAN_TIMEOUT = BT_EVT_ADV_TIMOUT,	//扫描超时

	BT_EVT_CONNECTED,							//连接成功	(pa)
	BT_EVT_DISCONNECTED,						//断开连接	(pa)
	BT_EVT_CONNECT_FAIL,						//(回连)连接失败	(pa)
	BT_EVT_READY,								//蓝牙notyif打开,可以接收数据

	BT_EVT_RX,									//主机发送消息, (pa)
	BT_EVT_TX,									//READ 事件
	BT_EVT_TX_COMPLETE,							//发送完成
} bt_evt_t;

typedef struct{
	uint8_t bonded; 
}bt_evt_init_t;
typedef struct{
	int8_t rssi; 		//RSSI值
	uint8_t mac[6];
	uint8_t name[31];
}bt_evt_scan_t;
typedef struct{
	uint8_t mac[6];
	uint8_t name[31];
}bt_evt_con_t;
typedef struct{
	uint8_t resion;
}bt_evt_discon_t,bt_evt_fail_t;
typedef struct{
	bt_server_t bts;
	uint8_t* buf;
	uint16_t len;
}bt_evt_rx_t;
typedef struct{
	bt_server_t bts;
}bt_evt_tx_t;

typedef union{
	bt_evt_init_t init;
	bt_evt_scan_t scan;
	bt_evt_con_t con;
	bt_evt_discon_t discon;
	bt_evt_fail_t fail;
	bt_evt_rx_t rx;
	bt_evt_tx_t tx;
}bt_evt_pa_t;



#ifdef __cplusplus
}
#endif
#endif

