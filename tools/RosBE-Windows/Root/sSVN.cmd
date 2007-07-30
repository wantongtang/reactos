::
:: PROJECT:     RosBE - ReactOS Build Environment for Windows
:: LICENSE:     GPL - See COPYING in the top level directory
:: FILE:        Root/SVN.cmd
:: PURPOSE:     Integrated SVN Client.
:: COPYRIGHT:   Copyright 2007 Daniel Reimer <reimer.daniel@freenet.de>
::
::
@echo off

::
:: Receive the first Parameter and decide what to do.
::
if "%1" == "" (
    echo No parameter specified. Try 'help [COMMAND]'.
    goto :ExitSVN
)
::
:: These two are directly parsed to svn.
::
if "%1" == "update" (
    title Updating...
    if not "%2" == "" (
        svn update -r %2
    ) else (
        svn update
    )
goto :ExitSVN
)
if "%1" == "cleanup" (
    title Cleaning...
    svn cleanup
    goto :ExitSVN
)
::
:: Check if the Folder is empty. If not, output an error.
::

if "%1" == "create" (
    title Creating...
    if exist ".svn\." (
        echo Folder already cotains a Reposority. Exiting
        goto :ExitSVN
    )
    dir /b 2>nul|findstr "." >nul
    if errorlevel 1 (
        svn checkout svn://svn.reactos.org/reactos/trunk/reactos
    ) else (
        echo Folder is not empty. Continuing is dangerous and can cause errors. ABORTED
    )
    goto :ExitSVN
)
::
:: Output the rev of your and the Online Tree and tell the User if
:: its Up to Date or not.
::
if "%1" == "status" (
    title Status
    for /f "usebackq" %%i IN (`svnversion .`) DO @set OFFSVN=%%i
    svn info svn://svn.reactos.org/reactos/trunk/reactos|find "Revision:"|cutz svn > "%ROSBEBASEDIR%\onsvn.tmp"
    set /P ONSVN=< "%ROSBEBASEDIR%\onsvn.tmp"
    call :UP
    goto :ExitSVN
)

if not "%1" == "" (
    echo Unknown parameter specified. Try 'help [COMMAND]'.
    goto :ExitSVN
)

:UP
echo Recent Offline Revision: %OFFSVN%
echo Online HEAD Revision: %ONSVN%
del "%ROSBEBASEDIR%\onsvn.tmp"
echo.
if %OFFSVN% LSS %ONSVN% (
    echo Your Tree is not Up to date. Do you want to update it?
    goto :UP2
)
if %OFFSVN% EQU %ONSVN% (
    echo Your Tree is Up to date.
    goto :ExitSVN
)

:UP2
SET /P XY="(yes), (no)"
if /I "%XY%"=="yes" %ROSBEBASEDIR%\ssvn update
if /I "%XY%"=="no" goto :ExitSVN

:ExitSVN
title ReactOS Build Environment %_VER%
