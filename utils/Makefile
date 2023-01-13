

#$(shell basename $(shell pwd))		#当前文件夹
DIR_BASE = $(shell basename $(shell pwd))
#当前目录SRC
SRC = $(wildcard *.c)	
OBJ := $(SRC:.c=.o)
#当前.o 文件目录改成$(BUILD_DIR)/*.o目录
OBJ := $(OBJ:%=$(BUILD_DIR)/%)

#源文件目录
SUB_DIR := $(shell find ./ -maxdepth 1 -mindepth 1 -type d)
SUB_DIR := $(addprefix $(ROOT_DIR)/$(DIR_BASE)/,$(SUB_DIR))

all : $(OBJ) sub_make

#.c 编译生成.o
$(OBJ):$(BUILD_DIR)/%.o:%.c
	$(DEBUG) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

sub_make:
ifneq ($(SUB_DIR),)
	@for dir in $(SUB_DIR); do $(MAKE) -C $$dir ; done
endif