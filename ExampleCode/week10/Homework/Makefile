obj-m += button_driver.o
obj-m += seg_driver.o
KDIR = ~/working/kernel
RESULT = segButton
SRC = $(RESULT).c
CCC = gcc

all:
	make -C $(KDIR) M=$(PWD) modules
	$(CCC) -o $(RESULT) $(SRC)

clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -f $(RESULT)
