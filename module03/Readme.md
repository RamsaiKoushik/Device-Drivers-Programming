# ram_array3 - Linux Character Device Driver

A Linux kernel module that implements a character device `/dev/ram_array3` backed by an in-kernel RAM buffer. This module demonstrates interaction via standard file operations (`open`, `read`, `write`, `llseek`, `release`) and custom IOCTL commands. It also includes mutual exclusion using a binary semaphore to handle concurrent access.

---

## Features

- RAM-backed buffer of size 1024 bytes
- File operations:
  - `open()`, `release()`
  - `read()`, `write()`, `llseek()`
- IOCTL system calls for:
  - Fetching buffer size
  - Clearing buffer
  - Counting vowels in buffer
- Safe concurrent access using `struct semaphore`

---

## 🛠️ Build & Load Instructions

1. **Save the module source code** as `ram_array3.c`

2. **Create a Makefile**:

   ```makefile
   obj-m += ram_array3.o

   all:
       make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

   clean:
       make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
   ```

3. **Build the module**:

   ```bash
   make
   ```

4. **Insert the module**:

   ```bash
   sudo insmod ram_array3.ko
   ```

5. **Check kernel log**:

   ```bash
   dmesg | tail
   ```

6. **Create the device file**:

   ```bash
   sudo mknod /dev/ram_array3 c <major_number> 0
   sudo chmod 666 /dev/ram_array3
   ```

   Replace `<major_number>` with the one printed by `dmesg`.

7. **Remove the module**:

   ```bash
   sudo rmmod ram_array3
   ```

---

## 🔧 Supported IOCTL Commands

| Macro Name        | Command              | Description                                 |
|-------------------|----------------------|---------------------------------------------|
| `RAM_GET_SIZE`    | `_IOR(..., 1, int)`  | Returns size of the buffer (1024 bytes)     |
| `RAM_CLEAR`       | `_IO(..., 2)`        | Zeros out the entire RAM buffer             |
| `RAM_COUNT_VOWELS`| `_IOR(..., 3, int)`  | Returns the number of vowels in the buffer  |

**Magic Number**: `'R'`  
**Header Requirement**: Include the IOCTL macros and number definitions in your user-space code.

---

## Concurrency Control with Semaphore

To protect shared access to the RAM buffer, the module uses a **binary semaphore** (`struct semaphore ram_sem`). This ensures **only one process can access the buffer at a time**.

### Example Usage

```c
sema_init(&ram_sem, 1);  // Initialize binary semaphore

if (down_interruptible(&ram_sem)) {
    return -ERESTARTSYS;  // Lock not acquired
}

// critical section (e.g., open, write, etc.)

up(&ram_sem);  // Release lock
```

---

##  Semaphore Variants in the Linux Kernel

| Type                  | Description                                  | Syntax / API                                      | Use Case                                                                 |
|-----------------------|----------------------------------------------|---------------------------------------------------|--------------------------------------------------------------------------|
| **Binary Semaphore**  | Allows only one process inside critical section at a time | `struct semaphore` <br> `sema_init(&sem, 1)` <br> `down()` / `down_interruptible()` / `up()` | Mutual exclusion — e.g., protecting a shared buffer (like in this module) |
| **Counting Semaphore**| Allows `N` concurrent accesses               | `sema_init(&sem, N)`                              | Limiting concurrent access to a finite resource pool                     |
| **Mutex**             | Specialized binary semaphore with ownership checks | `DEFINE_MUTEX(name)` <br> `mutex_lock()` / `mutex_unlock()` | Mutual exclusion with ownership and reentrancy safety                   |
| **Spinlock**          | Busy-wait lock used in atomic contexts       | `spinlock_t lock;` <br> `spin_lock()` / `spin_unlock()` | Used in interrupt context or where sleeping is not allowed              |
| **RW Semaphore**      | Allows multiple readers or one writer        | `init_rwsem(&rwsem)` <br> `down_read()` / `down_write()` / `up_read()` / `up_write()` | Efficient shared-read, exclusive-write locking                          |
| **Completions**       | One-time events / signaling                  | `DECLARE_COMPLETION()` <br> `wait_for_completion()` / `complete()` | Used when one task must wait for another to complete an event           |

---
