CPATH	:= $(CPATH)/mmu
OBJS	:= s5p4418_mmu.o
SOBJS	:= mmu_asm.o

all : $(SOBJS) $(OBJS)
$(OBJS) : %.o : %.c
	@echo [CC] $(CPATH)/$^
	@$(CC) $(X_CFLAGS) $(X_INCDIR) -c $^ -o $@
$(SOBJS) : %.o : %.S
	@echo [AS] $(CPATH)/$^
	@$(AS) $(X_SFLAGS) $(X_INCDIR) -c $^ -o $@
