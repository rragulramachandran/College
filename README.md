# College Coursework

An organised archive of laboratory work, programming exercises, and academic assignments completed during undergraduate coursework at Mepco Schlenk Engineering College.

## Repository structure

```text
College/
├── Semester_4/
│   └── Design_and_Analysis_Of_Algorithm/
├── Semester_5/
│   ├── Compiler_Design/
│   │   ├── LEX/                — 19 Lex lexical analysis programs
│   │   ├── RightContext/       — 14 Lex right-context & start-condition programs
│   │   ├── calculator/         — Lex + YACC arithmetic expression validator
│   │   ├── yacc/               — YACC grammar validator (a^n b* c^n)
│   │   ├── Simple-YACC/        — 28 YACC parsers (14 groups × 2 questions)
│   │   ├── quad/               — Intermediate code generator (quadruples)
│   │   ├── optimization/       — Code optimisation (CSE, constant folding, copy propagation, dead code elimination)
│   │   ├── pdfs/               — Reference PDFs
│   │   └── dfa.c               — DFA simulator
│   ├── Computer_Networks/
│   ├── Operating Systems Laboratory/
│   ├── Web-user_interface_design/
│   └── FreeOpenSourceSoftware/
│       ├── Assignment_2/
│       └── Assignment_3/
```

The folders are grouped by course or semester. Refer to each assignment's own README for its specific requirements and run instructions.

## Semester 5 — Compiler Design

The newest addition to the repository. Covers the full compiler pipeline:

- **Lexical Analysis** — 19 Lex scanner programs plus 14 right-context / start-condition demonstrations
- **Syntax Analysis** — YACC parsers for arithmetic expressions, formal languages, and C/Java control-flow statements (28 parsers across 14 groups)
- **Intermediate Code Generation** — Lex + YACC front-end producing three-address code in quadruple form for assignments, `if`, and `while`
- **Code Optimisation** — Common Subexpression Elimination, Constant Folding, Copy Propagation, and Dead Code Elimination on quadruples
- **DFA Simulation** — Interactive DFA with configurable transition table and string acceptance checking

See the [Compiler Design README](Semester_5/Compiler_Design/README.md) for full details.

## Free and Open Source Software assignments

- **Assignment 2 — TextTools (`wordkit`):** a modular Python package for word and character metrics, word frequency, sentence counting, and related text analysis. See [Assignment 2 README](Semester_5/FreeOpenSourceSoftware/Assignment_2/README.md).
- **Assignment 3 — Campus Task Planner:** a PHP and MySQL task-planning application with a four-table relational schema for categories, priorities, tasks, and checklists. See [Assignment 3 README](Semester_5/FreeOpenSourceSoftware/Assignment_3/README.md).

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

### Lex

```bash
flex program.l
cc lex.yy.c -o scanner
./scanner
```

### Lex + YACC

```bash
yacc -d parser.y
lex parser.l
cc y.tab.c lex.yy.c -o parser
./parser
```

Replace the filename with the source file you want to run. Multi-file exercises may require compiling all related source files together, and some lab tasks need additional tools or setup documented in their folders.

## Purpose

This repository is an **academic archive and learning reference**, not a single deployable application. Larger projects are maintained in separate repositories.
