CPATH	:= $(CPATH)/photograph

X_INCDIR  += -I ../include

OBJS	:= 	photograph.o \
			pic_opt/picture_opt.o \
			bmp/bmp.o \
			jpeg/jpeg.o \
			jpeg/libjpeg/jcapimin.o	\
			jpeg/libjpeg/jcparam.o	\
			jpeg/libjpeg/jdcoefct.o \
			jpeg/libjpeg/jdmainct.o	\
			jpeg/libjpeg/jdphuff.o \
			jpeg/libjpeg/jmemnobs.o \
			jpeg/libjpeg/jcapistd.o	\
			jpeg/libjpeg/jctrans.o \
			jpeg/libjpeg/jidctred.o	\
			jpeg/libjpeg/jdmarker.o	\
			jpeg/libjpeg/jdpostct.o	\
			jpeg/libjpeg/jquant1.o \
			jpeg/libjpeg/jchuff.o \
			jpeg/libjpeg/jdapimin.o	\
			jpeg/libjpeg/jidctint.o	\
			jpeg/libjpeg/jdmaster.o	\
			jpeg/libjpeg/jdsample.o	\
			jpeg/libjpeg/jquant2.o \
			jpeg/libjpeg/jcinit.o \
			jpeg/libjpeg/jdapistd.o	\
			jpeg/libjpeg/jdcolor.o \
			jpeg/libjpeg/jdmerge.o \
			jpeg/libjpeg/jdtrans.o \
			jpeg/libjpeg/jsimd_none.o \
			jpeg/libjpeg/jcmaster.o	\
			jpeg/libjpeg/jdatadst.o	\
			jpeg/libjpeg/jddctmgr.o \
			jpeg/libjpeg/jidctfst.o	\
			jpeg/libjpeg/jerror.o \
			jpeg/libjpeg/jidctflt.o \
			jpeg/libjpeg/jcomapi.o \
			jpeg/libjpeg/jdatasrc.o	\
			jpeg/libjpeg/jdhuff.o \
			jpeg/libjpeg/jmemmgr.o \
			jpeg/libjpeg/jutils.o \
			jpeg/libjpeg/jdinput.o

all : $(OBJS)
$(OBJS) : %.o : %.c
	@echo [CC] $(CPATH)/$^
	@$(CC) $(X_CFLAGS) $(X_INCDIR) -c $^ -o $@

