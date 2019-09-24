CPATH	:= $(CPATH)/nxp
OBJS 	:= nx_rstcon.o \
		   nx_gpio.o \
		   nx_clk.o \
		   nx_display.o \
		   nx_pwm.o \

all : $(OBJS)
$(OBJS) : %.o : %.c
	@echo [CC] $(CPATH)/$^
	@$(CC) $(X_CFLAGS) $(X_INCDIR) -c $^ -o $@
