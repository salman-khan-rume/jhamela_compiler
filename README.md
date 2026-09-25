# JHAMELA Compiler (ঝামেলা কম্পাইলার)

A complete Bangla programming language compiler with GUI, built in **C++17**, generating **executable Python** code.

**Course:** CSE-4114 — Compiler Design and Construction Sessional
**Team:** Salman Khan Rume & Akash Das
**Institution:** Leading University, Sylhet

---

## 📋 Project Overview

**JHAMELA** is a toy Bangla programming language compiler. It lets you write programs using original Bengali keywords and compiles them to valid Python source that runs on any Python 3.8+ interpreter.

- **Source Language:** Bengali syntax (custom invented)
- **Target Language:** Python 3
- **Compiler Implementation:** C++17 (compiled language — allowed by project rules)
- **GUI:** Python (Tkinter) — front-end only, not part of the compiler
- **Architecture:** Lexer → Parser → AST → Semantic Analyzer → Code Generator

---

## ✨ Features Implemented

### Mandatory Requirements (CSE-4114 §2.1)

- ✓ **Three data types with type checking:** `পূর্ণসংখ্যা` (int), `সত্যমূল্য` (bool), `টেক্সট` (string)
- ✓ **Arithmetic with correct precedence:** `+ - * /` and unary `-`
- ✓ **Comparison operators:** `>  <  >=  <=  ==  !=`
- ✓ **Assignment statements:** `রাখো`
- ✓ **Conditional control:** `যদি` / `নাহলে` (If / Else)
- ✓ **Looping control:** `যতক্ষণ` (While)
- ✓ **Print statement:** `দেখাও`
- ✓ **Boolean literals:** `সত্য` (true), `মিথ্যা` (false)
- ✓ **Syntax error recovery:** skips to `;`, `}`, EOL, or EOF
- ✓ **No runtime crashes:** graceful UTF-8 handling, exception-safe
- ✓ **Generates valid, executable Python code** (`output.py`)

### Additional Features

- Python GUI (Tkinter) with vertical split layout
- Tabbed output: Tokens, Parse Tree, Python Code, Execution Output
- Real-time compilation feedback
- Symbol table display with type + initialization status
- Parse tree visualization
- One-click compile + run

---

## 🏗️ Architecture

```
┌──────────────────────────────────────────────────────┐
│                 JHAMELA COMPILER                     │
├──────────────────────────────────────────────────────┤
│                                                      │
│  Input (.jh)                                         │
│     ↓                                                │
│  [Lexer]  (lexer.h / lexer.cpp)                      │
│     ↓ Tokens                                         │
│  [Parser] (parser.h / parser.cpp)                    │
│     ↓ AST                                            │
│  [Semantic Analyzer] (semantic.h)                    │
│     ↓ Type-checked AST                               │
│  [Code Generator] (codegen.h)                        │
│     ↓ Python code                                    │
│  output.py                                           │
│     ↓                                                │
│  [Python Runtime] → Execution Output                 │
│                                                      │
└──────────────────────────────────────────────────────┘
```

### File Structure

```
jhamela_compiler/
├── lexer.h                # Token definitions + Lexer class
├── lexer.cpp              # Tokenizer implementation
├── parser.h               # Recursive-descent parser
├── parser.cpp
├── ast.h                  # AST node classes
├── semantic.h             # Type checker + symbol table
├── codegen.h              # Python code generator
├── main.cpp               # Compiler driver
├── jhamela_gui.py         # Tkinter GUI
├── build.bat              # Windows build script
├── build.sh               # Linux/Mac build script
├── GRAMMAR.bnf            # Formal grammar (BNF)
├── input.jh               # Example source program
└── README.md              # This file
```

---

## 🛠️ Setup & Build

### Prerequisites

**Windows:**
- MinGW-w64 g++ (C++17 support)
- Python 3.8+ with tkinter

**Linux / macOS:**
- g++ 7+ (C++17 support)
- Python 3.8+ with tkinter

### Build Steps

#### Windows

```bat
build.bat
```

Creates `jhamela.exe`.

#### Linux / macOS

```bash
chmod +x build.sh
./build.sh
```

Creates `jhamela`.

#### Manual Compilation

```bash
# Windows
g++ -std=c++17 -Wall -Wextra lexer.cpp parser.cpp main.cpp -o jhamela.exe

# Linux / macOS
g++ -std=c++17 -Wall -Wextra lexer.cpp parser.cpp main.cpp -o jhamela
```

---

## 🎯 Usage

### GUI Mode (Recommended)

```bash
# Windows
python jhamela_gui.py

# Linux / macOS
python3 jhamela_gui.py

# With custom compiler path
python jhamela_gui.py /path/to/jhamela
```

**GUI Layout:**
- **Left pane:** Bengali source editor
- **Right pane:** four tabs —
  - 🔤 Tokens — lexical analysis output
  - 🌳 Parse Tree — AST visualization
  - 🐍 Python Code — generated `output.py`
  - ▶️ Execution Output — program output
- **Buttons:** Compile · Run · Save · Load · Clear

### CLI Mode

```bash
# Compile a Bengali source file
jhamela.exe input.jh            # Windows
./jhamela input.jh               # Linux / macOS

# Run the generated Python
python output.py                 # Windows
python3 output.py                # Linux / macOS
```

---

## 📝 JHAMELA Language Reference

### Types

| Bengali | English | Example |
|---|---|---|
| `পূর্ণসংখ্যা` | integer | `42`, `-5`, `০` |
| `সত্যমূল্য` | boolean | `সত্য`, `মিথ্যা` |
| `টেক্সট` | string | `"হ্যালো"` |

### Keywords

| Bengali | Meaning | Category |
|---|---|---|
| `ধরো` | declare | Declaration |
| `পূর্ণসংখ্যা` | int type | Type |
| `সত্যমূল্য` | bool type | Type |
| `টেক্সট` | string type | Type |
| `সত্য` | true | Literal |
| `মিথ্যা` | false | Literal |
| `রাখো` | assign | Statement |
| `যদি` | if | Control |
| `নাহলে` | else | Control |
| `যতক্ষণ` | while | Loop |
| `দেখাও` | print | Statement |

### Operators

| Symbol | Meaning | Precedence |
|---|---|---|
| `-` (unary) | negation | 1 (highest) |
| `*` `/` | multiply, divide | 2 |
| `+` `-` | add, subtract | 3 |
| `>` `<` `>=` `<=` | ordering | 4 |
| `==` `!=` | equality | 4 |
| `=` | assignment | — |

---

## 💡 Example Programs

### Example 1 — Hello, world

```jhamela
দেখাও "হ্যালো বিশ্ব";
```

Generated Python:

```python
print("হ্যালো বিশ্ব")
```

### Example 2 — Arithmetic with precedence

```jhamela
ধরো পূর্ণসংখ্যা ক;
ধরো পূর্ণসংখ্যা খ;

রাখো ক = ২ + ৩ * ৪;
রাখো খ = (२ + ३) * ४;
দেখাও ক;
দেখাও খ;
```

Output: `14`, then `20`.

### Example 3 — If / Else

```jhamela
ধরো পূর্ণসংখ্যা ক;
রাখো ক = ১৫;

যদি ক > ১০ {
    দেখাও "বড় সংখ্যা";
} নাহলে {
    দেখাও "ছোট সংখ্যা";
}
```

Output: `বড় সংখ্যা`

### Example 4 — While loop

```jhamela
ধরো পূর্ণসংখ্যা ক;
রাখো ক = ৫;

যতক্ষণ ক > ০ {
    দেখাও ক;
    রাখো ক = ক - ১;
}
```

Output:

```
5
4
3
2
1
```

### Example 5 — Boolean variable

```jhamela
ধরো সত্যমূল্য প;
রাখো প = ১০ > ৫;

যদি প {
    দেখাও "সত্য";
} নাহলে {
    দেখাও "মিথ্যা";
}
```

Output: `সত্য`

---

## 🔍 Compilation Phases

### 1. Lexical Analysis

```
Input:   ধরো পূর্ণসংখ্যা ক;
Output:  KEYWORD(ধরো)  KEYWORD(পূর্ণসংখ্যা)  IDENTIFIER(ক)  PUNCTUATION(;)
```

### 2. Syntax Analysis (Recursive Descent)

```
Declaration
├── Type: পূর্ণসংখ্যা
└── Identifier: ক
```

### 3. Semantic Analysis

```
=== SYMBOL TABLE ===
ক : int (no)
```

### 4. Code Generation

```python
# Declare: ক (পূর্ণসংখ্যা)
```

---

## ⚠️ Error Handling

### Compile-Time Errors

| Error | Example | Detection |
|---|---|---|
| Undeclared variable | `দেখাও ক;` (ক not declared) | Semantic |
| Use before initialization | `দেখাও ক;` before `রাখো ক = ...;` | Semantic |
| Duplicate declaration | Same name declared twice | Semantic |
| Type mismatch on assignment | `ধরো সত্যমূল্য প; রাখো প = ৫;` | Semantic |
| Arithmetic on non-int | `প + ১` where প is bool | Semantic |
| Ordering on non-int | `"a" > ৫` | Semantic |
| Syntax error | Missing `;` or `}` | Parser |

### Error Recovery Strategy

`Parser::synchronize()` recovers by:

1. Always advancing at least one token (no infinite loops).
2. Stopping after consuming `;`.
3. Stopping before `}` so the enclosing block can close cleanly.
4. Stopping when the line number increases (end-of-line).
5. Stopping at `EOF`.

This lets the compiler report **multiple errors in one pass** instead of dying on the first.

### No-Crash Guarantees

- UTF-8 bytes cast to `unsigned char` before `isspace` (avoids UB).
- File I/O wrapped in try/catch in `main.cpp` and the GUI.
- Code generator throws descriptive `std::runtime_error` on unknown nodes.
- Semantic analyzer throws on undeclared/uninitialized variables.
- `main.cpp` catches all `std::exception` and exits with code 1.
- GUI shows error dialogs instead of crashing on compiler failure.

---

## 📊 Test Cases

| # | Tests | Expected |
|---|---|---|
| 1 | Arithmetic precedence + parentheses | `14`, `20` |
| 2 | If / Else branch | `বড়` |
| 3 | While loop counting down | `5 4 3 2 1` |
| 4 | Boolean variable from comparison | `সত্য` |
| 5 | Nested while + if | `বড় বড় ছোট` |
| 6 | Syntax error recovery | Compiler continues after bad line |
| 7 | Type-checking error | Semantic error raised |

Full sample programs for each test are in `input.jh` and the GUI's built-in sample.

---

## 📚 Formal Grammar

See [`GRAMMAR.bnf`](GRAMMAR.bnf) for the complete Backus-Naur Form grammar of the JHAMELA source language.

---

## 🎓 CSE-4114 Compliance Checklist

- [x] Invented Bangla toy language with custom syntax
- [x] Two (actually three) data types with type checking
- [x] Arithmetic operations with correct precedence
- [x] Assignment statements
- [x] If-Else conditional
- [x] While loop
- [x] Basic syntax error recovery (`;` / EOL / `}`)
- [x] No runtime crashes (exception-safe)
- [x] Generates valid executable Python
- [x] Compiler written in a compiled language (C++17)
- [x] Formal grammar in BNF (`GRAMMAR.bnf`)
- [ ] WebAssembly target — **Not implemented** (optional)
- [ ] Final Report — separate deliverable
- [ ] Live Presentation — separate deliverable

---

## 🐛 Known Limitations

- Single global scope (no nested scopes)
- No user-defined functions
- No arrays / lists
- No `for` loops
- No comments in the source language
- Single-pass compilation
- No constant folding / optimization passes

## 🚀 Future Work

- Function definitions
- Array / list type
- `for` loop
- Multiple scopes
- Source-level comments
- Better error positions (column tracking)
- Optimization passes
- Optional: WebAssembly backend

---

## 📞 Troubleshooting

| Problem | Fix |
|---|---|
| `g++ not found` | Install MinGW-w64 (Windows) or `build-essential` (Linux) |
| `tkinter not available` | Linux: `sudo apt install python3-tk` · Mac: `brew install python-tk` |
| `jhamela.exe not found` | Run `build.bat` (Windows) or `./build.sh` (Linux/Mac) first |
| UTF-8 encoding issues | Save source files as UTF-8. Use a UTF-8-aware editor. |
| GUI shows `(Python file not generated)` | Compilation failed — check the Tokens tab for errors |

---

## 📄 License

Educational project. Free to use and modify for academic purposes.

---

**Last Updated:** September 2026
**Status:** Mandatory compiler features complete ✓