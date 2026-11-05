# Operating Systems Laboratory

Laboratory programs for the Operating Systems course, implementing fundamental OS kernel mechanisms including process hierarchies, inter-process communication (IPC), CPU scheduling, process synchronization, deadlock avoidance, memory management, page replacement, file allocation, and disk scheduling.

All programs are implemented in **C** using POSIX system calls (ork, pipe, msgget, shmget, semget, pthread).

## Exercise List

| Exercise | Topic | Concepts | Description |
|----------|-------|----------|-------------|
| [EX1](EX1/) | Process Creation & Hierarchies | ork(), getpid(), getppid(), wait() | Linear process chains, binary tree hierarchies, custom process trees, and concurrent task division |
| [EX2](EX2/) | Inter-Process Communication: Pipes | pipe(), 
ead(), write(), file descriptors | Full-duplex two-way communication, bi-directional pipes, and cascaded pipelines |
| [EX3](EX3/) | IPC: Message Queues & Shared Memory | System V IPC (msgget, msgsnd, msgrcv, shmget, shmat) | Message queues (number passing, word/string transfer, chunked file transfer) and shared memory (matrix addition, string reversal) |
| [EX4](EX4/) | CPU Scheduling Algorithms | Preemptive & Non-preemptive scheduling | FCFS, SJF, Priority Scheduling, Round Robin (RR), and SRTF with turnaround, waiting times, and modular implementations |
| [EX5](EX5/) | Process Synchronization (Semaphores) | Counting & Binary Semaphores | Producer-Consumer problem solving mutual exclusion, buffer coordination, and bounded buffer file transfer |
| [EX6](EX6/) | Deadlock Avoidance | Banker's Algorithm | Safety state verification and Resource Request algorithm on allocation matrices with step-by-step tracing |
| [EX7](EX7/) | Contiguous Memory Allocation | Memory partitioning | First Fit, Best Fit, and Worst Fit dynamic memory placement strategies with remaining hole tracking |
| [EX8](EX8/) | Page Replacement Algorithms | Virtual memory management | FIFO, Least Recently Used (LRU), and Optimal page replacement with step-by-step frame hit/fault ratios |
| [EX10](EX10/) | File Allocation Simulation | Secondary storage allocation | Contiguous file allocation using bit vectors, directory tables, search, content inspection, and deletion |
| [EX11](EX11/) | Disk Scheduling Algorithms | Secondary storage optimization | FCFS, SSTF, SCAN, C-SCAN, LOOK, and C-LOOK head movement minimization |
| [Leetcode](Leetcode/) | LeetCode OS & Concurrency | Threads, Semaphores, CPU, Cache | Implementations of classic concurrency and OS design problems (LRU, LFU, Dining Philosophers, Task Scheduler) |
| [ModelPractice](ModelPractice/) | Model Exam Practice | Comprehensive OS Lab Review | Complete practice suite covering Banker's algorithm, Memory Allocation, Disk Scheduling, Page Replacement, and Semaphores |

## Build and Run

Compile any exercise with gcc:

`ash
# Process Creation (EX1)
gcc EX1/LinearHierarchy.c -o linear && ./linear
gcc EX1/TreeHierarchy1.c -o tree1 && ./tree1
gcc EX1/Concurrent_Task_Division.c -o task_div && ./task_div

# Two-Way Pipe (EX2)
gcc EX2/two_way_pipe.c -o pipe && ./pipe
gcc EX2/BiDirectional_Pipes.c -o bipipe && ./bipipe

# CPU Scheduling (EX4)
cd EX4 && gcc main.c scheduling_algorithms.c -o sched && ./sched
# Or modular version:
cd EX4/Modular && gcc app.c imp.c -o sched_mod && ./sched_mod

# Banker's Algorithm (EX6)
gcc EX6/bankersAlgorithm.c -o bankers && ./bankers
gcc EX6/Bankers_Algorithm_Resource_Request.c -o bankers_req && ./bankers_req

# Memory Allocation (EX7)
gcc EX7/continuos_memory_allocation.c -o mem_alloc && ./mem_alloc
gcc EX7/Memory_Allocation_Detailed.c -o mem_detailed && ./mem_detailed

# Page Replacement (EX8)
gcc EX8/pageReplacementAlgorithms.c -o page_replace && ./page_replace
gcc EX8/Page_Replacement_StepByStep.c -o page_step && ./page_step

# File Allocation (EX10)
cd EX10/BitVector_Directory && gcc app.c imp.c -o file_alloc && ./file_alloc

# Disk Scheduling (EX11)
gcc EX11/diskSchedulingAlgorithms.c -o disk_sched && ./disk_sched
gcc EX11/disk.c -o disk && ./disk
`
