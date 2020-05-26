@echo off
setlocal

:: set DEBUGPYTHON if not already set
if not defined DEBUGPYTHON (
    set "DEBUGPYTHON=%~dp0\tools\python3\python.exe"
)
"%DEBUGPYTHON%" -E "%~dp0/tools/debug/debug.py" %*
