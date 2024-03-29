
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


#ifndef _adb_auth_sign_h
#define _adb_auth_sign_h
#include "emf_typedef.h" 

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ADB_AUTH_SIGN_ENABLE
#define ADB_AUTH_SIGN_ENABLE 	0
#endif


#define SIGN_SIZE 256
#define SHA1_SIZE 20

#define KEY_P "\xeb\x19\xf5\x25\xaf\x0f\x51\xba\xae\x18\x50\x96\xd4\x05\x14\xe1\x9a\xc0\x28\x10\xd5\x89\xd2\x98\x5b\xf3\xa0\x51\x77\x8e\x4e\x3e\xea\x72\xc2\x4b\xd0\x2b\xbb\xa7\xdd\xf0\xa9\x65\x19\xa9\xeb\xba\x57\x03\x27\xf2\xfb\x7c\x0c\xea\xbc\x2d\x37\x60\xf2\x3d\x17\x85\xb4\xc1\xf2\xf6\xb1\xf5\x39\x9b\xbe\xa7\x02\x56\xb6\x50\x73\xc3\x37\xf8\x85\x80\x79\x12\x9e\xdb\xe0\xec\x73\x15\xcf\x38\x7f\xe1\xf2\x7d\x27\x18\xc1\x7b\x89\x58\x03\xb4\xaf\x03\xf5\xbc\xe6\x05\x69\x5b\xfe\x20\x3f\x81\x0b\xf5\xe6\xa7\x08\xd9\x05\xc9\x33\x87"
#define KEY_Q "\xc5\x89\x14\x98\xbe\x9a\x0a\x32\x61\xa5\xd7\x69\xed\x2b\x2a\x6a\x4c\xf7\xd2\x65\x4a\x1f\x31\x45\x36\xf1\x9e\x87\xd0\x33\xa0\x5a\xb0\x97\xad\x2e\x54\xe4\x56\xb1\x7a\xab\xfa\xde\x99\x03\x65\x2a\xd3\x7b\x9b\x8d\xc3\x9e\xd4\xf5\x8b\x6f\xe5\x0c\x6f\x02\xdb\x22\x4c\x63\x28\xab\x81\x36\x2e\x62\x0f\xab\xd1\xb8\xee\x0c\x85\xa2\xec\xb2\xce\x5e\x77\x6b\x12\xed\x24\x81\x11\xce\x22\x51\x02\x57\x6d\xf2\x42\x5f\xeb\x4e\x12\x39\xdb\x77\x7c\xc9\x3d\x9f\x33\x7d\xde\x50\x58\x82\x7f\xd9\x56\x13\x7e\x57\xfb\xb4\x57\x8e\xbe\xab"
#define KEY_DP "\xb4\xf2\x88\x9d\x15\x6c\xdc\x01\xab\x87\x29\x57\xbd\x1d\xa0\x21\xb7\xe2\x97\xb3\xc5\x36\xcb\xb0\xda\x6c\xdb\x82\x78\x5b\x3d\xdb\xec\xc4\xa0\xc8\x8b\x13\x6d\xde\x58\xd2\xa6\xfe\x1a\x4f\x8f\x10\x1f\x72\x5d\x68\x05\xa9\x47\xa3\xcf\x6b\x99\xc8\xf3\x08\x94\x37\x23\x95\x77\x33\xea\x6a\x0c\x89\xd3\xa6\x65\x05\x6a\x52\xe7\x96\x64\x58\xd2\x1f\xbc\x8f\xd9\x56\xf0\x79\xec\x65\xe7\xd4\xdb\x90\x82\xfb\x52\x81\x1a\x41\xc3\xc6\xcb\x2b\x60\xdc\xd2\x61\x2e\x45\x11\xff\x6c\xcb\x53\x35\x36\x88\x6f\x9b\x36\x13\x1b\x01\xed\xb7"
#define KEY_DQ "\xc1\x9e\xc1\xef\x8e\x34\x12\xf6\x9b\x3c\x21\x4f\x94\xc7\x10\xc6\x2d\x37\x64\xa5\x1f\xe8\x3f\x35\x26\xcb\xe2\xa6\x20\xef\xd9\xa7\xcb\x1c\x81\x05\xbe\xf4\x29\xfd\xc0\xce\x3e\x12\x5e\x4d\x33\xa6\xac\xb0\xa6\x22\x68\xb2\x90\xef\x35\x83\xb2\x31\x63\xc6\x9a\xe0\x9d\x9b\x3c\x4c\x0f\x54\xfa\x55\xe8\x22\x74\x69\x1d\xfc\x6c\x28\xec\x75\xbf\x4e\x5d\x23\x38\xe3\xf5\x3a\x06\x27\xd4\xff\x2e\x9a\x60\x0c\x6b\xfe\xf4\xf8\x0b\x69\x66\xb3\x52\x3f\x37\x05\x8b\x45\x3e\xd9\xe6\x47\xd3\xe5\x46\x37\xc9\x49\x98\x50\x86\xda\x46\x0d"
#define KEY_QP "\xb5\xeb\xb5\x7e\x56\x91\x56\xaf\xb8\xc3\x73\xf2\x56\x2e\x24\x7f\x4b\x96\xe0\x2c\xa6\x42\xd9\x47\xf7\x79\xd7\xcc\xb7\xea\x87\xee\xe2\xc3\x5f\xbb\xc3\x21\xb2\xf6\xaa\x5a\x6d\x15\x39\xda\xfe\x9d\x14\x76\x4d\x99\xec\xed\xd5\x97\x8c\xb2\x0a\xa9\xf2\xbf\x6d\x00\xf4\x7f\x27\x40\x06\x0c\xc8\x9b\xe5\x73\xa7\x8f\x69\x70\xb9\xb8\xe4\x06\x68\xbf\xa9\x3a\x70\xb4\x52\xbf\xe6\x62\x50\x48\x2a\x90\xdf\x79\xb1\x8f\x33\xb5\x91\xd1\x38\x6e\x4e\x40\xcc\xfa\x54\x36\x2d\x9e\xe6\x9c\x5d\x64\x72\x2f\x48\x6e\xbd\x6a\xd2\x6e\x87\x87"
#define KEY_RP "\x64\x46\xab\x36\x0b\x91\xc9\x8a\xc8\xc0\xd5\x01\x32\x1a\x15\x8b\x7c\xbb\xb5\x78\x2d\x9c\x21\xae\x29\x2a\x5a\x1f\x6d\x4b\x62\xbe\x74\x7c\xab\x53\xbd\xd9\x7d\xb7\xeb\x83\x53\x14\xad\xc3\x96\x18\x9c\x4f\xf3\xe0\x47\xd8\xf1\x29\x3c\xa7\x33\xab\x82\x13\x68\x3a\xc7\xa3\x79\x87\x53\xad\x63\x33\xdd\xdf\x5e\x03\x86\xd3\xfb\x19\xf0\x5c\x10\xbe\x6b\x3f\xc6\x9e\xbb\xf5\x66\x7b\x0f\x33\x2e\xe6\x54\x5f\x6b\x86\xb4\xbb\x81\x01\x46\xe1\xac\x41\xbb\x4e\x23\xc6\xbf\xde\x29\x49\x99\xd6\x6e\xea\x94\x08\xf4\x76\xcd\x27\x96\x86"
#define KEY_RQ "\x55\x8f\x4f\x7c\xf1\xb5\x6b\x80\x87\x55\xe2\xdb\xae\x07\x06\xe2\x94\x2b\x60\xcb\x9e\xb0\xe9\x54\x9f\xb9\xcb\x1d\x2f\x77\x59\x58\x4f\xd7\x9a\x8c\x3c\xc1\x04\x00\x3b\x46\x17\xb7\xba\x9f\x87\x84\x33\xbd\x3f\xdc\x7e\xf7\x47\xd2\x2f\xfa\x97\x8c\x7c\x68\xaf\xb0\x6e\x30\x12\x81\x1f\x8a\x83\xfe\x85\x70\xaf\xb8\xd0\x85\xd4\xbd\x78\x42\x98\xbb\x81\x98\xe3\x6b\x3d\xcc\x77\x54\x01\x9b\x01\x33\xac\x42\x6c\x30\xfc\x3e\x6c\x19\x65\x85\x07\xa0\x8a\x08\xa2\xbf\x35\xe3\x1d\x87\xf3\x01\xd7\x53\x3f\xa6\x72\xac\xa0\x19\xc6\x71"
#define KEY_CP "\xab\x34\xd3\x9e\x23\x83\x12\xfb\xb0\x54\xae\x51\x3f\xaf\x87\x84\xe4\xb3\xdf\xa4\x8d\x30\x94\x5f\xce\x72\x1f\x97\x87\x7a\xaf\x79\x7e\x5f\x32\x8e\x63\xe3\x14\x7e\x1d\x82\x53\xa1\x91\x75\xa4\x7c\x02\x06\x38\xfb\x4c\xfa\xc9\x69\xe2\x12\xb6\x9e\x34\x3d\x99\x03\xe2\x32\xbb\xa0\xb0\x6d\xe6\xaa\x3c\xa1\x81\xe6\xb9\x12\x6b\xb4\xa3\x3a\x0e\x8f\xcb\xa7\x71\xc6\x53\x6c\x27\x48\x32\xe8\xb8\x56\x84\x53\xe9\x1e\x60\x15\xcd\x78\xea\xd5\x69\x3a\x0e\xba\x61\x6d\x40\x62\xc9\x2e\xf6\x51\xc0\xce\x93\x27\xd2\xf7\x52\xb0\x3a\xa1"
#define KEY_CQ "\x32\x6d\xeb\x8a\xa5\x12\x02\x29\x37\x82\x7e\xab\x66\x2c\xb2\xff\xf4\xaf\x23\xe2\x71\x05\x37\xdb\xb7\xf1\xe9\xa5\xcd\xd6\xac\xcc\x19\x69\x37\xfa\xda\xf0\x4b\xcb\x92\x8b\x41\xb4\xe6\x8b\x1d\x20\x7f\xc9\x23\x6e\x2e\x3b\x17\xd7\xc6\x5e\xce\x73\xdd\xa7\xb9\x76\x1e\xd4\xe4\x8f\xfd\x1c\x6d\xe8\x3c\x71\x2d\x89\x0e\xe6\x7f\x61\x02\xdb\x8d\xc7\xba\xff\x2e\xa4\xa4\x7a\x16\x7d\x88\x06\xc6\x79\x11\x14\x87\xb5\x96\xdc\x86\xa9\xf6\x43\x4d\xcf\x06\xa6\x2a\x1c\x55\x30\xfa\x7e\x7b\x67\x07\x46\xe4\x95\x41\x24\x2c\xfb\x46\xcc"

#define KEY_ADB "QAAAAFtFNG8tnYWnSylb/x9gQtczuVqDRBNIEd17DxncZrylZnkjhLAqJF8uImSy9eBP6R16O2MA6jwqd7odrlRLsZC20+EzNYf+Xebm0gN4HMoTzW+zM639el/FPr98EgI8igaPOolUM/r35BXjRhstZ+FW7hv61ZtZe0yF1C+AXp/mniDA9wHo3VXejm1l1hUKi+joJ/0QsQ47GsxB0yJ08KKbsxtFocjMJxUOgnIkIpX0GnucFONyAS0eqZtiL087lWYz9YH7ckwPScdw3o0iw+B/uMrdGU2gY/8dmyjh6XyCZdfla57ENKPqvEtEXswoZ5j0tncxi0JV/ShGQRG0fH503Wi1U2HjuNZv6xKZx50iLNLCPTw5JZOU+RPUmY7/w+Z9omymLQnayec1tgZaG4ywuZiU6unozwJCoCp/eVKkccaPHLLyHL5btlR6Dky+tP17pCOz55SUa2ojA2GUZrpkUHZqNWo10bBdi/QzKaBnedDXBsnPJAi5jgh+5cLEGwGoan3YmUjgTwcEEN+wt8kRIrUWVooFwK4zTDAh0YZj8tf7zgVkqc4/C50uApg6S1fdUAby57nG7R9YFiLPFWsw2RqAU3VEu3QNFBAHOSJsZJtrWo5ZcDFEU5YtFX60LBj+LVlubYjzHegvQmM1QF5K1aeGDFbXbbjo/BY7rHXSDHMgQgEAAQA= emf"


int adb_auth_sign(const unsigned char *token, size_t token_size, unsigned char *sig);

#ifdef __cplusplus
}
#endif
#endif

