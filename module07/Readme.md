# ram_array7 - Linux Character Device Driver

A Linux kernel module that implements a character device `/dev/ram_array7` backed by an in-kernel RAM buffer. This module demonstrates interaction via standard file operations (`open`, `read`, `write`, `llseek`, `release`) and custom IOCTL commands. It also includes mutual exclusion using the locking mechanism RCU(Read Copy Update) to handle concurrent access.

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

To protect shared access to the RAM buffer, the module uses RCU. This allows multiple readers to read at the same time, along with one writer having access to write simultaneously.

###  **RCU API Calls Used (Basic Table)**

| **API Call**           | **Syntax**                         | **Description**                                                                    | **When to Use**                                                                     |
| ---------------------- | ---------------------------------- | ---------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------- |
| `rcu_read_lock()`      | `rcu_read_lock();`                 | Marks the beginning of an RCU read-side critical section.                          | Use before accessing shared RCU-protected data to ensure safe concurrent access.    |
| `rcu_read_unlock()`    | `rcu_read_unlock();`               | Marks the end of an RCU read-side critical section.                                | Use after you're done reading the RCU-protected data.                               |
| `synchronize_rcu()`    | `synchronize_rcu();`               | Blocks the caller until all pre-existing RCU read-side critical sections finish.   | Use before freeing or modifying RCU-protected data to ensure no readers are active. |
| `kfree_rcu()`          | `kfree_rcu(ptr, rcu_head_member);` | Schedules a structure for freeing after an RCU grace period.                       | Use instead of `kfree()` when deleting RCU-protected data structures.               |
| `rcu_assign_pointer()` | `rcu_assign_pointer(p, v);`        | Safely assigns a pointer to an RCU-protected variable with proper memory barriers. | Use when updating RCU-protected pointers.                                           |
| `rcu_dereference()`    | `rcu_dereference(p);`              | Safely dereferences an RCU-protected pointer with memory barriers.                 | Use when reading RCU-protected pointers inside an RCU read-side section.            |

---

### **RCU API Variants Table**

| **API Call**                  | **Syntax**                          | **Description**                                                                  | **When to Use**                                           |
| ----------------------------- | ----------------------------------- | -------------------------------------------------------------------------------- | --------------------------------------------------------- |
| `rcu_read_lock_bh()`          | `rcu_read_lock_bh();`               | Disables bottom halves during RCU read section.                                  | Use in softirq or bottom half contexts.                   |
| `rcu_read_unlock_bh()`        | `rcu_read_unlock_bh();`             | Re-enables bottom halves after RCU read section.                                 | Use with `rcu_read_lock_bh()`.                            |
| `rcu_read_lock_sched()`       | `rcu_read_lock_sched();`            | Marks beginning of an RCU-sched read-side section (scheduling-disabled version). | Use when you’re in atomic or scheduler-disabled contexts. |
| `rcu_read_unlock_sched()`     | `rcu_read_unlock_sched();`          | Ends RCU-sched read-side section.                                                | Use with `rcu_read_lock_sched()`.                         |
| `synchronize_rcu_expedited()` | `synchronize_rcu_expedited();`      | Faster (but more expensive) grace period sync.                                   | Use in rare cases where faster updates are critical.      |
| `call_rcu()`                  | `call_rcu(&rcu_head, callback);`    | Asynchronously invokes a callback after RCU grace period.                        | Use for cleanup/deferred operations on RCU data.          |
| `rcu_replace_pointer()`       | `rcu_replace_pointer(p, new, old);` | Atomically replaces an RCU-protected pointer.                                    | Use when you need to swap pointers conditionally.         |

---
