# Assignment 2: TextTools Package (wordkit)

**Course:** 19CSP01 – Free Open Source Software  
**Student:** Ragul Ramachandran  
**Register Number:** 24BCS174  
**Institution:** MEPCO SCHLENK ENGINEERING COLLEGE (Autonomous), Sivakasi  

---

## Overview

`wordkit` is a modular, lightweight Python package designed for text analysis, metrics calculation, and string analytics. Developed as part of Assignment 2, this project demonstrates standard Python package design, local version control using Git, remote repository hosting on GitHub, and collaborative check-out / modification workflows.

## Package Architecture

```text
Assignment_2_TextTools/
├── .gitignore
├── README.md
├── demo.py
├── setup.py
├── tests/
│   └── test_text_metrics.py
└── wordkit/
    ├── __init__.py
    └── text_metrics.py
```

## Features

- **Word Count**: Calculates total words in text.
- **Unique Word Count**: Counts case-insensitive unique vocabulary words.
- **Longest Word**: Finds the longest word, ignoring punctuation.
- **Average Word Length**: Computes mean characters per word.
- **Character Count**: Evaluates characters with or without whitespaces.
- **Sentence Count**: Counts sentences using regex delimiters (`.!?`).
- **Word Frequency**: Returns the top N most frequent words.

## Installation & Usage

### 1. Local Package Installation

```bash
pip install .
```

### 2. Running Demo

```bash
python demo.py
```

### 3. Running Unit Tests

```bash
python -m unittest tests/test_text_metrics.py
```

---
Hosted on GitHub: [rragulramachandran/College](https://github.com/rragulramachandran/College)
