@echo off
setlocal

if not defined LABPYTHON (
  set "LABPYTHON=%~dp0\tools\python3\python.exe"
)

set "SCRIPT_DIR=%~dp0/tools/lab_finder"
"%LABPYTHON%" -E "%SCRIPT_DIR%/main.py" %*
