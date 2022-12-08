# 
#  BSD 2-Clause License
#  Copyright (c) 2022, LiteEMF
#  All rights reserved.
#  This software component is licensed by LiteEMF under BSD 2-Clause license,
#  the "License"; You may not use this file except in compliance with the
#  License. You may obtain a copy of the License at:
#      opensource.org/licenses/BSD-2-Clause
# 
# 



# c compile
PREFIX = #arm-none-eabi-			#切换交叉编译链
CC = $(PREFIX)gcc
AR = $(PREFIX)ar 					#静态库链接
CFLAGS =
LDFLAGS =
# 选择优化等级:
# 1. gcc中指定优化级别的参数有：-O0、-O1、-O2、-O3、-Og、-Os、-Ofast。
# 2. 在编译时，如果没有指定上面的任何优化参数，则默认为 -O0，即没有优化。
# 3. 参数 -O1、-O2、-O3 中，随着数字变大，代码的优化程度也越高，不过这在某种意义上来说，也是以牺牲程序的可调试性为代价的。
# 4. 参数 -Og 是在 -O1 的基础上，去掉了那些影响调试的优化，所以如果最终是为了调试程序，可以使用这个参数。不过光有这个参数也是不行的，这个参数只是告诉编译器，编译后的代码不要影响调试，但调试信息的生成还是靠 -g 参数的。
# 5. 参数 -Os 是在 -O2 的基础上，去掉了那些会导致最终可执行程序增大的优化，如果想要更小的可执行程序，可选择这个参数。
# 6. 参数 -Ofast 是在 -O3 的基础上，添加了一些非常规优化，这些优化是通过打破一些国际标准（比如一些数学函数的实现标准）来实现的，所以一般不推荐使用该参数。
# 7. 如果想知道上面的优化参数具体做了哪些优化，可以使用 gcc -Q --help=optimizers 命令来查询，比如下面是查询 -O3 参数开启了哪些优化：
CFLAGS += -Og
#开关警告
CFLAGS += -W #-Wall #-W 
#是否开启优化掉未使用的函数和符号
LDFLAGS += -Wl,--gc-sections



# define root dir
TARGET = LiteEMF
TARGET_LIBA = $(addprefix $(TARGET), .a)
ROOT_DIR = $(PWD)
BUILD_DIR = $(ROOT_DIR)/build

#搜索所有的c文件，制作所有的.c文件依赖Obj
#PS:去掉终极目标的原始路径前缀并添加输出文件夹路径前缀(改变了依赖文件的路径前缀，需要重新指定搜索路径)
# SRC = $(shell find ./ -type f -iname "*.c")
# BUILD_OBJ = $(addprefix $(BUILD_DIR)/,$(notdir $(SRC:%.c=%.o)))	#生成的.o文件
BUILD_OBJ = $(wildcard $(BUILD_DIR)/*.o)		

# 头文件包含
INCLUDES = \
	-I $(ROOT_DIR) \
	-I $(ROOT_DIR)/hal \
	-I $(ROOT_DIR)/sdk_hal \
	-I $(ROOT_DIR)/utils

#源文件目录
SRC_DIR = \
	-C $(ROOT_DIR)/utils \
	$(ROOT_DIR)/app \
	$(ROOT_DIR)/hal \
	$(ROOT_DIR)/sdk_hal 

#export 编译器compile
export CC AR CFLAGS 
#export DIR
export ROOT_DIR BUILD_DIR INCLUDES		

.PHONY: all clean debug pre_build build
all: debug pre_build build link		#执行make指令默认参数是make all 

debug: 
	@echo  "-----------------------------------echo-----------------------------------"
	@echo "CC = $(CC)"
	@echo "SRC_DIR = $(SRC_DIR)	"
	@echo  "ROOT_DIR = $(ROOT_DIR)"
	@echo  "TARGET_LIBA = $(TARGET_LIBA)"
	
pre_build:debug
	@echo  "-----------------------------------pre_build-----------------------------------"
	mkdir -p $(BUILD_DIR)
	@echo "none"
build:pre_build 								#在$(SUB_DIR) 目录下面执行make,-C Change to DIRECTORY before doing anything.
	@echo  "-----------------------------------build-----------------------------------"
	@for dir in $(SRC_DIR); do $(MAKE) -C $$dir ; done

# 通过.o 链接生产目标执行文件	
link:build
	@echo  "-----------------------------------link-----------------------------------"
	$(AR) -rc $(TARGET_LIBA) $(BUILD_OBJ)
ifneq ($(PREFIX),arm-none-eabi-)
	$(CC) $(LDFLAGS) -o $(TARGET) $(BUILD_OBJ)
endif

clean:
	@echo  "-----------------------------------clean-----------------------------------"
	rm -rf $(BUILD_DIR)/*.o
	rm -f *.o $(TARGET) $(TARGET_LIBA)



