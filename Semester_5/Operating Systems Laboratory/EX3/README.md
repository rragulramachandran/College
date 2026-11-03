# EX3: IPC via Message Queues and Shared Memory

Implementation of System V IPC mechanisms for inter-process message passing and shared memory access in C.

## Overview

### 1. Message Queues (`msgget`, `msgsnd`, `msgrcv`)
Message queues provide structured asynchronous message exchange between processes without requiring them to be simultaneously active. Messages include explicit message types (`mtype`) enabling priority or selective retrieval.

### 2. Shared Memory (`shmget`, `shmat`, `shmdt`)
Shared memory provides the fastest IPC mechanism by mapping a shared region of physical memory directly into the address spaces of cooperating processes, bypassing kernel copy overhead.

## Modules

```text
EX3/
├── Message Queue/
│   ├── Batch1/ & Batch2/
│   │   ├── producer.c / consumer.c — Producer pushes messages; consumer reads and processes
│   │   └── q1.c / q2.c             — Bidirectional dialogue across message queues
├── Shared Memory/
│   ├── Batch 1/
│   │   ├── matrix_sum_shared_memory.c — Parallel matrix addition computed via shared memory
│   │   └── producer_consumer_vowels.c — Producer writes text; consumer filters and counts vowels
│   └── Batch 2/
│       ├── process_chain_array_sum.c  — Array sum computation across chained processes
│       └── producer_consumer_reverse.c — String reversal via shared buffer
```

## Compilation

```bash
# Message Queue Producer-Consumer
gcc "Message Queue/Batch1/producer.c" -o producer
gcc "Message Queue/Batch1/consumer.c" -o consumer

# Shared Memory Matrix Addition
gcc "Shared Memory/Batch 1/matrix_sum_shared_memory.c" -o matrix_shm
./matrix_shm
```
