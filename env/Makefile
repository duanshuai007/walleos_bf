SUBDIRS	:= lib
CPATH	:= $(CPATH)/env

subdirs : $(SUBDIRS)
$(SUBDIRS) :
	@make -s -C $@

.PHONY: subdirs $(SUBDIRS)
