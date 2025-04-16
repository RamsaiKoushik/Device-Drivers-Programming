#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/slab.h>

#define DEVICE_NAME "ram_array"
#define ARRAY_SIZE 1024

static int major;
static char *ram_array;
static int cursor = 0;
static struct cdev ram_cdev;

static ssize_t ram_read(struct file *file, char __user *buf, size_t count, loff_t *pos) {
    if (*pos >= ARRAY_SIZE) {
        printk(KERN_INFO "ram_array: Read position out of bounds\n");
        return 0;
    }
    if (*pos + count > ARRAY_SIZE)
        count = ARRAY_SIZE - *pos;
    
    if (copy_to_user(buf, ram_array + *pos, count)) {
        printk(KERN_ERR "ram_array: Failed to copy data to user\n");
        return -EFAULT;
    }
    
    printk(KERN_INFO "ram_array: Read %zu bytes from position %lld\n", count, *pos);
    *pos += count;
    return count;
}

static ssize_t ram_write(struct file *file, const char __user *buf, size_t count, loff_t *pos) {
    if (*pos >= ARRAY_SIZE) {
        printk(KERN_INFO "ram_array: Write position out of bounds\n");
        return 0;
    }
    if (*pos + count > ARRAY_SIZE)
        count = ARRAY_SIZE - *pos;
    
    if (copy_from_user(ram_array + *pos, buf, count)) {
        printk(KERN_ERR "ram_array: Failed to copy data from user\n");
        return -EFAULT;
    }
    
    printk(KERN_INFO "ram_array: Wrote %zu bytes at position %lld\n", count, *pos);
    *pos += count;
    return count;
}

static loff_t ram_seek(struct file *file, loff_t offset, int whence) {
    loff_t new_pos;
    switch (whence) {
        case SEEK_SET: new_pos = offset; break;
        case SEEK_CUR: new_pos = file->f_pos + offset; break;
        case SEEK_END: new_pos = ARRAY_SIZE + offset; break;
        default: return -EINVAL;
    }
    if (new_pos < 0 || new_pos > ARRAY_SIZE)
        return -EINVAL;
    
    printk(KERN_INFO "ram_array: Seek to position %lld\n", new_pos);
    file->f_pos = new_pos;
    return new_pos;
}

static struct file_operations ram_fops = {
    .owner = THIS_MODULE,
    .read = ram_read,
    .write = ram_write,
    .llseek = ram_seek,
};

static int __init ram_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &ram_fops);
    if (major < 0) {
        printk(KERN_ALERT "Failed to register char device\n");
        return major;
    }
    
    ram_array = kmalloc(ARRAY_SIZE, GFP_KERNEL);
    if (!ram_array) {
        unregister_chrdev(major, DEVICE_NAME);
        return -ENOMEM;
    }
    memset(ram_array, 0, ARRAY_SIZE);
    
    printk(KERN_INFO "ram_array driver registered with major %d\n", major);
    return 0;
}

static void __exit ram_exit(void) {
    kfree(ram_array);
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "ram_array driver unregistered\n");
}

module_init(ram_init);
module_exit(ram_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Koushik");
MODULE_DESCRIPTION("RAM-backed array device driver");

