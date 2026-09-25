@echo off
REM Build script for JHAMELA Compiler on Windows

echo ========================================
echo   JHAMELA Compiler Build Script
echo ========================================

where g++ >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo Error: g++ not found. Install MinGW-w64.
    pause
    exit /b 1
)

echo.
echo Compiling JHAMELA Compiler...
g++ -std=c++17 -Wall -Wextra ^
    lexer.cpp ^
    parser.cpp ^
    main.cpp ^
    -o jhamela.exe

if %ERRORLEVEL% NEQ 0 (
    echo Compilation FAILED
    pause
    exit /b 1
)

echo.
echo Compilation SUCCESSFUL
echo Generated: jhamela.exe
echo.
echo Next steps:
echo   1. Run GUI: python jhamela_gui.py
echo   2. Or use CLI: jhamela.exe input.jh
echo.
pause