OBJS	:= platform.o

CPATH	:= ${CPATH}/base

all: $(OBJS)
$(OBJS) : %.o : %.c
	@echo [CC] ${CPATH}/$^
	@$(CC) $(X_CFLAGS) $(X_INCDIR) -c $^ -o $@
