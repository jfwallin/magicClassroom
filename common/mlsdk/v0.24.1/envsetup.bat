@echo off

call :realpath_noslash "%~dp0" _ROOT

call :realpath_noslash "%MLSDK%" _X

if DEFINED MLSDK (
    if "%_X%" NEQ "" (
        if /I NOT "%_ROOT%" == "%_X%" (
            echo *** Replacing MLSDK=%MLSDK%...
        )
    )
)

echo *** Setting MLSDK=%_ROOT% and updating PATH
set "MLSDK=%_ROOT%"

PATH=%_ROOT%;%_ROOT%\internal\tools\mldb;%_ROOT%\tools\mldb;%_ROOT%\lumin\runtime\mxs\bin;%PATH%

goto :exit

:realpath_noslash
  @rem get canonical path of argument 1, without a trailing "\", and write to variable named in argument 2
  set "_T=%~df1"
  if [%_T:~-1%] == [\] (
    set "%~2=%_T:~0,-1%"
  ) else (
    set "%~2=%_T%"
  )
  exit /b

:exit

set _X=
set _T=
set _ROOT=
