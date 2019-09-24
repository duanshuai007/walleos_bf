SUBDIRS := $(shell ls -l | grep ^d | awk '{print $$9}')
CPATH	:= $(CPATH)/lib
COBJS	:=	alloc.o \
			cache_cp15.o \
			cache_v7.o \
			debug.o \
			do_interrupt.o \
			errno.o \
			s5p4418_clk.o \
			s5p4418_gpio.o \
			s5p4418_interrupt.o \
			s5p4418_led.o \
			s5p4418_pwm.o \
			s5p4418_rstcon.o \
			s5p4418_serial.o \
			s5p4418_serial_stdio.o \
			s5p4418_tick_delay.o \
			s5p4418_timer.o \
			scheduler.o \
			string.o \
			task.o \
			timer.o \
			synchronize.o \
			taskswitchinterrupt.o \
			timeslicetimerhandler.o \
			uspibind.o \
			uspienv.o \
			assert.o \
			smp.o \
			fifo.o \
			printk.o \
			global_timer.o

SOBJS  :=  delay.o \
		   taskswitch.o \
		   headsmp.o \
		   mutexes.o \
		   div64_asm.o

all : $(SOBJS) $(COBJS) $(SUBDIRS)

$(SOBJS) : %.o : %.S
	@echo [AS] $(CPATH)/$^
	@$(AS) $(X_SFLAGS) $(X_INCDIR) -c $< -o $@

$(COBJS) : %.o : %.c
	@echo [CC] $(CPATH)/$^
	@$(CC) $(X_CFLAGS) $(X_INCDIR) -c $< -o $@

subdirs : $(SUBDIRS)
$(SUBDIRS) :
	@make -s -C $@

.PHONY: subdirs $(SUBDIRS)
