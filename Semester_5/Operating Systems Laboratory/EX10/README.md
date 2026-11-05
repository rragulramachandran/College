# EX10 - File Allocation Simulation

Simulation of sequential / contiguous file allocation and single-level directory management on secondary storage.

## Structure

- **BitVector_Directory/**: Contiguous file allocation using a bit vector (itVector[SIZE]) to track free/used disk blocks and a structured directory table (File directory[MAX]) with metadata (filename, start block, length, content).
  - pp.c: Interactive menu-driven driver program.
  - head.h: Data structures and function prototypes.
  - imp.c: Implementation of disk block allocation, directory listing, searching, content viewing, and file deletion.
- **Array_Based/**: Alternative sequential allocation implementation using parallel arrays for tracking file starts, lengths, disk blocks, and directory names.
  - pp.c: Menu-driven interface.
  - head.h: Header prototypes and global array declarations.
  - imp.c: Implementation logic.
- **docs/**: Assignment documentation and supplementary reference materials.

## Build and Run

### BitVector & Directory Simulation:
`ash
cd BitVector_Directory
gcc app.c imp.c -o file_alloc
./file_alloc
`

### Array-Based Simulation:
`ash
cd Array_Based
gcc app.c imp.c -o file_alloc_arr
./file_alloc_arr
`
