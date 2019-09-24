CPATH	:= $(CPATH)/stdlib
OBJS	:= abs.o \
		   atoi.o \
		   rand.o \
		   strtol.o \
		   strtoul0.o \
		   strtoul.o

all : $(OBJS)
$(OBJS) : %.o : %.c
	@echo [CC] $(CPATH)/$^
	@$(CC) $(X_CFLAGS) $(X_INCDIR) -c $^ -o $@
