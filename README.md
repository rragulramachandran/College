# College Coursework

An organised archive of laboratory work, programming exercises, and academic assignments completed during undergraduate coursework in Computer Science and Engineering at Mepco Schlenk Engineering College.

## Repository Structure

```text
College/
├── Semester_4/
│   └── Design_and_Analysis_Of_Algorithm/   — 9 DAA exercises in C++ (Sorting, Divide & Conquer, DP, Greedy, Backtracking, Branch & Bound)
└── Semester_5/
    ├── Compiler_Design/                     — Lexical analysis, 28 YACC parsers, intermediate code generation, optimization, and DFA
    │   ├── LEX/                             — 19 basic Lex scanner programs
    │   ├── RightContext/                    — 14 right-context & start-condition Lex programs
    │   ├── calculator/                      — Lex + YACC arithmetic expression evaluator
    │   ├── yacc/                            — YACC grammar validator (a^n b* c^n)
    │   ├── Simple-YACC/                     — 28 YACC parsers (14 groups × 2 questions)
    │   ├── quad/                            — Intermediate code generator (quadruples)
    │   ├── optimization/                    — Code optimization on quadruples (CSE, folding, propagation, DCE)
    │   ├── pdfs/                            — Reference materials & exercise walkthroughs
    │   └── dfa.c                            — Interactive DFA simulator
    ├── Computer_Networks/                   — End-to-end networking lab in C (POSIX sockets) and NS-3
    │   ├── EX0 – EX10                       — Layering, BISYNC/HDLC/PPP framing, CRC, Hamming, Dijkstra/Bellman-Ford, Sockets, NS-3
    │   ├── ARP-TCP                          — TCP address resolution service
    │   ├── Current-Time-TCP                 — Iterative vs concurrent daytime service
    │   ├── FTP-TCP                          — Socket file transfer system
    │   ├── Go-Back-N                        — GBN sliding window ARQ flow control
    │   ├── Selective-Reapeat-TCP            — Selective Repeat ARQ flow control
    │   ├── Stop-and-Wait-TCP                — Stop-and-Wait 1-bit flow control
    │   ├── String-Operations-TCP            — Remote string transformations
    │   ├── Model2                           — Consolidated exam revision programs
    │   └── Exercise_Questions               — LeetCode bit manipulation & string problems
    ├── Operating Systems Laboratory/        — Kernel mechanisms and system programming in C
    │   ├── EX1                              — Process creation and hierarchies (fork)
    │   ├── EX2                              — Full-duplex two-way pipes
    │   ├── EX3                              — System V message queues & shared memory
    │   ├── EX4                              — CPU scheduling (FCFS, SJF, Priority, Round Robin)
    │   ├── EX5                              — Process synchronization with semaphores
    │   ├── EX6                              — Banker's deadlock avoidance
    │   ├── EX7                              — Dynamic contiguous memory allocation (First, Best, Worst Fit)
    │   ├── EX8                              — Page replacement algorithms (FIFO, LRU, Optimal)
    │   └── EX11                             — Disk scheduling algorithms (FCFS, SSTF, SCAN, C-SCAN, LOOK, C-LOOK)
    ├── Web-user_interface_design/           — Responsive web applications
    │   └── College_Course_Registration_Web_Application — Multi-page academic registration portal (HTML5/CSS3/JS)
    └── FreeOpenSourceSoftware/              — Open source software assignments
        ├── Assignment_2/                    — TextTools (`wordkit`) Python package & test suite
        └── Assignment_3/                    — Campus Task Planner 4-table relational PHP/MySQL web application
```

## Compilation and Execution

### C Programs (OS & Networks)
```bash
gcc filename.c -o output
./output
```

### C++ Programs (DAA & Practice)
```bash
g++ -std=c++17 filename.cpp -o output
./output
```

### Lex & YACC (Compiler Design)
```bash
yacc -d parser.y
lex parser.l
cc y.tab.c lex.yy.c -o parser
./parser
```

### NS-3 Network Simulation
```bash
./ns3 run scratch/filename
```

## Purpose

This repository serves as an **academic record and comprehensive reference** demonstrating algorithms, operating systems fundamentals, network protocol engineering, compiler construction, and software design.
