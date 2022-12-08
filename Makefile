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
CC = g++
CFLAGS =
TARGET = LiteEMF

# define root dir
ROOT_DIR = $(PWD)
BUILD_DIR = $(ROOT_DIR)/build
BUILD_OBJ = $(wildcard $(BUILD_DIR)/*.o)		#生成的.o文件
# 头文件包含
INCLUDES = \
	-I$(ROOT_DIR) \
	-I$(ROOT_DIR)/hal \
	-I$(ROOT_DIR)/sdk_hal \
	-I$(ROOT_DIR)/utils
#源文件目录
SRC_DIR = \
	-C $(ROOT_DIR)/utils \
	$(ROOT_DIR)/app \
	$(ROOT_DIR)/hal \
	$(ROOT_DIR)/sdk_hal 

#export 编译器
export CC CFLAGS 
#export DIR
export ROOT_DIR BUILD_DIR INCLUDES		

.PHONY: all clean debug pre_build build
all: debug pre_build build link		#执行make指令默认参数是make all 

debug: 
	@echo  "-----------------------------------echo-----------------------------------"
	@echo "CC = $(CC)"
	@echo "SRC_DIR = $(SRC_DIR)	"
	@echo  "ROOT_DIR = $(ROOT_DIR)"
pre_build:
	@echo  "-----------------------------------pre_build-----------------------------------"
	mkdir -p $(BUILD_DIR)
	@echo "none"
build: 								#在$(SUB_DIR) 目录下面执行make,-C Change to DIRECTORY before doing anything.
	@echo  "-----------------------------------build-----------------------------------"
	@for dir in $(SRC_DIR); do $(MAKE) -C $$dir ; done
# 通过.o 链接生产目标执行文件	
link:
	@echo  "-----------------------------------link-----------------------------------"
	$(CC) $(CFLAGS) -o LiteEMF $(BUILD_OBJ)

clean:
	@echo  "-----------------------------------clean-----------------------------------"
	rm -rf $(BUILD_DIR)/*.o
	rm -f *.o $(TARGET)



