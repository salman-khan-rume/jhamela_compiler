# Jhamela Compiler

* **Implementation Language:** C++
* **Target Output:** Python

* **Architecture:**
  * Custom hand-written C++ Lexer (std::string exact UTF-8 byte matching to prevent crashes)
  * Custom hand-written Recursive Descent Parser.
  
* **Mandatory Features Included:** 
Type checking, basic syntax error recovery (skipping to `;` or `\n`), arithmetic precedence, assignments, if/else, while loops, print[cite: 3].

* **Syntax Rules:**
  * Block Delimiters: `{ }`
  * Statement Terminator: `;`
  * Comments: `//` (single) and `/* */` (multi)
  * Case Sensitivity: No (case-insensitive for ASCII characters).
  
* **Bengali Keywords:**
  * Types: পূর্ণসংখ্যা (Integer), সত্যমূল্য (Boolean)
  * Values: সত্য (True), মিথ্যা (False)
  * Control Flow: যদি (If), নাহলে (Else), যতক্ষণ (While)
  * Operations: দেখাও (Print), ধরো (Declare), রাখো (Assign), ফেরাও (Return), নাও (Input)
