#!/bin/bash
# Build script for JHAMELA Compiler on Linux/Mac

echo "========================================"
echo "  JHAMELA Compiler Build Script"
echo "========================================"

# Check if g++ exists
if ! command -v g++ &> /dev/null; then
    echo "Error: g++ not found. Install build-essential (Linux) or Xcode (Mac)."
    exit 1
fi

echo ""
echo "Compiling JHAMELA Compiler..."
g++ -std=c++17 -Wall -Wextra \
    lexer.cpp \
    parser.cpp \
    main_updated.cpp \
    -o jhamela

if [ $? -ne 0 ]; then
    echo "Compilation FAILED"
    exit 1
fi

echo ""
echo "✓ Compilation SUCCESSFUL"
echo "Generated: jhamela"
echo ""
echo "Next steps:"
echo "  1. Run GUI: python3 jhamela_gui.py"
echo "  2. Or use CLI: ./jhamela input.jh"
echo ""
