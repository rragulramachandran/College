# College Lab Programs

A collection of laboratory programs completed during undergraduate coursework at Mepco Schlenk Engineering College.

## Repository Structure

```
College/
├── Semester_4/
│   └── Design_and_Analysis_Of_Algorithm/   # DAA Lab (CS3401)
└── Semester_5/
    └── Computer_Networks/                  # CN Lab (CS3591)
```

## Semesters

| Semester | Subject | Language | Exercises |
|----------|---------|----------|-----------|
| 4 | Design and Analysis of Algorithms | C++ | 9 |
| 5 | Computer Networks | C | 3 |

## How to Compile

**C++ programs (DAA):**
```bash
g++ filename.cpp -o output
./output
```

**C programs (CN):**
```bash
gcc filename.c -o output
./output
```

For multi-file programs (e.g., CN EX2):
```bash
gcc Main.c Sender.c Reciever.c -o output
./output
```