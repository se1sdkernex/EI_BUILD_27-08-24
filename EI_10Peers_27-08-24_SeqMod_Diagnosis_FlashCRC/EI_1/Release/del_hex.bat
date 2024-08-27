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
echo Before_CD
cd Release
echo Before_Delete
del CRC\%folderName%.hex

