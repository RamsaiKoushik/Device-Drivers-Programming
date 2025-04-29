# RAM Array Character Device Driver

## Overview

This project implements a simple Linux **character device driver** called `ram_array2` that simulates a RAM-backed array with basic read, write, seek, and `ioctl` control functionalities.  
An accompanying **test application** (`app-code`) is provided to interact with the driver from user space.

---

## File Structure

| File         | Description                                              |
|--------------|-----------------------------------------------------------|
| `ram_array2.c` | Kernel module source file (the device driver)             |
| `app2.c`      | User-space application to test and interact with the driver |
| `Makefile`   | Makefile to compile the kernel module and app               |

---

## Features

- Read and Write arbitrary data to an internal RAM array.
- Seek to arbitrary positions using `lseek()`.
- Perform custom operations via `ioctl()`:
  - Clear buffer
  - Get buffer size
  - Count number of vowels in the buffer

---

## How to Build and Run

1. **Build the Kernel Module:**
   ```bash
   make
   ```

2. **Insert the Module:**
   ```bash
   sudo insmod ram_array2.ko
   ```

3. **Check Device Node:**
   - Use `dmesg` to find the major number (printed during init).
   - Create the device node manually if needed:
     ```bash
     sudo mknod /dev/ram_array2 c <major_number> 0
     ```

4. **Build and Run the Application:**
   ```bash
   gcc app.c -o app
   ./app
   ```

5. **Remove the Module:**
   ```bash
   sudo rmmod ram_array2
   ```

6. **Clean Build Files:**
   ```bash
   make clean
   ```

---

## Kernel APIs Used

The following Linux kernel APIs are used in the driver:

| API | Syntax | Purpose | When to Use |
|:---|:---|:---|:---|
| `register_chrdev` | `int register_chrdev(unsigned int major, const char *name, const struct file_operations *fops)` | Registers a character device and assigns it a major number. | When initializing a simple character device. |
| `unregister_chrdev` | `void unregister_chrdev(unsigned int major, const char *name)` | Unregisters the character device. | During cleanup when removing the module. |
| `kmalloc` | `void *kmalloc(size_t size, gfp_t flags)` | Allocates kernel memory dynamically. | To allocate buffer for storing data. |
| `kfree` | `void kfree(const void *ptr)` | Frees dynamically allocated memory. | During cleanup to prevent memory leaks. |
| `copy_to_user` | `long copy_to_user(void __user *to, const void *from, unsigned long n)` | Copies data from kernel space to user space. | When returning data to user processes (e.g., in read or ioctl). |
| `copy_from_user` | `long copy_from_user(void *to, const void __user *from, unsigned long n)` | Copies data from user space to kernel space. | When receiving data from user processes (e.g., in write). |
| `memset` | `void *memset(void *s, int c, size_t n)` | Sets memory to a specific value. | To clear the buffer contents. |
| `printk` | `printk(KERN_INFO "message")` | Kernel-space printing for logs. | For debugging or logging kernel activities. |
| `MODULE_*` macros | `MODULE_LICENSE`, `MODULE_AUTHOR`, `MODULE_DESCRIPTION` | Provide metadata about the kernel module. | Required for proper module information display and licensing. |
| `file_operations` | Struct containing function pointers for device operations. | Registers functions like `.read`, `.write`, `.llseek`, `.unlocked_ioctl`. | To define device behavior for system calls. |

---

## IOCTL Commands Supported

| IOCTL Command | Definition | Purpose |
|:---|:---|:---|
| `RAM_GET_SIZE` | `_IOR('R', 1, int)` | Returns the size of the buffer. |
| `RAM_CLEAR` | `_IO('R', 2)` | Clears the buffer content by setting all bytes to 0. |
| `RAM_COUNT_VOWELS` | `_IOR('R', 3, int)` | Counts and returns the number of vowels (case-insensitive) in the buffer. |

**Note:** `ioctl()` operations are accessible in the app via the `ioctl(fd, cmd, arg)` system call.

---

## Important Notes

- **Device Name:** `/dev/ram_array2`
- **Buffer Size:** 1024 bytes
- The **cursor** for read/write operations is updated after each operation and can be modified using `lseek`.

---
