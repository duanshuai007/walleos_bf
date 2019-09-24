CPATH	:=${CPATH}/x_sample

#添加需要编译的工程文件夹
WORK_DIR := hello_world

subdirs: $(WORK_DIR)
$(WORK_DIR):
	@make -s -C $@

.PHONY: subdirs $(WORK_DIR)
