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

## Mutex APIs used and their Variants

| **API Call**           | **Syntax**                                | **Description**                                                                               | **When to Use**                                                                                |
| ---------------------- | ----------------------------------------- | --------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------- |
| `mutex_init`           | `mutex_init(&mutex);`                     | Dynamically initializes a mutex.                                                              | Use during initialization of a mutex if it's declared globally or on the heap.                 |
| `DEFINE_MUTEX`         | `DEFINE_MUTEX(my_mutex);`                 | Statically defines and initializes a mutex in one step.                                       | Use for global/static variables to simplify initialization.                                    |
| `mutex_lock`           | `mutex_lock(&mutex);`                     | Acquires the mutex, blocks if it's not available.                                             | Use in normal critical sections where blocking is acceptable.                                  |
| `mutex_unlock`         | `mutex_unlock(&mutex);`                   | Releases the mutex.                                                                           | Always call this after `mutex_lock()` to avoid deadlocks.                                      |
| `mutex_trylock`        | `mutex_trylock(&mutex);`                  | Tries to acquire the mutex without blocking; returns `true` if successful, `false` otherwise. | Use when you cannot afford to sleep or block, e.g., short paths like `open()`.                 |
| `mutex_is_locked`      | `mutex_is_locked(&mutex);`                | Returns non-zero if the mutex is currently locked.                                            | Use only for debugging or diagnostic purposes, not for logic control.                          |
| `mutex_destroy`        | `mutex_destroy(&mutex);`                  | Destroys a mutex (used mostly in userspace-like constructs).                                  | Rarely needed in kernel code, used with dynamically allocated mutexes.                         |
| `mutex_trylock_nested` | `mutex_trylock_nested(&mutex, subclass);` | Acquires a mutex without blocking, with lockdep subclass annotation.                          | Use for nested locking scenarios to avoid false positives in lock dependency checks (lockdep). |

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

## Screenshot of Application Code Execution

![image](https://github.com/user-attachments/assets/a26f750d-a98c-4f88-a8b4-a5e03c99579c)

## What is a Mutex?

A **mutex** (short for *mutual exclusion*) is a synchronization primitive used to ensure that only **one thread or process** accesses a **critical section** of code or data at any given time.

In kernel space:
- It's lighter and simpler than semaphores when used for short code sections.
- It ensures safety in concurrent environments like kernel modules or device drivers.

> Think of it as a **lock** that only one entity can hold. Others trying to acquire it must wait or fail until it's released.

---
