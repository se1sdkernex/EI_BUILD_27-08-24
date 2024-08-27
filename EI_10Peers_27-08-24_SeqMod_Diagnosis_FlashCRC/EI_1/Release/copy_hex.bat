@echo off

setlocal enabledelayedexpansion

set "hexFile="

REM Iterate through files in the current directory
for %%f in (*.hex) do (
    REM Store the filename with extension in the variable
    set "hexFile=%%~nxf"
    REM Display or use the variable as needed
	echo Found .hex file: !hexFile!
	STM32_HEX_CRC.exe !hexFile!
	mkdir ..\..\HEX
	echo copying hex file to HEX folder
	copy CRC\*.hex ..\..\HEX\
    echo Found .hex file: %hexFile%
    REM Exit loop after finding the first .hex file (remove if you need to find all)
    exit /b
)

REM Check if a .hex file was found
if "%hexFile%"=="" (
    echo No .hex file found in the current directory.
) else (
    echo Found .hex file: %hexFile%
)

endlocal
