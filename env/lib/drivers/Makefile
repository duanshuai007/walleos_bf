CPATH	:= $(CPATH)/drivers
OBJS	:= common/board_r.o \
		   devices/dw_mmc.o \
		   lib/div64.o \
		   lib/initcall.o \
		   mmc/dw_mmc.o \
		   mmc/mmc_write.o \
		   mmc/part.o \
		   mmc/mmc.o \
		   ethernet/dwmac1000_core.o \
		   ethernet/dwmac_lib.o \
		   ethernet/mmc_core.o \
		   ethernet/nx_gmac.o \
		   ethernet/dwmac1000_dma.o \
		   ethernet/enh_desc.o \
		   ethernet/nxpmac_main.o \

all : $(OBJS)
$(OBJS) : %.o : %.c
	@echo [CC] $(CPATH)/$^
	@$(CC) $(X_CFLAGS) $(X_INCDIR) -c $^ -o $@
