
ARCH := mips
CROSS_COMPILE := /home/matteius/output/cinnado_d1_t31l/host/bin/mipsel-linux-

obj-m := isp_driver.o

KDIR := /home/matteius/output/cinnado_d1_t31l/build/linux-22b9213568c866c0126a7ed3e0b0c30621339c64
PWD := $(shell pwd)

EXTRA_CFLAGS := -march=mips32

all:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) EXTRA_CFLAGS=$(EXTRA_CFLAGS) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(PWD) clean

