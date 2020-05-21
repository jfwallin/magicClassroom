@echo off
setlocal
set ROOT=%~dp0\tools\mabu\tools\MinGW
PATH=%ROOT%\bin;%ROOT%\msys\1.0\bin;%PATH%

if not defined MABUPYTHON (
  set "MABUPYTHON=%~dp0\tools\python3\python.exe"
)

set "SCRIPT_DIR=%~dp0/tools/mabu"
"%MABUPYTHON%" -E "%SCRIPT_DIR%/src/mabu.py" %*
