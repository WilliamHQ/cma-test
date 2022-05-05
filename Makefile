EXTRA_CFLAGS += -I$(PWD)/include 
ifneq ($(KERNELRELEASE),)
	obj-m := cmatest.o
	cmatest-objs := test_main.o cma_test.o
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
default:
	$(MAKE) -C $(KERNELDIR) $(EXTRA_CFLAGS) M=$(PWD) modules
endif

clean:
	rm -rf *.ko *.o Mo* *.mod.c .test.* .tmp*  *.order *.o.rc .*.ko.cmd .*.o.cmd

unix:
	dos2unix *.* 
