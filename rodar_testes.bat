@echo off
REM rodar_testes.bat
REM Executa todos os testes do projeto Foodies em sequencia.
REM Rode este arquivo a partir da raiz do projeto.

setlocal

set PASSOU=0
set FALHOU=0

echo ================================================
echo    FOODIES - Execucao de todos os testes
echo ================================================

REM --- teste_avaliacao ---
echo.
echo --- Modulo Avaliacao ---
Testes\teste_avaliacao
if %ERRORLEVEL% == 0 (set /a PASSOU+=1) else (set /a FALHOU+=1)

REM --- teste_postar ---
echo.
echo --- Modulo Postar ---
Testes\teste_postar
if %ERRORLEVEL% == 0 (set /a PASSOU+=1) else (set /a FALHOU+=1)

REM --- teste_buscar ---
echo.
echo --- Modulo Buscar ---
Testes\teste_buscar
if %ERRORLEVEL% == 0 (set /a PASSOU+=1) else (set /a FALHOU+=1)

REM --- teste_feed ---
echo.
echo --- Modulo Feed ---
Testes\teste_feed
if %ERRORLEVEL% == 0 (set /a PASSOU+=1) else (set /a FALHOU+=1)

REM --- teste_perfil ---
echo.
echo --- Modulo Perfil ---
Testes\teste_perfil
if %ERRORLEVEL% == 0 (set /a PASSOU+=1) else (set /a FALHOU+=1)

REM --- teste_inicializar ---
echo.
echo --- Modulo Inicializar ---
Testes\teste_inicializar
if %ERRORLEVEL% == 0 (set /a PASSOU+=1) else (set /a FALHOU+=1)

REM --- teste_restaurante ---
echo.
echo --- Modulo Restaurante ---
Testes\teste_restaurante
if %ERRORLEVEL% == 0 (set /a PASSOU+=1) else (set /a FALHOU+=1)

REM --- teste_pratos ---
echo.
echo --- Modulo Pratos ---
Testes\teste_pratos
if %ERRORLEVEL% == 0 (set /a PASSOU+=1) else (set /a FALHOU+=1)

REM --- Resumo final ---
echo.
echo ================================================
echo    RESUMO: %PASSOU%/8 modulos passaram
if %FALHOU% GTR 0 (
    echo    ATENCAO: %FALHOU% modulo(s) com falha
)
echo ================================================

endlocal
