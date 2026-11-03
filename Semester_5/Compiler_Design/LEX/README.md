# Lex practice programs

Each numbered `.l` file answers the corresponding question. Every `main()` contains only `yylex();` and `return 0;`. Input comes from standard input; there are no input-file opening calls. End-of-input reporting and cleanup use `yywrap()` rather than `<<EOF>>` rules. C blocks use Allman braces; Lex requires each rule action to start on the same line as its pattern.

Compile and run any program, for example:

```sh
flex 01_count.l
cc lex.yy.c -o count
./count
```

Finish interactive input with Ctrl+D. To scan an existing file:

```sh
./count < input.txt
```

To save a transformed document (questions 6, 8, 9, 12, 15, or 16), redirect standard output:

```sh
flex 12_caesar_cipher.l
cc lex.yy.c -o encrypt
./encrypt < input.txt > encrypted.txt
```

Only the questions explicitly requesting separate output files open files themselves. These files are created in the current working directory and replaced on each run:

| Question | Output files |
| --- | --- |
| 7 | `emails.txt`, `urls.txt` |
| 18 | `binary_palindromes.txt` |
| 19 | `ab_palindromes.txt` |

Notes:

- Question 1 counts characters as bytes, words separated by whitespace, and lines as newline characters.
- Question 2 keeps the first longest string when lengths are equal.
- Question 4 recognizes signed integers, decimal fractions, and fractions such as `2/3`. Zero is reported separately.
- Question 7 extracts conventional email addresses and HTTP, HTTPS, FTP, and `www.` URLs. It does not attempt complete RFC email validation.
- Question 8 strips tags and HTML comments; entities such as `&amp;` remain unchanged.
- Question 10 lists absolute URLs and quoted or unquoted `href`/`src` values, including relative links.
- Question 14 preserves input whitespace and punctuation while annotating lexemes as in the question.
- Question 15 treats words as runs of alphabetical characters.
- Question 16 replaces runs of two or more spaces/tabs with one space; single tabs remain unchanged.
- Question 17 checks one complete string per line and rejects C keywords. It supports the basic ASCII identifier syntax.
- Questions 18 and 19 check whole whitespace-separated strings, count repeated occurrences, and reject strings containing characters outside the requested alphabet.
