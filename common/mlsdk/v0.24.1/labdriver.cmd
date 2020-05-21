@echo off
setlocal

if not defined LABPYTHON (
  set "LABPYTHON=%~dp0\tools\python3\python.exe"
)

set "SCRIPT_DIR=%~dp0/tools/lab_finder"
set CMD="%LABPYTHON%" -E "%SCRIPT_DIR%/main.py" %*
for /F "usebackq delims=" %%L IN (`"%CMD%"`) do set "_LD=%%L"

if ["%_LD%"]==[""] (exit /b 9) else ("%_LD%" %*)
