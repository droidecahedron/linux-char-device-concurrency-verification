
obj-m := asp.o

KERNEL_DIR = /usr/src/linux-headers-$(shell uname -r)

all:
	$(MAKE) -C $(KERNEL_DIR) M=$(shell pwd)
	
app: 
	gcc -pthread -o userapp userapp.c

clean:
	rm -rf *.o *.ko *.mod.* *.symvers *.order *~
