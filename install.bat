@echo off
REM Arduino Nicla Voice Libraries Installation Script for Windows
REM This script installs the modified NDP library and examples for Nicla Voice

setlocal enabledelayedexpansion

echo ==================================================
echo Arduino Nicla Voice Libraries Installation
echo ==================================================
echo This script will install the modified NDP library for Nicla Voice
echo.

REM Configuration for Windows
set "ARDUINO_LIBRARIES_DIR=%USERPROFILE%\AppData\Local\Arduino15\packages\arduino\hardware\mbed_nicla\4.4.1\libraries"
set "ARDUINO_EXAMPLES_DIR=%USERPROFILE%\Documents\Arduino\libraries\NiclaVoice-Examples"
set "BACKUP_DIR=%USERPROFILE%\arduino-nicla-backup-%date:~-4,4%%date:~-10,2%%date:~-7,2%_%time:~0,2%%time:~3,2%%time:~6,2%"
set "BACKUP_DIR=%BACKUP_DIR: =0%"

echo Detected OS: Windows
echo Arduino Libraries Path: %ARDUINO_LIBRARIES_DIR%
echo Examples Path: %ARDUINO_EXAMPLES_DIR%
echo Backup Directory: %BACKUP_DIR%
echo.

REM Function to print status
:print_status
set "status=%1"
set "message=%2"
if "%status%"=="INFO" (
    echo [INFO] %message%
) else if "%status%"=="SUCCESS" (
    echo [SUCCESS] %message%
) else if "%status%"=="WARNING" (
    echo [WARNING] %message%
) else if "%status%"=="ERROR" (
    echo [ERROR] %message%
) else (
    echo [%status%] %message%
)
goto :eof

REM Check if running as administrator
net session >nul 2>&1
if %errorLevel% neq 0 (
    call :print_status "WARNING" "Not running as administrator. Some operations may require elevated privileges."
    echo.
)

REM Check if Arduino IDE is installed
if not exist "%ARDUINO_LIBRARIES_DIR%" (
    call :print_status "ERROR" "Arduino IDE with Nicla Voice support not found!"
    call :print_status "ERROR" "Expected path: %ARDUINO_LIBRARIES_DIR%"
    call :print_status "INFO" "Please install Arduino IDE and the Nicla Voice board package first."
    echo.
    call :print_status "INFO" "Installation steps:"
    echo 1. Install Arduino IDE from https://www.arduino.cc/en/software
    echo 2. Open Arduino IDE
    echo 3. Go to Tools ^> Board ^> Boards Manager
    echo 4. Search for "Nicla Voice" and install the Arduino Mbed OS Nicla Boards package
    echo 5. Run this script again
    pause
    exit /b 1
)

call :print_status "SUCCESS" "Arduino IDE with Nicla Voice support found!"

REM Create backup directory
if not exist "%BACKUP_DIR%" (
    mkdir "%BACKUP_DIR%"
    call :print_status "SUCCESS" "Created backup directory: %BACKUP_DIR%"
)

REM Backup existing NDP library
if exist "%ARDUINO_LIBRARIES_DIR%\NDP" (
    call :print_status "INFO" "Backing up existing NDP library..."
    xcopy "%ARDUINO_LIBRARIES_DIR%\NDP" "%BACKUP_DIR%\NDP" /E /I /H /Y >nul
    if %errorLevel% equ 0 (
        call :print_status "SUCCESS" "Backup completed successfully!"
    ) else (
        call :print_status "WARNING" "Backup may have failed, but continuing..."
    )
) else (
    call :print_status "INFO" "No existing NDP library found to backup."
)

REM Install new NDP library
call :print_status "INFO" "Installing enhanced NDP library..."

REM Create NDP directory if it doesn't exist
if not exist "%ARDUINO_LIBRARIES_DIR%\NDP" (
    mkdir "%ARDUINO_LIBRARIES_DIR%\NDP"
)

REM Copy NDP library files
if exist "NDP\src\NDP.h" (
    copy "NDP\src\NDP.h" "%ARDUINO_LIBRARIES_DIR%\NDP\" >nul
    call :print_status "SUCCESS" "Copied NDP.h"
) else (
    call :print_status "ERROR" "NDP.h not found in current directory!"
    pause
    exit /b 1
)

if exist "NDP\src\NDP.cpp" (
    copy "NDP\src\NDP.cpp" "%ARDUINO_LIBRARIES_DIR%\NDP\" >nul
    call :print_status "SUCCESS" "Copied NDP.cpp"
) else (
    call :print_status "ERROR" "NDP.cpp not found in current directory!"
    pause
    exit /b 1
)

if exist "NDP\library.properties" (
    copy "NDP\library.properties" "%ARDUINO_LIBRARIES_DIR%\NDP\" >nul
    call :print_status "SUCCESS" "Copied library.properties"
)

REM Create examples directory
if not exist "%ARDUINO_EXAMPLES_DIR%" (
    mkdir "%ARDUINO_EXAMPLES_DIR%"
    call :print_status "SUCCESS" "Created examples directory: %ARDUINO_EXAMPLES_DIR%"
)

REM Copy examples
call :print_status "INFO" "Installing example sketches..."

if exist "examples" (
    xcopy "examples" "%ARDUINO_EXAMPLES_DIR%" /E /I /H /Y >nul
    if %errorLevel% equ 0 (
        call :print_status "SUCCESS" "Examples installed successfully!"
    ) else (
        call :print_status "WARNING" "Examples installation may have failed."
    )
) else (
    call :print_status "WARNING" "Examples directory not found."
)

REM Create library.properties for examples
echo name=NiclaVoice-Examples > "%ARDUINO_EXAMPLES_DIR%\library.properties"
echo version=1.0.0 >> "%ARDUINO_EXAMPLES_DIR%\library.properties"
echo author=Mariano Abad ^(fxd0h^) >> "%ARDUINO_EXAMPLES_DIR%\library.properties"
echo maintainer=Mariano Abad ^(fxd0h^) >> "%ARDUINO_EXAMPLES_DIR%\library.properties"
echo sentence=Enhanced examples for Nicla Voice with improved NDP library >> "%ARDUINO_EXAMPLES_DIR%\library.properties"
echo paragraph=Examples demonstrating the enhanced NDP library features including logging control and improved performance. >> "%ARDUINO_EXAMPLES_DIR%\library.properties"
echo category=Other >> "%ARDUINO_EXAMPLES_DIR%\library.properties"
echo url=https://github.com/fxd0h/arduino-nicla-ndp-enhanced >> "%ARDUINO_EXAMPLES_DIR%\library.properties"
echo architectures=* >> "%ARDUINO_EXAMPLES_DIR%\library.properties"

call :print_status "SUCCESS" "Examples library.properties created!"

REM Final verification
call :print_status "INFO" "Verifying installation..."

if exist "%ARDUINO_LIBRARIES_DIR%\NDP\NDP.h" (
    call :print_status "SUCCESS" "NDP.h verified!"
) else (
    call :print_status "ERROR" "NDP.h verification failed!"
)

if exist "%ARDUINO_LIBRARIES_DIR%\NDP\NDP.cpp" (
    call :print_status "SUCCESS" "NDP.cpp verified!"
) else (
    call :print_status "ERROR" "NDP.cpp verification failed!"
)

if exist "%ARDUINO_EXAMPLES_DIR%\LoggingControlTest" (
    call :print_status "SUCCESS" "Examples verified!"
) else (
    call :print_status "WARNING" "Examples verification failed!"
)

echo.
echo ==================================================
echo INSTALLATION COMPLETED SUCCESSFULLY!
echo ==================================================
echo.
call :print_status "SUCCESS" "Enhanced NDP library installed!"
call :print_status "SUCCESS" "Example sketches installed!"
echo.
call :print_status "INFO" "Next steps:"
echo 1. Open Arduino IDE
echo 2. Go to File ^> Examples ^> NiclaVoice-Examples
echo 3. Try the LoggingControlTest example
echo 4. Upload to your Nicla Voice board
echo.
call :print_status "INFO" "Backup location: %BACKUP_DIR%"
call :print_status "INFO" "If you need to restore the original library, copy files from backup to:"
echo    %ARDUINO_LIBRARIES_DIR%\NDP\
echo.
pause
