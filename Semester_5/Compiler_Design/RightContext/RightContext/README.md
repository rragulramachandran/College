# Lex context programs

The 14 numbered programs follow the question order and the Lex structure in `EX2/Unary_MInus.l`. Each `main()` contains only `yylex();` and `return 0;`. None opens or writes files.

Compile and run one program at a time:

```sh
cd ModelPractive/RightContext
flex 01_if_keyword.l
cc lex.yy.c -o scanner
./scanner
```

Enter input, then press Ctrl+D to finish. Optional shell redirection also works: `./scanner < input.txt`.

| File | Context demonstrated | Example input |
| --- | --- | --- |
| `01_if_keyword.l` | Right context `/` | `if(x) if (x)` |
| `02_assignment_context.l` | Right context `/` | `a=1; b+2; c==3;` |
| `03_fortran_do.l` | Right context `/` | `DO 5 I = 1,25` versus `DO 5 I = 1` |
| `04_function_name.l` | Right context `/` | `fun(x); fun;` |
| `05_label_recognition.l` | Right context `/` | `case 5: next:` |
| `06_struct_tag.l` | `%s TAG`, `BEGIN` | `struct Person p; Person q;` |
| `07_comment_state.l` | `%s COMMENT`, `BEGIN` | `a /* ignored */ b` |
| `08_header_filename.l` | `%s HEADER`, `BEGIN` | `#include <stdio.h>` versus `"hello"` |
| `09_goto_label.l` | `%s LABEL`, `BEGIN` | `goto end; end;` |
| `10_template_brackets.l` | `%s EXPECT`, `BEGIN`, nesting depth | `vector<vector<int>> v; a < b;` |
| `11_string_state.l` | `%s STRING`, `BEGIN` | `if "if + return" +` |
| `12_member_decimal.l` | `%s AFTER_ID`, `BEGIN` | `obj.field 12.5 .5 obj .field` |
| `13_struct_both_contexts.l` | Left state and right context `/` | `struct Node { int x; }; struct Node n;` |
| `14_pointer_declarator.l` | Left state and right context `/` | `int *p; a*b;` |

Question 1 requires `(` immediately after `if`; `if (` is classified as an identifier. Other right-context programs allow spaces/tabs where appropriate. Trailing context is checked without consuming it, so the following token is still scanned.

These are individual classroom context demonstrations, not full language parsers:

- Question 3 supports the numeric bounds in the supplied examples. The assignment output removes spaces from the left-hand side to show `DO5I=`.
- Question 5 recognizes numeric `case` labels and identifier labels.
- Question 10 knows `vector`, `list`, `map`, and `set`; a following `<` opens a template, and matching `>` characters close nested templates. It does not resolve arbitrary C++ expression/type ambiguities.
- Question 12 requires the member dot to immediately follow an identifier. Whitespace clears that context.
- Question 13 applies the two requested right contexts: `{` or a following variable identifier. Other tag uses, such as `struct Node;`, fall back to ordinary identifier classification.
- Question 14 recognizes a single `*` after a type keyword and before an identifier. It does not parse compound declarators, multiple pointers, qualifiers, or typedef names.
