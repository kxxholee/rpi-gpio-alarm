obj-m += seg_driver.o
KDIR = ~/Workbench/RaspberryPi/kernel
RESULT = seg_example
SRC = $(RESULT).c
RESULT2 = seg_example2
SRC2 = $(RESULT2).c
CCC = arm-linux-gnueabihf-gcc

all:
	make -C $(KDIR) M=$(PWD) modules
	$(CCC) -o $(RESULT) $(SRC)
	$(CCC) -o $(RESULT2) $(SRC2)

clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -f $(RESULT)
	rm -f $(RESULT2)
