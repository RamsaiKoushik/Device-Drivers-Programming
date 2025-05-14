## 📦 Module 00 – Hello Kernel (Basic Module Skeleton)

### 📄 Description
This is a **minimal Linux kernel module** that demonstrates how to write, load, and unload a simple driver. It registers basic init and exit functions and uses `printk` to log messages into the kernel log buffer.

---

### 🧠 Source Code

```c
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ram");
MODULE_DESCRIPTION("A simple kernel module-00");

static int __init my_module_init(void) {
    printk(KERN_INFO "My module loaded\n");
    return 0;
}

static void __exit my_module_exit(void) {
    printk(KERN_INFO "My module unloaded\n");
}

module_init(my_module_init);
module_exit(my_module_exit);
```

---

### ⚙️ Build Instructions

1. Create a `Makefile`:
```Makefile
obj-m += module00.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
```

2. Compile the module:
```bash
make
```

---

### 🚀 Load & Test

```bash
# Insert the module
sudo insmod my_module.ko

# Check kernel logs
dmesg | tail

# Remove the module
sudo rmmod my_module

# Check logs again
dmesg | tail
```

Expected Output:
```bash
[ 1234.567890] My module loaded
[ 1235.678901] My module unloaded
```

---

### 📚 Notes

- `printk(KERN_INFO ...)` prints info-level logs, viewable using `dmesg`.
- Always unload the module before recompiling.
- `MODULE_LICENSE("GPL")` is required to avoid the “tainted kernel” warning.
