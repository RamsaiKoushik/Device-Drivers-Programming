# `ram_array4` – RAM-Backed Char Device Driver with Spinlock Protection

## Overview

`ram_array4` is a Linux kernel module that implements a character device backed by an in-memory buffer. It supports:

- Read and write access to the buffer.
- Custom `ioctl` operations for buffer management.
- Position control via `llseek`.
- Spinlock-based mutual exclusion for concurrency-safe `open()` and `release()`.

It creates a simple RAM-like block with exclusive open semantics to prevent multiple concurrent access, enforced using a **spinlock**.

---

## 🛠️ Features

- **Fixed-size buffer** (1024 bytes)
- **Exclusive open semantics** (single access at a time)
- **Spinlock for mutual exclusion** between concurrent kernel threads
- **IOCTLs for:**
  - Getting buffer size
  - Clearing buffer
  - Counting vowels in the buffer

---

## Files and Interfaces

| Component     | Description                                    |
|---------------|------------------------------------------------|
| `ram_open()`  | Checks and allows exclusive device open. Uses `spin_lock_irqsave`. |
| `ram_release()` | Marks device as available again. Also protected by spinlock. |
| `ram_read()`  | Copies data from kernel buffer to user space.  |
| `ram_write()` | Writes user data into kernel buffer.           |
| `ram_ioctl()` | Custom commands: size, clear, count vowels     |
| `ram_seek()`  | Sets the read/write position in the buffer     |
| `init/exit`   | Sets up device, allocates memory, initializes spinlock |

---

##  Supported IOCTL Commands

| Macro             | Operation                | Description                        |
|------------------|--------------------------|------------------------------------|
| `RAM_GET_SIZE`   | `_IOR(..., int)`         | Returns buffer size (1024)         |
| `RAM_CLEAR`      | `_IO(...)`               | Clears the RAM buffer              |
| `RAM_COUNT_VOWELS` | `_IOR(..., int)`       | Counts vowels (`aeiouAEIOU`) in buffer |

---

## Concurrency Control (Spinlock)

This driver uses a **spinlock** (`spin_lock_irqsave`) to enforce exclusive access to the device and protect the `device_open` flag. This ensures mutual exclusion across concurrent access attempts (e.g., multiple `open()` calls).

---

## Spinlock Functions Used

| Call                | Syntax                                    | What it Does                                                                | When is it Useful                                     |
|---------------------|--------------------------------------------|------------------------------------------------------------------------------|--------------------------------------------------------|
| `spin_lock_irqsave` | `spin_lock_irqsave(&lock, flags);`         | Acquires the spinlock and disables local interrupts, saving old state.      | Use when accessing shared data in code that might run with interrupts. |
| `spin_unlock_irqrestore` | `spin_unlock_irqrestore(&lock, flags);` | Releases spinlock and restores interrupt state.                            | Always used after `spin_lock_irqsave()` to restore state safely. |
| `spin_lock_init`    | `spin_lock_init(&lock);`                   | Initializes the spinlock before use.                                         | Call once during module setup (`init`).                |

---

## Variants of Spinlock Functions

| Call                | Syntax                              | What it Does                                                              | When is it Useful                                                  |
|---------------------|--------------------------------------|---------------------------------------------------------------------------|---------------------------------------------------------------------|
| `spin_lock`         | `spin_lock(&lock);`                 | Acquires spinlock (without disabling interrupts).                         | Use in non-interrupt context where preemption is allowed.           |
| `spin_unlock`       | `spin_unlock(&lock);`               | Releases spinlock (without restoring interrupts).                         | Use with `spin_lock()` in the same context.                         |
| `spin_lock_bh`      | `spin_lock_bh(&lock);`              | Disables bottom halves and acquires spinlock.                             | Use in softirq context (e.g., network drivers).                     |
| `spin_unlock_bh`    | `spin_unlock_bh(&lock);`            | Re-enables bottom halves and releases spinlock.                           | Use with `spin_lock_bh()`.                                          |
| `spin_trylock`      | `spin_trylock(&lock);`              | Tries to acquire lock without blocking; returns true on success.          | Use when you can't sleep or block, and want to skip if locked.      |
| `spin_is_locked`    | `spin_is_locked(&lock);`            | Checks if lock is currently held.                                         | Useful in debugging or assertions.                                  |

---

## ⚠️ Notes

- Never sleep while holding a spinlock.
- Always match `irqsave` with `irqrestore`, etc.
- Spinlocks are lightweight and fast but must be used **very carefully** in interrupt and atomic contexts.
