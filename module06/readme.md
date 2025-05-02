# Read-Write Lock (`rwlock_t`) in Character Driver

This README focuses on the use of **read-write lock (`rwlock_t`)** in a simple RAM-backed Linux character device driver. Other mechanisms like mutex, semaphore, and spinlock are already covered separately.

---

## Files in the Folder
- `module06.c` – Character driver source code (uses `rwlock_t`)
- `app.c` – User-space test application (reused from previous tests)
- `Makefile` – Standard Makefile to build the driver

## 🛠️ Syntax and Use-Cases

Here's a table summarizing the key Linux kernel APIs related to **read lock mechanisms** (`rwlock_t`) used in this kernel module:

| **API Call**     | **Syntax**                  | **What It Does**                             | **When to Use**                                                               |
| ---------------- | --------------------------- | -------------------------------------------- | ----------------------------------------------------------------------------- |
| `rwlock_t`       | `rwlock_t lock_name;`       | Declares a read-write lock variable.         | Use during global/static variable declarations to enable rwlock usage.        |
| `rwlock_init()`  | `rwlock_init(&lock_name);`  | Initializes the read-write lock.             | Call once (e.g., during driver/module init) before using the lock.            |
| `read_lock()`    | `read_lock(&lock_name);`    | Acquires the lock in read (shared) mode.     | Use before reading shared data to allow concurrent readers but block writers. |
| `read_unlock()`  | `read_unlock(&lock_name);`  | Releases the previously acquired read lock.  | Use immediately after a `read_lock()` once reading is done.                   |
| `write_lock()`   | `write_lock(&lock_name);`   | Acquires the lock in write (exclusive) mode. | Use before writing/updating shared data to ensure exclusive access.           |
| `write_unlock()` | `write_unlock(&lock_name);` | Releases the previously acquired write lock. | Use immediately after a `write_lock()` once writing is done.                  |

### Notes:

* **Readers** can acquire the lock simultaneously as long as no **writer** holds the lock.
* **Writers** must wait until **all readers** and **other writers** have released the lock.
* `rwlock_t` is **spinlock-based**, so it's appropriate for short critical sections and **not sleepable**. Avoid blocking/sleeping inside read/write lock critical sections.


### 🔹 Locking Variants

### Locking Variants

| Function                          | Type           | Description                                           | When to Use                                         |
|-----------------------------------|----------------|-------------------------------------------------------|-----------------------------------------------------|
| `read_lock()`                     | Blocking       | Acquires read lock (allows multiple readers)          | Normal read operations outside interrupt context    |
| `read_unlock()`                   | -              | Releases read lock                                    | After completing read operation                    |
| `write_lock()`                    | Blocking       | Acquires write lock (exclusive access)                | Normal write operations outside interrupt context   |
| `write_unlock()`                  | -              | Releases write lock                                   | After completing write operation                   |
| `read_lock_irqsave(flags)`         | Interrupt-safe | Acquires read lock and disables interrupts            | Read operations inside interrupt handler or atomic sections |
| `read_unlock_irqrestore(flags)`    | -              | Releases lock and restores interrupt state            | After completing critical read section inside interrupt |
| `write_lock_irqsave(flags)`        | Interrupt-safe | Acquires write lock and disables interrupts           | Write operations inside interrupt handler or atomic sections |
| `write_unlock_irqrestore(flags)`   | -              | Releases lock and restores interrupt state            | After completing critical write section inside interrupt |

> Use IRQ-safe versions only if you're working in interrupt context or need atomicity w.r.t. interrupts.

---

## What is `rwlock_t`?

A `rwlock_t` is a kernel synchronization primitive that allows:

- **Multiple concurrent readers** (read_lock)
- **Only one writer at a time** (write_lock), and no readers during that write

This makes it more efficient in **read-heavy workloads** where contention on write is minimal.

---

## When to Use `rwlock_t`?

- Use when **frequent reads and rare writes** occur.
- Avoid in **write-heavy scenarios** as writers get blocked if readers keep coming in.

---

