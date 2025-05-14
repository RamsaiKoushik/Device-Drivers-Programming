# Linux Kernel Programming Modules

This repository showcases a progressive series of Linux kernel modules demonstrating core kernel programming concepts and synchronization mechanisms. Each module is implemented in a separate directory (`module00` to `module07`) and includes a detailed README explaining its design, code, and usage.

##  Repository Structure

Below is a summary of the modules included in this repository:

### [`module00`](./module00/README.md)

> A minimal Linux kernel module demonstrating basic module loading and unloading using `printk()` for logging.

📖 [Read more](./module00/README.md)

---

### [`module01`](./module01/README.md)

> Implements a simple character device backed by a fixed-size in-kernel RAM buffer. Supports file-like operations from user space.

📖 [Read more](./module01/README.md)

---

### [`module02`](./module02/README.md)

> An enhanced version of a RAM-backed character device supporting basic `ioctl` operations for additional control.

📖 [Read more](./module02/README.md)

---

### [`module03`](./module03/README.md)

> Adds concurrency control using **binary semaphores** to the RAM-backed device, ensuring safe multi-process access.

📖 [Read more](./module03/README.md)

---

### [`module04`](./module04/README.md)

> Replaces semaphores with **spinlocks** to handle synchronization in a busy-wait fashion for kernel-space concurrency.

📖 [Read more](./module04/README.md)

---

### [`module05`](./module05/README.md)

> Implements synchronization using **mutexes**, providing blocking mechanisms for mutual exclusion.

📖 [Read more](./module05/README.md)

---

### [`module06`](./module06/README.md)

> Uses **read-write locks** to differentiate between read and write accesses, allowing concurrent readers.

📖 [Read more](./module06/README.md)

---

### [`module07`](./module07/README.md)

> Demonstrates **Read-Copy-Update (RCU)**, a sophisticated synchronization mechanism optimized for read-heavy workloads.

📖 [Read more](./module07/README.md)

---

## Notes

* Each module directory is self-contained with its own `Makefile`, source code, and documentation.
* All modules are tested on a recent Linux kernel version with dynamic module insertion/removal using `insmod` and `rmmod`.

Feel free to explore each module’s directory and corresponding README to dive deeper into their implementation details.
