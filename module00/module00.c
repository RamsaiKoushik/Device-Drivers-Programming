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
