@echo off
setlocal

:: set BACKTRACEPYTHON if not already set
if not defined BACKTRACEPYTHON (
    set "BACKTRACEPYTHON=%~dp0\tools\python3\python.exe"
)
"%BACKTRACEPYTHON%" -E "%~dp0/tools/backtrace/backtrace.py" %*
