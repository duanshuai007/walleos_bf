CPATH	:= $(CPATH)/input
OBJS := gesturedetector.o \
		messagequeue.o \
		scalegesturedetector.o \
		touchscreen.o \
		motioneventfifo.o \

all : $(OBJS)
$(OBJS) : %.o : %.c
	@echo [CC] $(CPATH)/$^
	@$(CC) $(X_CFLAGS) $(X_INCDIR) -c $^ -o $@
