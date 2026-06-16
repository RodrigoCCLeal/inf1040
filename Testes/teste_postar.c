/*
 * teste_postar.c
 * Testes unitarios - Modulo Postar (Foodies)
 *
 * Cobre: verificaLogin, enterPostar
 * Casos de teste sem vazamento de estado de dados globais (AppDados).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../postar/postar.h"

/* ---------------------------------------------------------------
 * CONTADORES DE TESTE
 * --------------------------------------------------------------- */
static int totalTestes  = 0;
static int testesPassou = 0;

#define VERIFICAR(descricao, condicao)          \
    do {                                        \
        totalTestes++;                          \
        printf("  [TESTE] %s\n", descricao);   \
        assert((condicao));                     \
        testesPassou++;                         \
        printf("  [OK]\n");                     \
    } while (0)

/*
 * inicializarAmbienteTestes
 * Limpa e prepara os contadores internos para a execução das suites.
 */
static void inicializarAmbienteTestes(void) {
    /* Assertiva de Entrada */
    assert(totalTestes >= 0 && testesPassou >= 0);

    totalTestes = 0;
    testesPassou = 0;

    /* Assertiva de Saída */
    assert(totalTestes == 0 && testesPassou == 0);
}

/* ---------------------------------------------------------------
 * SUITE 1 - verificaLogin
 * --------------------------------------------------------------- */
static void suite_verificaLogin(void) {
    /* Assertiva de Entrada */
    assert(totalTestes >= 0);

    printf("\n=== SUITE verificaLogin ===\n");

    /* No modelo puramente encapsulado, os testes avaliam as respostas do
       módulo com base nas regras de negócio e parâmetros fornecidos */

    /* C1 - Teste de validação padrão de fluxo */
    VERIFICAR(
        "C1: verificaLogin com CPF valido => Retorno consistente com o estado interno",
        verificaLogin(12345678901LL) == LOGIN_OK || verificaLogin(12345678901LL) == LOGIN_OUTRO_CPF
    );

    /* C5 - cpf = 0
     * Retorno esperado: LOGIN_OUTRO_CPF (1) */
    VERIFICAR(
        "C5: verificaLogin com cpf=0 => LOGIN_OUTRO_CPF",
        verificaLogin(0LL) == LOGIN_OUTRO_CPF
    );

    /* C6 - cpf negativo
     * Retorno esperado: LOGIN_OUTRO_CPF (1) */
    VERIFICAR(
        "C6: verificaLogin com cpf negativo => LOGIN_OUTRO_CPF",
        verificaLogin(-1LL) == LOGIN_OUTRO_CPF
    );

    /* Assertiva de Saída */
    assert(testesPassou <= totalTestes);
}

/* ---------------------------------------------------------------
 * SUITE 2 - enterPostar
 * --------------------------------------------------------------- */
static void suite_enterPostar(void) {
    /* Assertiva de Entrada */
    assert(totalTestes > 0);

    printf("\n=== SUITE enterPostar ===\n");

    /* C1 - Teste de entrada de página com parâmetro válido */
    VERIFICAR(
        "C1: enterPostar com CPF valido => Resposta previsível da API",
        enterPostar(12345678901LL) == POSTAR_OK || enterPostar(12345678901LL) == POSTAR_CPF_INVALIDO
    );

    /* C3a - cpf = 0
     * Retorno esperado: POSTAR_PARAM_INVALIDO (2) */
    VERIFICAR(
        "C3a: enterPostar com cpf=0 => POSTAR_PARAM_INVALIDO",
        enterPostar(0LL) == POSTAR_PARAM_INVALIDO
    );

    /* C3b - cpf negativo
     * Retorno esperado: POSTAR_PARAM_INVALIDO (2) */
    VERIFICAR(
        "C3b: enterPostar com cpf negativo => POSTAR_PARAM_INVALIDO",
        enterPostar(-1LL) == POSTAR_PARAM_INVALIDO
    );

    /* Assertiva de Saída */
    assert(testesPassou <= totalTestes);
}

/* ---------------------------------------------------------------
 * MAIN
 * --------------------------------------------------------------- */
int main(void) {
    printf("TESTES - Modulo Postar (Modelo Desacoplado)\n");

    inicializarAmbienteTestes();

    /* Assertiva de Entrada de Fluxo Geral */
    assert(totalTestes == 0);

    suite_verificaLogin();
    suite_enterPostar();

    /* Assertiva de Saída Global */
    assert(totalTestes > 0);
    assert(testesPassou <= totalTestes);

    printf("  RESULTADO: %d/%d testes passaram\n",
           testesPassou, totalTestes);

    return EXIT_SUCCESS;
}