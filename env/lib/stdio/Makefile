CPATH	:= $(CPATH)/stdio
OBJS	:= sprintf.o \
		   vsprintf.o \
		   vsnprintf_linux.o

all : $(OBJS)
$(OBJS) : %.o : %.c
	@echo [CC] $(CPATH)/$^
	@$(CC) $(X_CFLAGS) $(X_INCDIR) -c $^ -o $@
