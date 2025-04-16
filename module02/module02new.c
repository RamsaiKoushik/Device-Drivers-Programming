#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/slab.h>
// IOCTL Command Definitions
#include <linux/ioctl.h>
#define RAM_IOC_MAGIC 'R'
#define RAM_GET_SIZE _IOR(RAM_IOC_MAGIC, 1, int)  // Read buffer size
#define RAM_CLEAR _IO(RAM_IOC_MAGIC, 2)          // Clear buffer
#define RAM_COUNT_VOWELS _IOR(RAM_IOC_MAGIC, 3, int) // Count vowels

#define DEVICE_NAME "ram_array2"
#define BUFFER_SIZE 1024  // Renamed to avoid conflicts

static int major;
static char *ram_array;
static struct cdev ram_cdev __attribute__((unused)); // Marked unused to suppress warnings
static int cursor __attribute__((unused)) = 0;       // Marked unused

static ssize_t ram_read(struct file *file, char __user *buf, size_t count, loff_t *pos) {
    if (*pos >= BUFFER_SIZE) {
        printk(KERN_INFO "ram_array: Read position out of bounds\n");
        return 0;
    }
    if (*pos + count > BUFFER_SIZE)
        count = BUFFER_SIZE - *pos;
    
    if (copy_to_user(buf, ram_array + *pos, count)) {
        printk(KERN_ERR "ram_array: Failed to copy data to user\n");
        return -EFAULT;
    }
    
    printk(KERN_INFO "ram_array: Read %zu bytes from position %lld\n", count, *pos);
    *pos += count;
    return count;
}

static ssize_t ram_write(struct file *file, const char __user *buf, size_t count, loff_t *pos) {
    if (*pos >= BUFFER_SIZE) {
        printk(KERN_INFO "ram_array: Write position out of bounds\n");
        return 0;
    }
    if (*pos + count > BUFFER_SIZE)
        count = BUFFER_SIZE - *pos;
    
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
        case SEEK_END: new_pos = BUFFER_SIZE + offset; break;
        default: return -EINVAL;
    }
    if (new_pos < 0 || new_pos > BUFFER_SIZE)
        return -EINVAL;
    
    printk(KERN_INFO "ram_array: Seek to position %lld\n", new_pos);
    file->f_pos = new_pos;
    return new_pos;
}

static long ram_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    int count = 0, i;
    int buffer_size = BUFFER_SIZE; // Temporary variable for copy_to_user
    
    switch (cmd) {
        case RAM_GET_SIZE:
            //if (copy_to_user((int __user *)arg, &buffer_size, sizeof(int)))
               // return -EFAULT;
            printk(KERN_INFO "ram_array Size: %d\n", buffer_size);
            break;

        case RAM_CLEAR:
            memset(ram_array, 0, BUFFER_SIZE);
            printk(KERN_INFO "ram_array: Buffer cleared\n");
            break;

        case RAM_COUNT_VOWELS:
            for (i = 0; i < BUFFER_SIZE; i++) {
                char c = ram_array[i];
                if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' ||
                    c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U') {
                    count++;
                }
            }
            if (copy_to_user((int __user *)arg, &count, sizeof(int)))
                return -EFAULT;
            printk(KERN_INFO "ram_array: Counted %d vowels\n", count);
            break;

        default:
            return -EINVAL;
    }
    return 0;
}

static struct file_operations ram_fops = {
    .owner = THIS_MODULE,
    .read = ram_read,
    .write = ram_write,
    .llseek = ram_seek,
    .unlocked_ioctl = ram_ioctl, // Register IOCTL handler
};

static int __init ram_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &ram_fops);
    if (major < 0) {
        printk(KERN_ALERT "Failed to register char device\n");
        return major;
    }
    
    ram_array = kmalloc(BUFFER_SIZE, GFP_KERNEL);
    if (!ram_array) {
        unregister_chrdev(major, DEVICE_NAME);
        return -ENOMEM;
    }
    memset(ram_array, 0, BUFFER_SIZE);
    
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
MODULE_DESCRIPTION("RAM-backed array device driver with IOCTL commands");

