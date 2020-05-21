@echo off
if [%1] == "-cc1" goto :L
"%~dp0\clang.exe" -target aarch64-none-linux-android %*
if ERRORLEVEL 1 exit /b 1
goto :done
:L
rem target/triple already spelled out.
"%~dp0\clang.exe" %*
if ERRORLEVEL 1 exit /b 1
:done
