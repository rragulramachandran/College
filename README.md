# College Coursework

An organised archive of laboratory work, programming exercises, and academic assignments completed during undergraduate coursework at Mepco Schlenk Engineering College.

## Repository structure

```text
College/
├── FreeOpenSourceSoftware/
│   ├── Assignment_2/
│   └── Assignment_3/
├── Semester_4/
└── Semester_5/
    ├── Computer_Networks/
    ├── Operating Systems Laboratory/
    └── Web-user_interface_design/
```

The folders are grouped by course or semester. Refer to each assignment's own README for its specific requirements and run instructions.

## Free and Open Source Software assignments

- **Assignment 2 — TextTools (`wordkit`):** a modular Python package for word and character metrics, word frequency, sentence counting, and related text analysis. See [Assignment 2 README](FreeOpenSourceSoftware/Assignment_2/README.md).
- **Assignment 3 — Campus Task Planner:** a PHP and MySQL task-planning application with a four-table relational schema for categories, priorities, tasks, and checklists. See [Assignment 3 README](FreeOpenSourceSoftware/Assignment_3/README.md).

## Compiling programs

### C

```bash
gcc filename.c -o output
./output
```

### C++

```bash
g++ -std=c++17 filename.cpp -o output
./output
```

Replace the filename with the source file you want to run. Multi-file exercises may require compiling all related source files together, and some lab tasks need additional tools or setup documented in their folders.

## Purpose

This repository is an **academic archive and learning reference**, not a single deployable application. Larger projects are maintained in separate repositories.
