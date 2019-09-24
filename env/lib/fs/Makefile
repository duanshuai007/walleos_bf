CPATH	:= $(CPATH)/fs
OBJS := fat.o fatdir.o fatfs.o file.o\
		partitionmanager.o \
		fatcache.o fatformal.o \
		fatinfo.o \
		partition.o \
		path.o

all : $(OBJS)
$(OBJS) : %.o : %.c
	@echo [CC] $(CPATH)/$^
	@$(CC) $(X_CFLAGS) $(X_INCDIR) -c $^ -o $@
