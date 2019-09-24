#OBJS	:=  mach-s5p4418/lcd/sparrow_dev.o
#OBJS	+= 	mach-s5p4418/lcd/sparrow_drv.o
#OBJS	+= 	base/platform.o
#
#X_INCDIR += -I base
#
#all : $(OBJS)
#$(OBJS) : %.o : %.c
#	@echo [CC] $@
#	@$(CC) $(X_CFLAGS) $(X_INCDIR) -c $^ -o $@
WORK_DIR	:= base mach-s5p4418
CPATH		:= ${CPATH}/drivers

subdirs: $(WORK_DIR)
$(WORK_DIR):
	@make -s -C $@

.PHONY: subdirs $(WORK_DIR)
