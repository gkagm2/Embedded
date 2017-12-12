#Makefile for a basic kernel module

obj-m   := fpga_text_lcd_driver.o fpga_push_switch_driver.o fpga_step_motor_driver.o

KDIR    := /work/achro5250/kernel
PWD     := $(shell pwd)
APP	:= oneshot

all: driver app
#all: driver 

driver:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules ARCH=arm

app:
	arm-none-linux-gnueabi-gcc -static -o $(APP) $(APP).c 
	arm-none-linux-gnueabi-gcc -static -o $(APP) $(APP).c 
	arm-none-linux-gnueabi-gcc -static -o $(APP) $(APP).c 


install:
	cp -a fpga_step_motor_driver.ko /nfsroot
	cp -a fpga_text_lcd_driver.ko /nfsroot
	cp -a fpga_push_switch_driver.ko /nfsroot
	cp -a $(APP) /nfsroot
clean:
	rm -rf *.ko
	rm -rf *.mod.*
	rm -rf *.o
	rm -rf $(APP)
	rm -rf Module.symvers
	rm -rf modules.order
	rm -rf .tmp*
