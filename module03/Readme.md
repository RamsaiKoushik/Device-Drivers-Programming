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
###  Semaphore Locking Functions Used in this Module

| Call       | Syntax                          | What it Does                                                                 | When is it Useful                                        |
|------------|----------------------------------|------------------------------------------------------------------------------|-----------------------------------------------------------|
| `sema_init`| `sema_init(&sem, val);`         | Initializes the semaphore `sem` with value `val`.                           | Use at module init to prepare a semaphore with desired count (typically 1 for binary lock). |
| `down_interruptible`| `down_interruptible(&sem);` | Attempts to acquire the semaphore. Sleeps if not available, but can be interrupted. | Use in user-triggered functions (`open()`, etc.) where you want to allow interruption via signals. |
| `up`       | `up(&sem);`                     | Releases the semaphore, waking up any sleeping waiters.                     | Always used after successful `down*()` to release lock.  |

---

###  Variants of the Semaphore Methods

####  Acquisition Variants (`down` family):

| Call               | Syntax                       | What it Does                                                                 | When is it Useful                                              |
|--------------------|------------------------------|------------------------------------------------------------------------------|-----------------------------------------------------------------|
| `down`             | `down(&sem);`                | Acquires the semaphore. Sleeps uninterruptibly if unavailable.              | Use in kernel threads where sleeping is allowed and should not be interrupted. |
| `down_interruptible`| `down_interruptible(&sem);` | Acquires the semaphore. Sleeps, but returns `-EINTR` if interrupted.        | Use when a signal should interrupt the sleep (e.g. user process). |
| `down_trylock`     | `down_trylock(&sem);`        | Tries to acquire the semaphore immediately. Non-blocking.                   | Use in atomic or interrupt context where sleeping is **not allowed**. |
| `down_killable`    | `down_killable(&sem);`       | Like `down_interruptible()`, but only fatal signals interrupt.              | Use when you only want fatal signals to cause exit from sleep.  |

---
