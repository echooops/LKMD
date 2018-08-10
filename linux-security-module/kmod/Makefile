LINUX_KERNEL := $(shell uname -r)
LINUX_KERNEL_PATH := /usr/src/linux-headers-$(LINUX_KERNEL)


# 内核目录 documention/kbuild/makefiles.txt 有详细介绍

# ccflags-y, asflags-y, ldflags-y
# 这三个标记只是用于定义它们的makefile中.
# 在递归构建时它们被用在所有的正规cc/as/ld指令调用中.
ccflags-y := -O3 -Werror -DDEBUG

# $(obj-m)指定了那些能被编译为可加载的内核模块的目标文件.
obj-m := vlsm_kmod.o

# 如果内核模块是通过几个源文件编译而成的,您可以使用和上面同样的方法指定您想要编译的模块.
# 然而此时Kbuild需要知道编译模块时是基于那些目标文件的,
# 因此您需要设置一个$(<module_name>-objs)变量来告诉编译器. 
vlsm_kmod-objs := vlsm_entry.o vlsm_hooks.o vlsm_kfifo.o vlsm_nlmsg.o vlsm_utils.o

all:
	$(MAKE) -C $(LINUX_KERNEL_PATH) M=$(PWD) modules
clean:
	$(MAKE)	-C $(LINUX_KERNEL_PATH) M=$(PWD) clean
