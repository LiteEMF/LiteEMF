# LiteEMF

EN | [中文](./README.md)

LiteEMF (Lite Embedded Middle Framework) is a general framework for embedded middleware, which integrates common modules and functions of embedded devices based on the HAL hardware abstraction layer.

LiteEMF is not an operating system and can run on any operating system or bare metal. 

LiteEMF specifies a series of standard API interfaces that are used to isolate the hardware and application layers through the HAL hardware abstraction layer. Porting a new platform only needs to adapt to the HAL layer, and the upper-layer development has nothing to do with the hardware. It allows developers to ignore hardware differences and spend more effort on product feature development. It can quickly port user programs to any chip and system, helping developers quickly achieve product development iteration.

LiteEMF In order to configure flexibly, users can configure through macro definition in the project to choose which functions to enable, the chip occupies FLASH, RAM space is determined according to the function usage, and 0 bytes can be occupied by completely off. 


> Framework:  
![这是图片](/doc/LiteEMF框架.jpg "LiteEMF")				

## LiteEMF origin

Embedded development is characterized by large differences in chips and many types of chip cores, and it is difficult to develop software completely out of hardware. Of course, there are many embedded system open source projects, and these open source projects are separated from hardware (e.g. MBED) for portability. Or an excellent open source platform like Arduino, which binds hardware and software SDKs together. But these more or less have some limitations in his field of application and itself, for example, mbed is an IOT platform launched by ARM that only supports ARM chips.

The goal of the LiteEMF project is to build a framework that can be easily used in your own hardware. Whether the chip is a 51 single-chip microcomputer, or arm, avr, risc-v, whether the SDK uses the main loop or Freertos, UcosII, Rtheard, Linux and other operating systems, you can quickly build a hardware basic input and output API interface through this framework, allowing users to focus on upper-level application development without hardware.

In addition to building a standard API to help users dock hardware, the upper layer should also provide rich standard protocol interfaces like mainstream IOT systems, and commonly used embedded modules (power management, buttons, RGB, USB, rumble, somatosensory sense, controller, Bluetooth, network, etc.), mainstream open source modules (such as nanopb), etc. 

At present, LiteEMF has done this, and has also successfully verified mass production products on 51 single-chip microcomputer, Arm and other chip platforms, and freeRtos, Linux operating system. 

Are you worried about changing the chip and reporting, whether you are worrying about the factory update SDK and requiring a lot of code changes, then come and adopt LiteEMF!

## Get started quickly

Welcome to the LiteEMF open source project, please refer to the detailed API interface[User manual](./%E7%94%A8%E6%88%B7%E6%89%8B%E5%86%8C.md)

## 编译

### Window compilation
LiteEMF can be compiled directly on the Windows platform, which can be used for code testing and Windows platform APP development. Note that the embedded hardware interface in the API layer cannot be called under the Windows platform. 

makefile compilation:
* download [MinGW for windows](https://sourceforge.net/projects/mingw-w64/files/mingw-w64/mingw-w64-release/)
* To install MinGW, it is recommended to install in the C:/MinGW" directory by default, if you install to other directories, you need to set environment variables. Modify MinGW gcc compiler directory[c_cpp_properties.json](.vscode/c_cpp_properties.json) and[launch.json](.vscode/launch.json) at the same time.
* vscode to open the code, select Terminal --Run Task --make, or Command Line to execute the make command
* Run the generated LiteEMF.exe

cmake compilation:
* Download [MinGW for windows] (https://sourceforge.net/projects/mingw-w64/files/mingw-w64/mingw-w64-release/)
* Terminal run instructions
```
mkdir build; cd build
cmake .. -G "Unix Makefiles"
make
```

### Embedded platform compilation
LiteEMF is ported and runs in embedded platforms

Migration steps:

* Download LiteEMF into your chip SDK, it is recommended to use the 'git submodule' method
* Copy the hal/ directory in LiteEMF to the chip SDK and rename it to xxx_hal/ (the LiteEMF/hal/ directory file is a reference file, and the hal layer of each platform needs to be adapted separately)  
In order not to destroy the original chip SDK, it is recommended to create new folders project/ and user/ directories
  * project/ Stores project files and project source files
  * user/ stores LiteEMF, and xxx_hal/, hw_confg.h, hw_board.h (these two headers are used for project management)  
![Recommended file directory](/doc/user文件目录.png)
* Create a new project, modify the project file or makefile file containing user/LiteEMF and user/xxx_hal (note: do not include the /user/LiteEMF/hal directory!)
* Write adaptation xxx_hal/ driver, add LiteEMF initialization and tasks to the project according to the routines in emf.c  
LiteEMF provides two ways: xxx_handler() function interface and xxx_task(*void) to invoke tasks
* Compiled according to chip SDK

## Platform support

* Support for bare metal systems
* Supported operating systems

## Future work plan

* USB BT module verification test
* Improve the API interface and provide more optimized APIs
* Officially adapted to more chip developers
* Port more open source projects to enrich APP layer applications
* Officially adapted to more peripheral drivers (NFC, GSM, SENSOR, etc.) for developers to use
* LVGL graphics library support
* Network interface support
* Several open source projects will also be implemented on top of LiteEMF, and the [QMK] (https://github.com/qmk/qmk_firmware) open source keyboard project is currently planned

## Communication

In order to improve the LiteEMF open source project and have more people to use, collaborators will also select suitable projects for 'free' port development for everyone. 

Welcome to use LiteEMF and join the QQ group exchange: 130611332

## Disclaimer

LiteEMF can be used as development, evaluation, and even mass production. LiteEMF is an open source project and does not assume any consequences arising from the use of these materials

