CPATH	:= $(CPATH)/start
SOBJS 	:= start.o vfp.o
DEPF	:= $(ROOT_DIR)/$(PRO_CONFIGH)

all : $(SOBJS)
$(SOBJS) : %.o : %.S $(DEPF)
	@echo [AS] $(CPATH)/$<
	@$(AS) $(X_SFLAGS) $(X_INCDIR) -c $< -o $@
