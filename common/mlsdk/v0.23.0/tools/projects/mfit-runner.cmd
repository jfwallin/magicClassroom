:: This script is the top-level driver for the mabu json tool.
:: For Magic Leap internal use only.
@echo off
setlocal
set ROOT=%~dp0\tools\mabu\tools\MinGW
set PATH=%ROOT%\bin;%ROOT%\msys\1.0\bin;%PATH%

:: set PYTHON if not already set
if not defined PYTHON (
    set PYTHON=%~dp0\..\python3\python.exe
)

"%PYTHON%" "%~dp0/../mabu/tools/mfit/mfit-runner.py" %*
