@echo off
set ARDUINO_CLI="C:\Users\chick\AppData\Local\Programs\Arduino IDE\resources\app\lib\backend\resources\arduino-cli.exe"
set PORT=%1

if "%PORT%"=="" (
    echo [!] Porta COM nao especificada. Uso: upload.bat COMx
    echo [i] Listando portas seriais disponiveis...
    %ARDUINO_CLI% board list
    exit /b 1
)

echo [*] Compilando para Arduino UNO...
%ARDUINO_CLI% compile --fqbn arduino:avr:uno --output-dir ".\build" .
if %ERRORLEVEL% neq 0 (
    echo [X] Erro de compilacao!
    exit /b %ERRORLEVEL%
)

echo [*] Fazendo upload para %PORT%...
%ARDUINO_CLI% upload -p %PORT% --fqbn arduino:avr:uno --input-dir ".\build"
if %ERRORLEVEL% equ 0 (
    echo [V] Upload concluido com sucesso!
) else (
    echo [X] Falha no upload para %PORT%!
)
