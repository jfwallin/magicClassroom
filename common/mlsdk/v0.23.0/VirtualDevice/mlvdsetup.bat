@echo off
set _VDROOT=%~dp0
call :realpath %~dp0\.. _PARENT

if DEFINED MLSDK (
	@rem See if MLSDK is the parent of this folder, or, looks legit
    if /I NOT "%MLSDK%" == "%_PARENT%" (
        if NOT EXIST "%MLSDK%\.metadata" (
            @rem Probably a stale env var? 
            call "%_PARENT%\envsetup.bat"
            if ERRORLEVEL 1 (
                echo !!! Please run envsetup.bat from your MLSDK before running this script.
                exit /b 1
            )
        )
    )
) else (
    @rem If no MLSDK set, envsetup.bat was likely not run, either.
    @rem We assume this script lives in VirtualDevice above MLSDK.
    call "%_PARENT%\envsetup.bat" 
    if ERRORLEVEL 1 (
        echo !!! Please run envsetup.bat from your MLSDK before running this script.
        exit /b 1
    )
)

@rem locate CLI binaries, and, favor ML Remote DLL overrides over MLSDK stubs
PATH=%_VDROOT%bin;%_VDROOT%lib;%MLSDK%\lib\win64;%PATH%

set ML_VDS_DATA=%_VDROOT%data
echo *** Environment now set up to run tools and MLSDK apps under Zero Iteration.
goto :exit

:realpath
  @rem get canonical path of argument 1, write to variable named in argument 2
  set "%~2=%~df1"
  exit /b

:exit
set _VDROOT=
set _PARENT=

