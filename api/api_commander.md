# commander protocol

## release note

* v1.0: original version

## 协议规范
    本协议统一使用`小端`方式通讯!
    协议中结构体都是单字节对齐!

## packet format

通讯协议需要上层协商通讯最大MTU(单包最大长度),确定MTU后指令需要根据MTU大封包.

> 数据包帧(command frame):

> | HEAD | LEN | PACK INDEX | CMD |    DATA    | CHECKSUM |
> | :--: | :-: | :--------: | :-: | :--------: | :------: |
> |  1B  | 1B |     1B     | 1B | MTU-5(max) |    1B    |

* HEAD: 包头, bit0: 1:表示起始包,0:普通包
* LEN: 包长度HEAD+LEN+CMD+INDEX+DATA+CHECKSUM的长度
* PACK INDEX: 包序号倒序, 包序号从最大包数量~1递减, 如果只有一包数据发送1
* CMD: 指令固定一个字节
* DATA: 数据长度范围0~(MTU-5)
* CHECKSUM: 校验: HEAD+LEN+CMD+INDEX+DATA 的校验和

## packet example

以下例子MTU=10:

* > 发送数据:*0x01 0x02 0x03 0x04 0x05 0x06 0x07 0x08 0x09 0x0a 0x0b 0x0c*
  >
  >> `0xa5 0x0a 0x03 0xcc 0x01 0x02 0x03 0x04 0x05 0x8d`  
  >> `0xa4 0x0a 0x02 0xcc 0x06 0x07 0x08 0x09 0x0a 0xa4`  
  >> `0xa4 0x07 0x01 0xcc 0x0b 0x0c 0x8f`  
  >>
  >

## dev mode

## command

### 基本数据结构:

```c
typedef enum{
  ERROR_SUCCESS     = 0 ,     //< Successful command
  ERROR_FAILE       = 1 ,     //FAILE
  ERROR_DISCON      = 2 ,     //DISCONNECTED
  ERROR_NOT_FOUND   = 3 ,     //< Not found
  ERROR_STALL       = 4 ,     // ERR STALL
  ERROR_BUSY        = 5 ,     //< Busy
  ERROR_TIMEOUT     = 6 ,     //< Operation timed out
  ERROR_NULL        = 7 ,     //< Null Pointer
  ERROR_NO_MEM      = 8 ,     //< No Memory for operation
  ERROR_PARAM       = 9 ,     //< Invalid Parameter
  ERROR_STATE       = 10 ,     //< Invalid state, operation disallowed in this state
  ERROR_LENGTH      = 11 ,     //< Invalid Length
  ERROR_DATA        = 12 ,     //< Invalid Data
  ERROR_ADDR        = 13 ,     //< Bad Memory Address
  ERROR_UNSUPPORT   = 14 ,     //< Not supported
  ERROR_UNKNOW      = 15 ,     //< Busy
}error_t;


trp定义:
typedef enum{
    TR_BLE		= 0,
    TR_EDR		= 1,
    TR_BLEC		= 2,
    TR_EDRC		= 3,
    TR_BLE_RF	= 4,				//BLE模拟2.4G
    TR_BLE_RFC	= 5,
    TR_RF		= 6,
    TR_RFC		= 7,
    TR_USBD		= 8,
    TR_USBH		= 9,
    TR_UART		= 10,
    TR_MAX,
}trp_t;
```

### 获取EMF cmd版本号(0X01)

```c
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t sum;
};
//回复emf mommand 版本号
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t version;
    uint8_t sum;
};
```

### 获取设备唯一UUID(0X02)

```c
//获取设备UUID
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t sum;
};
//回复设备UUID
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t uuid[16];	//长度可变
    uint8_t sum;
};

```

### 获取设备版本号(0X03)

```c
//获取设备版本号
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t sum;
};
//回复设备版本号
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint32_t version;     //4个字节
    uint8_t sum;
};
```

### 获取设备设备model(0X04)

```c
//获取设备mode
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t sum;
};
//回复设备mode
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t mode[16];     //长度可变
    uint8_t sum;
};

```

### 获取设备通讯MTU大小(0X05)


```c
//获取设备指定通讯MTU
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    trp_t   trp;        //可省略,省略后获取的是当前通讯通道的MTU
    uint8_t sum;
};
//回复设备mode
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    trp_t   trp;
    uint8_t mtu;
    uint8_t sum;
};

```

### 预留(0X06,0X07)

### 设置/获取设备PID/VID码号(0X08)

```c
//获取设备 VID/PID
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t sum;
};
//设置设备 VID/PID
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint16_t vid;
    uint16_t pid;
    uint8_t sum;
};
//回复设备 VID/PID
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint16_t vid;
    uint16_t pid;
    uint8_t sum;
};

```

### 设置/获取设备SN码(0X09)

Serial Number

```c
//获取设备SN
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t sum;
};
//设置设备SN
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t sn[16];    //动态长度
    uint8_t sum;
};
//回复设备SN
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t sn[16];    //动态长度
    uint8_t sum;
};
```

### 设置/获取设备MAC地址(0X0A)

```c
//获取设备 mac 地址
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    trp_t   trp;        //0:ble , 1:edr
    uint8_t sum;
};
//设置设备 mac 地址
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t id;             //默认为0
    trp_t   trp;
    uint8_t mac[6];
    uint8_t sum;
};
//回复设备 mac 地址
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t id;             //默认为0
    trp_t   trp;
    uint8_t mac[6];
    uint8_t sum;
};
```

### 设置/获取设备EID号(0X0B)

Extended Identification Data 扩展id,用于区分相同款产品的颜色,外观等

```c

//获取设备EID
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t sum;
};
//设置设备EID
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t eid[16];    //动态长度
    uint8_t sum;
};
//回复设备EID
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t eid[16];    //动态长度
    uint8_t sum;
};
```

### 设置/获取设备名称(0X0C)

```c
//获取设备 名称
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t sum;
};
//设置设备 名称
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t name[32];   //动态长度
    uint8_t sum;
};
//回复设备 名称
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t name[32];   //动态长度
    uint8_t sum;
}
```

### 预留(0X0D,0X0E,0X0F)

---

### 设置/获取设备模式(0X10)

设备类型定义

```c
//dev type
#define DEF_DEV_TYPE_VENDOR				0  			
#define DEF_DEV_TYPE_AUDIO     			1
#define DEF_DEV_TYPE_CDC				2		/*Communications Device Class*/
#define DEF_DEV_TYPE_HID				3
#define DEF_DEV_TYPE_PRINTER   			4
#define DEF_DEV_TYPE_MSD   				5		/*mass storage devices*/
#define DEF_DEV_TYPE_HUB       			6
//7, 8
#define DEF_DEV_TYPE_ADB          		9
#define DEF_DEV_TYPE_AOA          		10
#define DEF_DEV_TYPE_USBMUXD	   		11
#define DEF_DEV_TYPE_IAP2				12
#define DEF_DEV_TYPE_AUTO				15
#define DEF_DEV_TYPE_NONE				16		/*type none 0 == ((uint16_t)BIT(DEV_TYPE_NONE)*/

//hid type
#define DEF_HID_TYPE_VENDOR		0
#define DEF_HID_TYPE_KB 		1
#define DEF_HID_TYPE_MOUSE    	2
#define DEF_HID_TYPE_CONSUMER   3
#define DEF_HID_TYPE_TOUCH		4
#define DEF_HID_TYPE_MT			5
#define DEF_HID_TYPE_GAMEPADE	6
#define DEF_HID_TYPE_X360		7
#define DEF_HID_TYPE_XBOX		8
#define DEF_HID_TYPE_SWITCH		9
#define DEF_HID_TYPE_PS3		10
#define DEF_HID_TYPE_PS4		11
#define DEF_HID_TYPE_PS5		12
//res
#define DEF_HID_TYPE_NONE 		16

```

指令

```c
//获取设备模式下设备模式
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t sum;
};
//设置设备下设备模式
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint16_t dev_trps;	    //设备传输类型
    uint16_t usb_hid_types;	//usb模式
    uint16_t bt_hid_types;	//蓝牙模式
    uint16_t rf_hid_types;	//2.4g模式
    uint8_t sum;
};
//回复设备 模式
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint16_t dev_trps_support;	//设备支持的传输类型
    uint16_t hid_types_support;	//设备支持的hid模式

    uint16_t usb_hid_types;	//usb模式
    uint16_t bt_hid_types;	//蓝牙模式
    uint16_t rf_hid_types;	//2.4g模式
    
    uint8_t sum;
};
```

### 设备控制 (0x11)

> 数据结构:

```c
#define CTRL_SUB_DEV	0X80	//控制从机单片机
typedef enum{
    CTRL_RESET=0,               //设备重新启动
    CTRL_STOP,                  //设备关机
	CTRL_BOOT,
}dev_ctrl_t;
```

> 指令:

```c
//设置控制指令
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    dev_ctrl_t ctrl;
    uint8_t sum;
};
//回复控制消息, 注意设备复位关机可能接收不到消息
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;			//0XFF
    dev_ctrl_t ctrl;
    error_t err;
    uint8_t sum;
};
```

### 设置/获取测试模式(0X12)

```c
//获取设备 测试模式状态
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t sum;
};
//设置设备 测试模式
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t test_trp;       //可以省略, 如果省略模式是通过当前通讯的协议发送测试指令
    uint8_t sum;
};
//回复设备 测试模式状态
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t test_trp;
    uint8_t sum;
};
```

### 配置保存配置指令 (0X1D)

> 注意: 设备保存配置需要时间, 一般在10~100ms以内回复保存状态
>
> 设备收到指令后先立即回复  `ERROR_BUSY `,等待设备保存执行后回复成功或者失败

```c
//设置设备 保存配置到flash
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t sum;
};
//回复设备 保存配置是否成功
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    error_t ret;
    uint8_t sum;
};
```

### 设置/获取心跳包(0x1E)

```c

//获取设备 心跳包频率
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t sum;
};
//设置设备 心跳包频率
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t heartbeat_ms;		//0:表示关闭心跳包
    uint8_t sum;
};
//回复设备 心跳包频率
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t heartbeat_ms;	//心跳包频率
    uint8_t sum;
};
```

### 恢复出厂设置(0X1F)

下发恢复出厂设置指令

```c
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t sum;
};
```


---

### 上报键鼠数据格式(0X20)

键鼠数据格式:

```c
typedef struct 
{
    uint8_t id;
    uint8_t but;
    int16_t x;
    int16_t y;
    int8_t w;
    int8_t res;		//占位
  
    uint8_t fn;
    uint8_t res;
    uint8_t key[6];
} app_km_t;
```

```c
//上报键鼠数据
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    app_km_t km;
    uint8_t sum;
};
```

### 上报手柄数据格式(0X21)

数据格式:

```c
typedef struct{ 
    int16_t x;
    int16_t y;
}axis2i_t;
typedef struct{
    int16_t x;
    int16_t y;
    int16_t z;
}axis3i_t;
typedef struct {
    uint32_t key;
    axis2i_t stick_l;
    axis2i_t stick_r;
    int16_t l2;
    int16_t r2;
    axis3i_t acc;
    axis3i_t gyro;
}app_gamepad_key_t;	//28
```
指令:

```c
//上报手柄数据
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    app_gamepad_key_t key;	//体感数据可以不发送
    uint8_t sum;
};
```

### 摇杆校准模式设置(0X22)

```c
//获取设备 校准状态
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t cal_dev;	//0: joystick, 1: imu
    uint8_t sum;
};
//设置设备 校准状态
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t cal_dev;	//0: joystick, 1: imu
    uint8_t cal_sta;	//校准状态
    uint8_t sum;
};
//上报设备 校准状态
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t cal_dev;	//0: joystick, 1: imu
    uint8_t cal_sta;	//校准状态
    uint8_t sum;
};
```

### 设置马达震动(0X23)

数据格式:

```c
typedef enum{
    RUMBLE_L = 0,
    RUMBLE_R,
    RUMBLE_SL,
    RUMBLE_SR,
    RUMBLE_MAX
}rumble_id;
typedef struct{
    uint8_t mode;
    uint8_t duty[RUMBLE_MAX];	//0~255
} rumble_t;
```

```c
//设置设备 马达震动
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    rumble_t rumble;
    uint8_t sum;
};
```

### LED灯驱控制(0X24)


```c

//获取 LED 灯状态
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t sum;
};
//设置设备 LED状态
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t period[n];
    uint8_t sum;
};
//回复设备LED 状态
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t period[n];
    uint8_t sum;
};
```
period 配置说明: 
0: 灭, 1: 亮, 其他:闪烁


### 设置RGB灯控制(0X25)

数据结构:

```c
typedef struct{
    uint32_t id_bit;  	//led灯id按位表示
    uint8_t mode;   	//部分模式不支持修改RGB颜色
    uint8_t att;	    //模式属性,不同模式下属性含义不一样
    uint8_t period; 	//0~255 周期单位100ms
    uint8_t bright;		//0~255 数值越大越亮,对应为HSV颜色模型的V

    uint8_t times;		//次数(部分模式支持)
    uint8_t rgb[n][3];  //rgb 数据组,顺序为r,g,b 根据模式来确定有多少组RGB数据
}rgb_t;  						//注意结构体采用单字节对齐
```

指令:

```c
//获取 RGB状态
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint32_t id_bit;	//注意只有相同模式rgb才能正常返回
    uint8_t sum;
};
//设置设备 RGB
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    rgb_t rgb;
    uint8_t sum;
};
//回复设备 RGB状态
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    rgb_t rgb;
    uint8_t sum;
};

```

### RGB全局亮度控制(0X26)

```c

//获取 RGB全局亮度
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t sum;
};
//设置设备 RGB全局亮度
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t bright;
    uint8_t sum;
};
//回复设备 RGB全局亮度
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t bright;
    uint8_t sum;
};
```

---

### SOCKET CMD(0XD0~D6)
socket 通讯指令

### 自定义协议封装(0XDF)

> 为了emf command 协议和sdk平台协议兼容,使用0XEF指令来封装SDK自定义协议

```c
//自定义协议封装写
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t vendor_buf[n];
    uint8_t sum;
};
//自定义协议封装回复
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;
    uint8_t vendor_buf[n];
    uint8_t sum;
};
```

### 用户自定义(0XE0~0XFE)


### 不支持该指令(0XFF)

> 注意: 如果不支持的指令接收不完整,不会回复

```c
//回复设备不支持的指令
typedef struct{
    uint8_t head;
    uint8_t len;
    uint8_t index;
    uint8_t cmd;			//0XFF
    uint8_t unsupport_cmd;
    error_t err;
    uint8_t sum;
};
```
