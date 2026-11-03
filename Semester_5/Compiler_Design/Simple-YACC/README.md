# Simple YACC Parsers

28 YACC parsers organised into 14 groups, each containing two questions:

- **Question 1** — Formal language recognition (e.g. `a b^n`, balanced parentheses, palindromes)
- **Question 2** — C/Java control-flow statement parsing (e.g. `for`, `while`, `if-else`, `switch`, `do-while`)

Each folder contains a `parser.l` (Lex scanner) and `parser.y` (YACC grammar). Build and run any parser:

```bash
cd Group-01-Question-01
yacc -d parser.y
lex parser.l
cc y.tab.c lex.yy.c -o parser
./parser < input.txt
```

## Group Index

| Group | Question 1 — Language/Grammar | Question 2 — Statement Parser |
|-------|------------------------------|-------------------------------|
| 01 | L = { a b^n \| n ≥ 1 } | For loop with assignment body |
| 02 | S → L=R \| R; L → *R \| id; R → L | While loop with assignment body |
| 03 | Balanced parentheses (including empty) | If-else with assignment in both branches |
| 04 | Arithmetic expressions (id+id*id) | Do-while loop |
| 05 | L = { a^n b^(2n) \| n ≥ 1 } | Switch with one or more cases |
| 06 | Nonempty bracketed identifier list [id,id,...] | For loop containing an if statement |
| 07 | Even-length palindromes over {a,b} | Function call with optional arguments |
| 08 | Assignment expression (id=id+num;) | While loop containing a for loop |
| 09 | L = { a^n b^n c^m \| n,m ≥ 1 } | Integer array declaration |
| 10 | Boolean expressions (not, and, or) | Pointer dereference assignment |
| 11 | Relational expression (id < num) | Struct declaration with integer members |
| 12 | L = { (ab)^n \| n ≥ 1 } | Java-style class with integer fields |
| 13 | Nested parentheses around an identifier | Ternary assignment (id = id ? num : id) |
| 14 | Dangling else (else binds to closest if) | Integer function definition with assignment body |
