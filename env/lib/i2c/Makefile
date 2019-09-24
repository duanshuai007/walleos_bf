CPATH	:= $(CPATH)/i2c
OBJS	:= i2c.o

all : $(OBJS)
$(OBJS) : %.o : %.c
	@echo [CC] $(CPATH)/$^
	@$(CC) $(X_CFLAGS) $(X_INCDIR) -c $^ -o $@
