@echo off
set ARDUINO_CLI="C:\Users\chick\AppData\Local\Programs\Arduino IDE\resources\app\lib\backend\resources\arduino-cli.exe"
set PORT=%1
if "%PORT%"=="" set PORT=COM10

echo [*] Compilando para ESP32 Dev Module (FQBN: esp32:esp32:esp32)...
%ARDUINO_CLI% compile --fqbn esp32:esp32:esp32 --output-dir ".\build_esp32" .
if %ERRORLEVEL% neq 0 (
    echo [X] Erro de compilacao!
    exit /b %ERRORLEVEL%
)

echo [*] Fazendo upload para %PORT%...
%ARDUINO_CLI% upload -p %PORT% --fqbn esp32:esp32:esp32 --input-dir ".\build_esp32"
if %ERRORLEVEL% equ 0 (
    echo [V] Upload concluido com sucesso no ESP32!
) else (
    echo [X] Falha no upload para %PORT%!
)
