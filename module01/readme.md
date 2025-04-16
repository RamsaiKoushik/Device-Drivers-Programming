# `ram_array` Kernel Module

This Linux kernel module implements a simple character device backed by a fixed-size in-kernel RAM buffer. It enables user-space applications to read, write, seek, and scan memory just like file operations, simulating a small RAM device.

---

## Features

- Character device with a virtual 1KB memory array
- Supports `read()`, `write()`, `lseek()` operations
- Simple, reusable interface
- Logs every action for debugging via `dmesg`
- Ready for man-page style documentation and learning purposes

---

##  Build & Usage Instructions

### 1. **Compile the Kernel Module**

```bash
make
```

### 2. **Insert the Module**

```bash
sudo insmod ram_array.ko
```

### 3. **Create the Device Node**

Find the major number from `dmesg`, then:

```bash
sudo mknod /dev/ram_array c <major_number> 0
sudo chmod 666 /dev/ram_array
```

### 4. **Compile and Run User Application**

```bash
gcc user_app.c -o user_app
./user_app
```

### 5. **Remove the Module**

```bash
sudo rmmod ram_array
sudo rm /dev/ram_array
```

---

## 🔌 Kernel Module Code (`module01.c`)

```c
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
    if (*pos >= ARRAY_SIZE) return 0;
    if (*pos + count > ARRAY_SIZE) count = ARRAY_SIZE - *pos;
    if (copy_to_user(buf, ram_array + *pos, count)) return -EFAULT;
    *pos += count;
    return count;
}

static ssize_t ram_write(struct file *file, const char __user *buf, size_t count, loff_t *pos) {
    if (*pos >= ARRAY_SIZE) return 0;
    if (*pos + count > ARRAY_SIZE) count = ARRAY_SIZE - *pos;
    if (copy_from_user(ram_array + *pos, buf, count)) return -EFAULT;
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
    if (new_pos < 0 || new_pos > ARRAY_SIZE) return -EINVAL;
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
    if (major < 0) return major;
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
```

---

## User Application Code (`app2.c`)

```c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define DEVICE_PATH "/dev/ram_array"
#define ARRAY_SIZE 1024

void scan_device(int fd) {
    lseek(fd, 0, SEEK_SET);
    char buffer[ARRAY_SIZE + 1];
    int bytesRead = read(fd, buffer, ARRAY_SIZE);
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        printf("Scanned Data: %s\n", buffer);
    } else {
        printf("Scan failed or no data to read.\n");
    }
}

int main() {
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return EXIT_FAILURE;
    }

    char command;
    char buffer[128];
    int offset, whence;

    while (1) {
        printf("\nEnter command (r=read, w=write, s=seek, x=scan, q=quit): ");
        scanf(" %c", &command);

        switch (command) {
            case 'r':
                printf("Enter number of bytes to read: ");
                scanf("%d", &offset);
                if (read(fd, buffer, offset) > 0) {
                    buffer[offset] = '\0';
                    printf("Read Data: %s\n", buffer);
                }
                break;

            case 'w':
                printf("Enter data to write: ");
                scanf("%s", buffer);
                write(fd, buffer, strlen(buffer));
                break;

            case 's':
                printf("Enter seek offset: ");
                scanf("%d", &offset);
                printf("Select seek mode (0=SEEK_SET, 1=SEEK_CUR, 2=SEEK_END): ");
                scanf("%d", &whence);
                if (lseek(fd, offset, whence) == -1) {
                    perror("Seek error");
                }
                break;

            case 'x':
                scan_device(fd);
                break;

            case 'q':
                close(fd);
                return 0;

            default:
                printf("Invalid command!\n");
        }
    }
}
```

---

## 📚 Kernel API Reference

| System Call | Syntax                               | Description |
|-------------|----------------------------------------|-------------|
| `read()`    | `read(fd, buf, count)`                 | Reads `count` bytes from the device into `buf`. |
| `write()`   | `write(fd, buf, count)`                | Writes `count` bytes from `buf` to device. |
| `lseek()`   | `lseek(fd, offset, SEEK_SET \| CUR \| END)`  | Moves file pointer within buffer range. |
| `copy_to_user()`  | `long copy_to_user(void __user *to, const void *from, unsigned long n)` | Copies `n` bytes of data from kernel space (`from`) to user space (`to`). Returns 0 on success, or the number of bytes not copied on error. |
| `copy_from_user()`| `long copy_from_user(void *to, const void __user *from, unsigned long n)` | Copies `n` bytes of data from user space (`from`) to kernel space (`to`). Returns 0 on success, or the number of bytes not copied. |
| `kmalloc()`       | `void *kmalloc(size_t size, gfp_t flags)`                               | Allocates `size` bytes of physically contiguous memory in kernel space. Use `GFP_KERNEL` for normal kernel allocations. Returns pointer on success, or `NULL` on failure. |
| `kfree()`         | `void kfree(const void *ptr)`                                           | Frees memory previously allocated with `kmalloc()`. |
| `memset()`        | `void *memset(void *s, int c, size_t n)`                               | Fills the first `n` bytes of the memory area pointed to by `s` with the constant byte `c`. Commonly used to zero out a buffer. |

## 🧠 Notes

- The buffer is 1024 bytes (`ARRAY_SIZE`)
- If you seek beyond bounds, `lseek` returns `-EINVAL`
- Proper user/kernel copy and error handling are done using `copy_to_user()` and `copy_from_user()`
---