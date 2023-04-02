#当前目录SRC
SRC = $(wildcard *.c)	
#当前.o 文件目录改成$(BUILD_DIR)/*.o目录
OBJ = $(patsubst %.c, $(BUILD_DIR)/%.o,$(SRC))
# OBJ := $(SRC:.c=.o)
# OBJ := $(OBJ:%=$(BUILD_DIR)/%)
DIR_BASE = $(shell pwd)
#源文件目录
SUB_DIR = $(shell find $(DIR_BASE) -maxdepth 1 -mindepth 1 -type d)


.PHONY: all  $(OBJ) $(SUB_DIR)

.PHONY: all $(SUB_DIR)

all : $(OBJ) sub_mudule

#.c 编译生成.o
$(OBJ):$(BUILD_DIR)/%.o:%.c
	$(DEBUG) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

sub_mudule:
#使用for循环防止Makefile文件不纯在编译出错
ifneq ($(SUB_DIR),)
	@for dir in $(SUB_DIR); do $(MAKE) -C $$dir ; done
endif