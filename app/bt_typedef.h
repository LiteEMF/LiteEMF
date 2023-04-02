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


#ifdef __cplusplus
}
#endif
#endif

