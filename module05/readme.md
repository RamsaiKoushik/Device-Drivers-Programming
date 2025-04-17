# Linux Character Device Driver with Mutex Synchronization

This module demonstrates a simple RAM-backed character device driver for Linux with **mutex-based mutual exclusion** implemented. It provides basic file operations (`read`, `write`, `llseek`, `ioctl`) and ensures only one user process can access the device at a time using a `mutex`.

---
## Folder Structure
```
.
├── module05.c       # Driver code (contains the mutex-guarded device logic)
├── app.c            # User-space application to test the device
├── Makefile         # Build instructions for kernel module
```
---

##  Focus: Mutex in Device Driver

### What changed?

In earlier versions, mutual exclusion was handled using a **semaphores/spinlocks**. This version replaces that with a **`mutex`**, which is simpler and more suitable for short, non-blocking critical sections in kernel code.

### Relevant Parts of the code

```c
static struct mutex ram_mutex;

static int ram_open(struct inode *inode, struct file *file) {
    if (!mutex_trylock(&ram_mutex)) {
        printk(KERN_INFO "ram_array: Could not acquire mutex in open\n");
        return -EBUSY;
    }
    printk(KERN_INFO "ram_array: Device opened\n");
    return 0;
}

static int ram_release(struct inode *inode, struct file *file) {
    mutex_unlock(&ram_mutex);
    printk(KERN_INFO "ram_array: Device released\n");
    return 0;
}

...

static int __init ram_init(void) {
    ...
    mutex_init(&ram_mutex);
    ...
}
```

The `mutex_trylock()` ensures that if the mutex is already held by another process, the current caller will return immediately with an error (`-EBUSY`) instead of blocking.

---

## Mutex Syntax Variants in Kernel

| Function                 | Description                                                                 |
|--------------------------|-----------------------------------------------------------------------------|
| `mutex_init(&mutex)`     | Initializes the mutex                                                       |
| `mutex_lock(&mutex)`     | Blocks the caller until the mutex is available                             |
| `mutex_unlock(&mutex)`   | Unlocks the mutex after use                                                 |
| `mutex_trylock(&mutex)`  | Tries to acquire the mutex without blocking; returns `0` if failed          |
| `mutex_is_locked(&mutex)`| Checks if the mutex is currently held (use with care)                      |

###  When to use what:

- `mutex_lock()` → Use when you're okay with blocking until the lock is available.
- `mutex_trylock()` → Use in `open()` or short-time critical code where waiting is not acceptable.
- `mutex_unlock()` → Always release after you're done.
- `mutex_is_locked()` → Diagnostic only, never rely on it for logic flow.

---

##  Testing

Use the `app.c` file to test the driver:

```bash
make
sudo insmod ram_array5.ko
sudo mknod /dev/ram_array5 c <major> 0
./app
```

> Replace `<major>` with the correct major number printed during `insmod`.

Make sure you test scenarios like opening the device from two processes to see how the mutex blocks access.

---

## What is a Mutex?

A **mutex** (short for *mutual exclusion*) is a synchronization primitive used to ensure that only **one thread or process** accesses a **critical section** of code or data at any given time.

In kernel space:
- It's lighter and simpler than semaphores when used for short code sections.
- It ensures safety in concurrent environments like kernel modules or device drivers.

> Think of it as a **lock** that only one entity can hold. Others trying to acquire it must wait or fail until it's released.

---

##  Cleanup

```bash
sudo rmmod ram_array3
make clean
```