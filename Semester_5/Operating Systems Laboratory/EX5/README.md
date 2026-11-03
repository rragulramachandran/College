# EX5: Process Synchronization using Semaphores

Classical Producer-Consumer (Bounded Buffer) problem solution using POSIX/System V semaphores.

## Overview

Coordinates concurrent execution of producer and consumer processes sharing a finite buffer:
- **Mutual Exclusion (`mutex`):** Ensures only one process modifies buffer indices at any given moment.
- **Empty Count (`empty`):** Counts available empty slots in the buffer (initialized to $N$).
- **Full Count (`full`):** Counts filled slots containing items ready for consumption (initialized to $0$).

## Synchronization Protocol

```text
Producer:
  wait(empty);
  wait(mutex);
  /* add item to buffer */
  signal(mutex);
  signal(full);

Consumer:
  wait(full);
  wait(mutex);
  /* remove item from buffer */
  signal(mutex);
  signal(empty);
```

## Files

| File | Description |
|------|-------------|
| `header.h` | Shared buffer structure, semaphore keys, and union semun |
| `producer.c` | Producer process creating items and acquiring semaphore locks |
| `consumer.c` | Consumer process waiting for available items and consuming safely |

## Compilation

```bash
gcc producer.c -o producer
gcc consumer.c -o consumer
```
