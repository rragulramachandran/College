# Compiler Design Lab

Laboratory programs for the Compiler Design course, covering lexical analysis (Lex), syntax-directed translation (YACC), intermediate code generation, code optimisation, and DFA simulation.

All programs use **Lex/Flex** and **YACC/Bison** for scanner and parser construction, and **C** for standalone programs.

## Folder Structure

```text
Compiler_Design/
├── LEX/                    — 19 Lex programs for lexical analysis
├── RightContext/           — 14 Lex programs demonstrating right context and start conditions
├── calculator/             — Lex + YACC arithmetic expression validator
├── yacc/                   — YACC grammar validator for the language a^n b* c^n
├── Simple-YACC/            — 28 YACC parsers across 14 groups (language recognition + control-flow parsing)
├── quad/                   — Lex + YACC intermediate code generator (quadruples for assignments, if, while)
├── optimization/           — Code optimisation on three-address code (CSE, constant folding, copy propagation, dead code elimination)
├── pdfs/                   — Reference PDFs and explanations for lab exercises
├── dfa.c                   — DFA simulator with configurable transition table and string acceptance checking
└── .gitignore
```

## Exercise Summary

| # | Topic | Folder | Description |
|---|-------|--------|-------------|
| 1 | Lexical Analysis — Basic Lex | `LEX/` | 19 Lex programs: counting, tokenising C/SQL, pattern matching, palindrome detection, Caesar cipher, and more |
| 2 | Lexical Analysis — Right Context & Start Conditions | `RightContext/` | 14 Lex programs demonstrating trailing context (`/`), start conditions (`%s`), and nested state machines |
| 3 | Arithmetic Expression Validation | `calculator/` | Lex + YACC calculator that validates integer expressions with `+`, `-`, `*`, `/`, unary minus, and parentheses |
| 4 | Grammar Validation (YACC) | `yacc/` | YACC parser for the context-free language L = { a (b\*)+ c } |
| 5 | Syntax Analysis — YACC Parsers | `Simple-YACC/` | 28 YACC parsers (14 groups × 2 questions): formal language recognition and C/Java control-flow statement parsing |
| 6 | Intermediate Code Generation | `quad/` | Lex + YACC front-end that generates three-address code in quadruple form for assignments, `if`, and `while` statements |
| 7 | Code Optimisation | `optimization/` | C program applying Common Subexpression Elimination, Constant Folding, Copy Propagation, and Dead Code Elimination on quadruples |
| 8 | DFA Simulation | `dfa.c` | Interactive DFA simulator: reads a transition table, displays it, and checks whether input strings are accepted or rejected |

## Compilation

### Standalone C programs

```bash
gcc dfa.c -o dfa
./dfa
```

```bash
gcc optimization/opt.c -o opt
./opt
```

### Lex programs

```bash
flex LEX/01_count.l
cc lex.yy.c -o count
./count
```

### Lex + YACC programs

```bash
cd calculator
yacc -d cal.y
lex cal.l
cc y.tab.c lex.yy.c -o calc
./calc
```

```bash
cd quad
bison -d quad.y
flex quad.l
cc quad.tab.c lex.yy.c -o quad
./quad
```

### Simple-YACC parsers

```bash
cd Simple-YACC/Group-01-Question-01
yacc -d parser.y
lex parser.l
cc y.tab.c lex.yy.c -o parser
./parser < input.txt
```

## Reference Materials

The `pdfs/` folder contains explanatory documents for several exercises:

| File | Topic |
|------|-------|
| `Ex.No 1.pdf` | Lex lexical analyser overview |
| `lex_lexical_analyzer.pdf` | Lex-based lexical analyser details |
| `lex_tokenizer.pdf` | Lex tokeniser explanation |
| `lex_yacc_calculator.pdf` | Lex + YACC calculator walkthrough |
| `lex_yacc_grammar.pdf` | Lex + YACC grammar explanation |
| `lex_yacc_parser.pdf` | Lex + YACC parser details |
| `Ex No 6 explanation.pdf` | Intermediate code generation explanation |
| `dfa_program.pdf` | DFA simulation program explanation |
| `common Subexpression Elimination.pdf` | CSE optimisation |
| `copy_const_propagation_folding.pdf` | Copy propagation and constant folding |
