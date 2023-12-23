obj-m += buttonDriver.o
obj-m += segmentDriver.o
obj-m += ledDriver.o
KDIR = ~/working/kernel

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
