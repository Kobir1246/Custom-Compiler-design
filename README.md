# 🧠 Mini Compiler in C

## 📌 Overview

This project implements a simple compiler for a custom programming language using C. It covers core phases of compilation including lexical analysis, syntax parsing, symbol table management, and intermediate code generation.

The compiler reads source code from an input file and generates three-address intermediate code.

---

## ⚙️ Features

* Lexical Analysis using Flex (tokenization)
* Syntax Parsing using recursive descent parsing
* Support for:

  * Variable declarations (`int`, `float`)
  * Arithmetic expressions (`+`, `-`, `*`, `/`)
  * Assignment statements
  * Conditional statements (`if`)
  * Print statements
* Symbol Table for variable tracking and type management
* Intermediate Code Generation (Three-Address Code)

---

## 🏗️ Project Structure

```
mini-compiler-c/
│
├── main.c                # Entry point of the compiler
├── parser.c              # Syntax analysis and parsing logic
├── symbol_table.c        # Symbol table implementation
├── codegen.c             # Intermediate code generation
├── scanner_bridge.c      # Connects lexer with parser
├── token.h               # Token definitions
│
├── lexer.l               # Lexical analyzer (Flex)
├── parser.y              # (Optional) Yacc/Bison grammar
│
├── input.mc              # Sample input program
├── makefile              # Build configuration
└── Run_compiler.bat      # Windows execution script
```

---

## 🔄 Compilation Flow

```
Input Program → Lexer → Tokens → Parser → Symbol Table → Code Generator → Output
```

---

## 🧾 Example

### Input (`input.mc`)

```
int a = 5;
int b = 10;
print(a + b);
```

### Output (Intermediate Code)

```
a = 5
b = 10
t1 = a + b
print t1
```

---

## 🚀 How to Run

### 1. Build the Project

Using Makefile:

```
make
```

Or manually:

```
gcc main.c parser.c symbol_table.c codegen.c scanner_bridge.c -o compiler
```

---

### 2. Run the Compiler

```
./compiler input.mc
```

---

## 🧩 Technologies Used

* C Programming Language
* Flex (Lexical Analyzer)
* Basic Compiler Design Concepts

---

## 📚 Concepts Covered

* Lexical Analysis
* Syntax Analysis (Parsing)
* Symbol Table Management
* Intermediate Code Generation
* Three-Address Code (TAC)

---

## ⚠️ Limitations

* No optimization phase
* Limited grammar support
* Basic error handling
* No Abstract Syntax Tree (AST)

---

## 📌 Future Improvements

* Add semantic analysis
* Implement AST generation
* Add optimization phase
* Improve error reporting
* Support more complex language features

---

## 👤 Author

Shahriar Kobir Sabbir
CSE Graduate 

---
