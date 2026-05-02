@echo off
setlocal EnableDelayedExpansion
cd /d "%~dp0"
title MiniC Compiler - Menu
color 0A

:menu
echo.
echo ===============================
echo      MiniC Compiler Menu
echo ===============================
echo 1. Generate Lexer (flex)
echo 2. Generate Parser (bison)
echo 3. Build Compiler (gcc)
echo 4. Run Compiler on test.mc
echo 5. Enter Live MiniC Code (save to input.mc)
echo 6. Run Compiler on input.mc
echo 7. Clean Generated Files
echo 8. Exit
echo ===============================
echo.

set "choice="
set /p choice=Enter your choice (1-8): 

if "%choice%"=="" (
    echo Please choose a valid option!
    pause
    goto menu
)

if "%choice%"=="1" goto lexer
if "%choice%"=="2" goto parser
if "%choice%"=="3" goto build
if "%choice%"=="4" goto runtest
if "%choice%"=="5" goto liveeditor
if "%choice%"=="6" goto runinput
if "%choice%"=="7" goto clean
if "%choice%"=="8" goto exit

echo Invalid choice!
pause
goto menu

:lexer
echo.
echo [*] Generating lexer using Flex...
flex lexer.l
echo [+] Lexer generation completed successfully.
pause
goto menu

:parser
echo.
echo [*] Generating parser using Bison...
bison -d parser.y
echo [+] Parser generation completed successfully.
pause
goto menu

:build
echo.
echo [*] Building MiniC compiler...
gcc -g parser.tab.c lex.yy.c codegen.c symbol_table.c scanner_bridge.c main.c -o mycompiler
echo [+] Compiler build completed successfully.
pause
goto menu

:runtest
echo.
echo [*] Running compiler on test.mc...
mycompiler test.mc
echo [+] Execution of test.mc completed.
pause
goto menu

:liveeditor
echo.
echo ===== Live MiniC Code Input =====
echo Type MiniC code line by line.
echo Use MiniC syntax only.
echo Type END on a new line to finish.
echo.

type nul > input.mc

:inputloop
set "line="
set /p line=

if /I "!line!"=="END" goto doneinput

echo !line!>> input.mc
goto inputloop

:doneinput
echo.
echo [+] input.mc saved successfully.
echo ----- input.mc content -----
type input.mc
echo ----------------------------
pause
goto menu

:runinput
echo.
echo [*] Running compiler on input.mc...
mycompiler input.mc
echo [+] Execution of input.mc completed.
pause
goto menu

:clean
echo.
echo [*] Cleaning generated files...
del lex.yy.c 2>nul
del parser.tab.c 2>nul
del parser.tab.h 2>nul
del mycompiler.exe 2>nul
echo [+] Cleanup completed successfully.
pause
goto menu

:exit
echo Exiting MiniC Compiler Menu.
exit
