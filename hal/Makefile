SRC = $(wildcard *.c)				#当前目录SRC
OBJ := $(SRC:.c=.o)
OBJ := $(OBJ:%=$(BUILD_DIR)/%)		#当前.o 文件目录改成$(BUILD_DIR)/*.o目录

#源文件目录
SUB_DIR =


all : $(OBJ) sub_make 


$(OBJ):$(BUILD_DIR)/%.o:%.c				#.c 编译生成.o
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

sub_make:
ifneq ($(SUB_DIR),)
	make -C $(SUB_DIR)
endif