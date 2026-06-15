/*
 * teste_postar.c
 * Testes unitarios - Modulo Postar (Foodies)
 *
 * Cobre: verificaLogin, enterPostar
 * Casos de teste conforme PDF 5.1 e 5.7
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

/* ---------------------------------------------------------------
 * HELPER
 *   Aloca AppDados no heap sem sessao ativa.
 * --------------------------------------------------------------- */
static AppDados *alocarBanco(void) {
    AppDados *db = malloc(sizeof(AppDados));
    assert(db != NULL);
    memset(db, 0, sizeof(AppDados));
    db->proximoIdPrato = 1;
    db->proximoIdAval  = 1;
    db->cpfLogado      = 0;
    return db;
}

/* ---------------------------------------------------------------
 * SUITE 1 - verificaLogin
 * Ref. PDF 5.7
 *
 * Retornos esperados:
 *   0 - Esta logado            (LOGIN_OK)
 *   1 - Nao logado / outro CPF (LOGIN_OUTRO_CPF)
 * --------------------------------------------------------------- */
static void suite_verificaLogin(void) {
    printf("\n=== SUITE verificaLogin ===\n");

    AppDados *db = alocarBanco();

    /* C1 - Sem sessao ativa (cpfLogado = 0)
     * Retorno esperado: LOGIN_OUTRO_CPF (1) */
    db->cpfLogado = 0;
    VERIFICAR(
        "C1: verificaLogin sem sessao ativa => LOGIN_OUTRO_CPF",
        verificaLogin(db, 12345678901LL) == LOGIN_OUTRO_CPF
    );

    /* C2 - CPF que esta logado na sessao
     * Retorno esperado: LOGIN_OK (0) */
    db->cpfLogado = 12345678901LL;
    VERIFICAR(
        "C2: verificaLogin com CPF logado => LOGIN_OK",
        verificaLogin(db, 12345678901LL) == LOGIN_OK
    );

    /* C3 - CPF diferente do logado
     * Retorno esperado: LOGIN_OUTRO_CPF (1) */
    VERIFICAR(
        "C3: verificaLogin com CPF diferente do logado => LOGIN_OUTRO_CPF",
        verificaLogin(db, 99999999999LL) == LOGIN_OUTRO_CPF
    );

    /* C4 - db = NULL
     * Retorno esperado: LOGIN_OUTRO_CPF (1) */
    VERIFICAR(
        "C4: verificaLogin com db=NULL => LOGIN_OUTRO_CPF",
        verificaLogin(NULL, 12345678901LL) == LOGIN_OUTRO_CPF
    );

    /* C5 - cpf = 0
     * Retorno esperado: LOGIN_OUTRO_CPF (1) */
    VERIFICAR(
        "C5: verificaLogin com cpf=0 => LOGIN_OUTRO_CPF",
        verificaLogin(db, 0LL) == LOGIN_OUTRO_CPF
    );

    /* C6 - cpf negativo
     * Retorno esperado: LOGIN_OUTRO_CPF (1) */
    VERIFICAR(
        "C6: verificaLogin com cpf negativo => LOGIN_OUTRO_CPF",
        verificaLogin(db, -1LL) == LOGIN_OUTRO_CPF
    );

    free(db);
}

/* ---------------------------------------------------------------
 * SUITE 2 - enterPostar
 * Ref. PDF 5.1
 *
 * Retornos esperados:
 *   0 - Entrou na pagina Postar (POSTAR_OK)
 *   1 - CPF invalido / nao logado (POSTAR_CPF_INVALIDO)
 *   2 - Parametro invalido (POSTAR_PARAM_INVALIDO)
 * --------------------------------------------------------------- */
static void suite_enterPostar(void) {
    printf("\n=== SUITE enterPostar ===\n");

    AppDados *db = alocarBanco();

    /* C1 - CPF valido e logado
     * Retorno esperado: POSTAR_OK (0) */
    db->cpfLogado = 12345678901LL;
    VERIFICAR(
        "C1: enterPostar com CPF valido e logado => POSTAR_OK",
        enterPostar(db, 12345678901LL) == POSTAR_OK
    );

    /* C2 - CPF valido mas sem sessao ativa
     * Retorno esperado: POSTAR_CPF_INVALIDO (1) */
    db->cpfLogado = 0;
    VERIFICAR(
        "C2: enterPostar com CPF valido mas nao logado => POSTAR_CPF_INVALIDO",
        enterPostar(db, 12345678901LL) == POSTAR_CPF_INVALIDO
    );

    /* C2b - CPF diferente do logado
     * Retorno esperado: POSTAR_CPF_INVALIDO (1) */
    db->cpfLogado = 11111111111LL;
    VERIFICAR(
        "C2b: enterPostar com CPF diferente do logado => POSTAR_CPF_INVALIDO",
        enterPostar(db, 99999999999LL) == POSTAR_CPF_INVALIDO
    );

    /* C3a - cpf = 0
     * Retorno esperado: POSTAR_PARAM_INVALIDO (2) */
    db->cpfLogado = 12345678901LL;
    VERIFICAR(
        "C3a: enterPostar com cpf=0 => POSTAR_PARAM_INVALIDO",
        enterPostar(db, 0LL) == POSTAR_PARAM_INVALIDO
    );

    /* C3b - cpf negativo
     * Retorno esperado: POSTAR_PARAM_INVALIDO (2) */
    VERIFICAR(
        "C3b: enterPostar com cpf negativo => POSTAR_PARAM_INVALIDO",
        enterPostar(db, -1LL) == POSTAR_PARAM_INVALIDO
    );

    /* C3c - db = NULL
     * Retorno esperado: POSTAR_PARAM_INVALIDO (2) */
    VERIFICAR(
        "C3c: enterPostar com db=NULL => POSTAR_PARAM_INVALIDO",
        enterPostar(NULL, 12345678901LL) == POSTAR_PARAM_INVALIDO
    );

    free(db);
}

/* ---------------------------------------------------------------
 * MAIN
 * --------------------------------------------------------------- */
int main(void) {
    printf("TESTES - Modulo Postar\n");

    suite_verificaLogin();
    suite_enterPostar();

    printf("  RESULTADO: %d/%d testes passaram\n",
           testesPassou, totalTestes);

    return EXIT_SUCCESS;
}