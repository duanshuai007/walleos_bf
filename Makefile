#
# Makefile for module.
#
CROSS		?= /home/duanshuai/4418/new_android/android/prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-
NAME		:= walle
SH			:= /bin/bash
SHELL_SCRIPT	:= make_config.sh 
SHELL_MKOBJ		:= make_obj.sh 
ROOT_DIR		:= $(shell pwd)
LIBNAME			:= libwalle.a
TARGET_DIR		:= debug
MUST_FIRST_O	:= start.o hello_world.o abort.o 
FULL_PATH_MFO	=
NO_MAKE_DIR		:= include debug
LIBS			:= include/libgcc.a
OUT_OBJ_DIR		:= debug/obj
OUT_BIN_DIR		:= debug/bin
PRO_CONFIGH		:= include/config.h
LD_FILE			:= arm.ld
LD_LOCATION		:= $(ROOT_DIR)
MKDIR			:= mkdir -p
CPATH			:= .
#
# Load variables of flag.
#
CFLAGS		:= -nostdinc -W -Wall -Wstrict-prototypes -fno-builtin -ffreestanding \
			   -Os -fno-stack-protector -g -fstack-usage -fno-toplevel-reorder \
			   -ffunction-sections -fdata-sections -Wl,--gc-sections\
			   -pipe \
			   -fno-common -ffixed-r9 -fno-short-enums -fno-strict-aliasing \
			   -I include \
			   -std=gnu99 \
			   -march=armv7-a -mtune=cortex-a9 \
			   -mfpu=vfpv3-d16 -mfloat-abi=softfp

#
#	设置OBJ目录和BIN目录
#
OBJS_DIR	= $(CPATH)/$(OUT_OBJ_DIR)
BIN_DIR		= $(CPATH)/$(OUT_BIN_DIR)

#
#	设置objdump所需要的源文件和目标文件
#
ELF_FILE	= $(CPATH)/$(OUT_BIN_DIR)/$(NAME).elf
OBJ_FILE	= $(NAME)
#
#   配置配置文件的定位，并根据配置文件生成设备能识别的.mk文件
#
CONFIG_H	:= $(CPATH)/$(PRO_CONFIGH)
CONFILES	:= $(patsubst %, %, $(CONFIG_H:.h=.mk))
#
#   包含配置文件(*.mk) 并根据CONFIG_SYS_TEXT_BASE生成LD_UBOOT_FLAGS
#
-include ${CONFILES}

ifneq ($(CONFIG_SYS_TEXT_BASE),)
LD_UBOOT_FLAGS := -Ttext $(CONFIG_SYS_TEXT_BASE)
endif
#
#	配置编译和链接选项
#
LDFLAGS		:= -g $(LD_UBOOT_FLAGS) -T $(LD_LOCATION)/$(LD_FILE)
OCFLAGS		:= -O binary
ODFLAGS		:=
SFLAGS		:= -march=armv7-a -mtune=cortex-a9 -mfpu=vfpv3-d16 -mfloat-abi=softfp 
INCDIRS		:=

#
# Add necessary directory for INCDIRS and SRCDIRS.
#

INCDIRS		+= 	include \
			drivers/base \
			env/include \
			env/include/nxp \
			env/include/mmu \
			env/include/fs \
			env/include/lcd \
			env/include/graphic \
			env/include/input \
			env/include/drivers \
			env/include/i2c \
			photograph \
			photograph/pic_opt \
			x_sample/hello_world \
 			net/include \
			env/lib

# You shouldn't need to change anything below this point.
#
AS		:= $(CROSS)gcc
CC		:= $(CROSS)gcc
LD		:= $(CROSS)ld.bfd
AR		:= $(CROSS)ar
OC		:= $(CROSS)objcopy
OD		:= $(CROSS)objdump

#
# X variables
#
X_SFLAGS	:= $(SFLAGS)
X_CFLAGS	:= $(CFLAGS)
X_LDFLAGS	:= $(LDFLAGS)
X_OCFLAGS	:= $(OCFLAGS)

ifneq ($(LIBS),)
X_LIBS		:= $(CPATH)/$(LIBS)
else
X_LIBS		:= 
endif

#
#	必须使用全路径才能在Makefile切换到其他文件夹时正确的找到头文件
#
X_INCDIR	:= $(patsubst %, -I $(ROOT_DIR)/%, $(INCDIRS))
X_OBJS		:= $(OBJS_DIR)/$(LIBNAME)
#
#	搜索生成的*.o文件，并将start.o hello_world.o abort.o排除在外
#	因为最后一个 | 在NO_AR的末尾处，所以在TOTAL_OBJS的tr前去掉了 |
#	grep -E "a|b|c" 能够搜素满足a或b或c的字符串
#
#	使用绝对路径搜索.o文件
#ALL_O_FILE	= $(shell find $(ROOT_DIR) -name *.o | tr "\n" " ")
#	使用相对路径搜索.o文件，原因是arm.ld中的start.o的地址必须与LD
#	中的地址相同。否侧会出现链接错误
ALL_O_FILE	= $(shell find -name *.o | tr "\n" " ")
FPM_O		= $(patsubst %, \%% , $(MUST_FIRST_O))
FULL_PATH_MFO	= $(filter $(FPM_O), $(ALL_O_FILE))
#NO_AR 			:= $(patsubst %, grep -v % |, $(MUST_FIRST_O))
#TOTAL_OBJS		= $(shell find -name *.o | $(NO_AR) tr "\n" " ")
TOTAL_OBJS	= $(filter-out $(FPM_O), $(ALL_O_FILE))
#
#	打印出当前目录的文件夹名并将include 和 debug文件夹排除在外
#	最后的tr不是必须的，是为了防止grep最后多出来的一个“|”而加上的
#
NO_MAKE 	:= $(patsubst %, grep -v % |, $(NO_MAKE_DIR))
#SUBDIRS = $(shell ls -l | grep ^d | awk '{if($$9 != "include" && $$9 != "debug") print $$9}')
SUBDIRS 	= $(shell ls -l | grep ^d | awk '{print $$9}' | $(NO_MAKE) tr "\n" " ")
#
#	导出环境变量给子Makefile使用
#
export AS CC X_SFLAGS X_CFLAGS X_INCDIR ROOT_DIR PRO_CONFIGH CPATH
#
#定义伪目标功能
#
.PHONY: subdirs $(SUBDIRS)
.PHONY: clean
all : $(CONFILES) $(NAME)
#
#	用于生成config.mk
#
$(CONFILES) : $(CONFIG_H)
	@echo "---工程[$(NAME)]开始编译---"
	@echo "---创建配置文件[$(CONFILES)]---"
	@$(SH) $(SHELL_SCRIPT) $(CONFIG_H) $(CONFILES)
#
#	生成目标文件
#
$(NAME) : $(X_OBJS)
	@echo "[CONFILES] = $(CONFILES)"
	@echo "[CONFIG_H] = $(CONFIG_H)"
	$(LD) $(X_LDFLAGS) -o $(BIN_DIR)/$@.elf $(FULL_PATH_MFO) $(X_OBJS) $(X_LIBS)
	$(OC) $(X_OCFLAGS) $(BIN_DIR)/$@.elf $(BIN_DIR)/$@.bin
	@$(SH) $(SHELL_MKOBJ) $(OD) $(ELF_FILE) $(OBJ_FILE)
	@echo "---工程[$(NAME)]编译完成---"
#
#	将编译好的.o文件变成一个.a库文件
#	$(X_OBJS) 需要等待$(SUBDIRS)执行完成才会开始执行
#
$(X_OBJS) : $(SUBDIRS)
	@echo [AR] $(X_OBJS)
	@$(AR) cr $(X_OBJS) $(TOTAL_OBJS)
#
#   用于进入子文件夹执行Makefile
#	-s 静默编译，不显示编译信息，此处是禁止显示进出文件夹的信息
#
subdirs: $(SUBDIRS)
$(SUBDIRS):
	@make -s -C $@
#
#	清空临时文件
#
clean:
	@rm -f $(CONFILES)
	@rm -f $(OBJS_DIR)/*
	@rm -f $(OBJ_FILE)
	@rm -f $(BIN_DIR)/*
	@find -name *.bin | xargs rm -f
	@find -name *.elf | xargs rm -f
	@find -name *.su | xargs rm -f
	@find -name *.o | xargs rm -f
	@echo "---工程[$(NAME)]清理完成---" 

sinclude $(shell $(MKDIR) $(OUT_OBJ_DIR) $(OUT_BIN_DIR))
