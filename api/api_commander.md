# commander protocol

## packet format

> 数据包帧(command frame):

> | HEAD | LEN | INDEX | CMD |    DATA    | CHECKSUM |
> | :--: | :-: | :---: | :-: | :--------: | :------: |
> |  1B  | 1B |  1B  | 1B | MTU-5(max) |    1B    |

* HEAD: 包头, bit0: 1:表示起始包,0:普通包
* LEN: 包长度HEAD+LEN+CMD+INDEX+DATA+CHECKSUM的长度
* INDEX:第一包: 表示要发总包数
  后续包: 表示包序号从1开始递增| HEAD  | 0XA5   | 0XA4   |
  | ----- | ------ | ------ |
  | INDEX | 总包数 | 包序号 |
* CMD: 指令固定一个字节
* DATA: 数据长度范围0~(MTU-5)
* CHECKSUM: 校验: HEAD+LEN+CMD+INDEX+DATA 的校验和

## packet example

以下例子MTU=10:

* > 发送数据:*0x01 0x02 0x03 0x04 0x05 0x06 0x07 0x08 0x09 0x0a 0x0b 0x0c*
  >
  >> `0xa5 0x0a 0x03 0xcc 0x01 0x02 0x03 0x04 0x05 0x8d`
  >> `0xa4 0x0a 0x01 0xcc 0x06 0x07 0x08 0x09 0x0a 0xa3`
  >> `0xa4 0x07 0x02 0xcc 0x0b 0x0c 0x90`
  >>
  >
