# Read-Write Lock (`rwlock_t`) in Character Driver

This README focuses on the use of **read-write lock (`rwlock_t`)** in a simple RAM-backed Linux character device driver. Other mechanisms like mutex, semaphore, and spinlock are already covered separately.

---

## Files in the Folder
- `module06.c` – Character driver source code (uses `rwlock_t`)
- `app.c` – User-space test application (reused from previous tests)
- `Makefile` – Standard Makefile to build the driver

## 🛠️ Syntax and Use-Cases

### 🔹 Basic Initialization
```c
rwlock_t my_lock;
rwlock_init(&my_lock);
```

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

> ⚠Use IRQ-safe versions only if you're working in interrupt context or need atomicity w.r.t. interrupts.

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

