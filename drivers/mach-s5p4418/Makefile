CPATH	:= ${CPATH}/mach-s5p4418

OBJS	:= lcd/sparrow_dev.o \
		   lcd/sparrow_drv.o

all: $(OBJS)
$(OBJS) : %.o : %.c
	@echo [CC] $(CPATH)/$^
	@$(CC) $(X_CFLAGS) $(X_INCDIR) -c $^ -o $@
