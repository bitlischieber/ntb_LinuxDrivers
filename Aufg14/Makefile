ifeq ($(KERNELRELEASE),)
	KERNELDIR ?= ~/zoom/buildroot-toolchain/linux-HS14  #/lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

modules:
	$(MAKE) ARCH=arm CROSS_COMPILE=~/zoom/buildroot-toolchain/buildroot-toolchain-HS14/bin/arm-buildroot-linux-gnueabi- -C $(KERNELDIR) M=$(PWD) modules

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.order *.symvers

copy2board:
	scp $(PWD)/zoomOmap_io.ko root@es057.ntb.ch:/media/ram

.PHONY: modules clean copy2board

else
endif
	obj-m := zoomOmap_io.o
