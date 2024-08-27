@echo off

setlocal enabledelayedexpansion

REM Get the current directory
cd ..
set "currentDir=%CD%"

REM Extract the folder name from the path
for %%I in ("%currentDir%") do set "folderName=%%~nxI"

REM Display the folder name
echo Current Folder: %folderName%
echo Deleting %foldername%.hex
del ..\HEX\%folderName%.hex
cd Debug
IF EXIST "CRC" (
    del "CRC\%folderName%.hex"
) ELSE (
    echo Folder CRC\%folderName%.hex does not exist, skipping delete.
)


