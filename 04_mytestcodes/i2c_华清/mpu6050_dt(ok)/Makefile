ifeq ($(KERNELRELEASE),)

ifeq ($(ARCH),arm)
#KERNELDIR ?= /home/linux/fs4412/linux-3.14
KERNELDIR ?= /home/dong/100ask_imx6ull-sdk/Linux-4.9.88

ROOTFS ?= /opt/4412/rootfs
else
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
endif
PWD := $(shell pwd)


modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules INSTALL_MOD_PATH=$(ROOTFS) modules_install

clean:
	rm -rf  *.o  *.ko  .*.cmd  *.mod.*  modules.order  Module.symvers   .tmp_versions

else
CONFIG_MODULE_SIG=n
obj-m += mpu6050_drv.o
obj-m += mpu6050_client.o
obj-m += mpu6050_client_probed.o


endif

