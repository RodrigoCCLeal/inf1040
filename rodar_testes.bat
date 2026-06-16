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
testes\teste_avaliacao.exe
if %ERRORLEVEL% == 0 (set /a PASSOU+=1) else (set /a FALHOU+=1)

REM --- teste_postar ---
echo.
echo --- Modulo Postar ---
testes\teste_postar.exe
if %ERRORLEVEL% == 0 (set /a PASSOU+=1) else (set /a FALHOU+=1)

REM --- teste_buscar ---
echo.
echo --- Modulo Buscar ---
testes\teste_buscar.exe
if %ERRORLEVEL% == 0 (set /a PASSOU+=1) else (set /a FALHOU+=1)

REM --- teste_feed ---
echo.
echo --- Modulo Feed ---
testes\teste_feed.exe
if %ERRORLEVEL% == 0 (set /a PASSOU+=1) else (set /a FALHOU+=1)

REM --- teste_perfil ---
echo.
echo --- Modulo Perfil ---
testes\teste_perfil.exe
if %ERRORLEVEL% == 0 (set /a PASSOU+=1) else (set /a FALHOU+=1)

REM --- teste_principal ---
echo.
echo --- Modulo Principal ---
testes\teste_principal.exe
if %ERRORLEVEL% == 0 (set /a PASSOU+=1) else (set /a FALHOU+=1)

REM --- teste_restaurante ---
echo.
echo --- Modulo Restaurante ---
testes\teste_restaurante.exe
if %ERRORLEVEL% == 0 (set /a PASSOU+=1) else (set /a FALHOU+=1)

REM --- teste_pratos ---
echo.
echo --- Modulo Pratos ---
testes\teste_pratos.exe
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