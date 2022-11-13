#imple Makefile to build a simple misc driveri
# Nick Glynn <Nick.Glynn@feabhas.com>

obj-m += kernel_module.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

CC := $(CROSS_COMPILE)gcc

all:
	$(MAKE) -C $(KDIR) M=${shell pwd} modules
        
clean:
	-$(MAKE) -C $(KDIR) M=${shell pwd} clean || true
	-rm *.o *.ko *.mod.{c,o} modules.order Module.symvers user_space kernel_module.o.ur-safe || true

