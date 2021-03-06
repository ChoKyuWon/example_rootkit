CONFIG_MODULE_SIG=n
obj-m += rootkit.o
DIR = $(shell pwd)
KERNEL_DIR = /lib/modules/$(shell uname -r)/build
CC=clang

all:
	make client
	make open_test
	make -C $(KERNEL_DIR) M=$(DIR) modules CC=$(CC)
client:
	$(CC) client.c -o client.o
open_test:
	$(CC) open_test -o open_test.o
clean:
	make -C $(KERNEL_DIR) M=$(DIR) clean
