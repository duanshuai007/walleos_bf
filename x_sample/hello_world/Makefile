CPATH	:=	${CPATH}/hello_world

OBJS	:= 	hello_world.o \
			abort.o \
		  	idletask.o \
		  	neverstoptask.o \
		  	tasksheetmanager.o \
		  	gesturetask.o \
		  	dtouchtimertask.o \
		  	readline.o \
		  	serial_shell.o \
		  	charset.o \
		  	cmd.o

all : $(OBJS)
$(OBJS) : %.o : %.c
	@echo [CC] ${CPATH}/$^
	@$(CC) $(X_CFLAGS) $(X_INCDIR) -c $^ -o $@

