/*
 * teste_feed.c
 * Testes unitarios - Modulo Feed (Foodies)
 *
 * Cobre: enterFeed, getFeedPratos, getFeedRest
 * Casos de teste conforme PDF 5.3, 5.5, 5.6
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../feed/feed.h"

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
 * HELPERS
 * --------------------------------------------------------------- */

/*
 * alocarBanco
 *   Aloca AppDados no heap para evitar stack overflow no Windows.
 */
static AppDados *alocarBanco(void) {
    AppDados *db = malloc(sizeof(AppDados));
    assert(db != NULL);
    memset(db, 0, sizeof(AppDados));
    db->proximoIdPrato = 1;
    db->proximoIdAval  = 1;
    db->cpfLogado      = 0;
    return db;
}

/*
 * popularGrande
 *   Insere 25 pratos e 8 restaurantes - suficiente para o feed.
 */
static void popularGrande(AppDados *db) {
    for (int i = 0; i < 8; i++) {
        db->restaurantes[i].cnpj = 10000000000100LL + i;
        snprintf(db->restaurantes[i].nome,     TAM_NOME,
                 "Restaurante %d", i + 1);
        snprintf(db->restaurantes[i].endereco, TAM_ENDERECO,
                 "Rua %d, %d", i + 1, (i + 1) * 10);
    }
    db->nRestaurantes = 8;

    for (int i = 0; i < 25; i++) {
        db->pratos[i].idPrato = db->proximoIdPrato++;
        snprintf(db->pratos[i].nome, TAM_NOME, "Prato %d", i + 1);
        db->pratos[i].cnpjRestaurante = db->restaurantes[i % 8].cnpj;
    }
    db->nPratos = 25;
}


/* ---------------------------------------------------------------
 * SUITE 1 - enterFeed
 * Ref. PDF 5.3
 *
 * Retornos esperados:
 *   0 - Entrou na pagina Feed   (FEED_OK)
 *   1 - CPF invalido / nao logado (FEED_CPF_INVALIDO)
 *   2 - Parametro invalido      (FEED_PARAM_INVALIDO)
 * --------------------------------------------------------------- */

static void suite_enterFeed(void) {
    printf("\n=== SUITE enterFeed ===\n");

    AppDados *db = alocarBanco();
    popularGrande(db);

    /* C1 - CPF valido e logado
     * Retorno esperado: FEED_OK (0) */
    db->cpfLogado = 12345678901LL;
    VERIFICAR(
        "C1: enterFeed com CPF valido e logado => FEED_OK",
        enterFeed(db, 12345678901LL) == FEED_OK
    );

    /* C2 - CPF valido mas nao logado
     * Retorno esperado: FEED_CPF_INVALIDO (1) */
    db->cpfLogado = 0;
    VERIFICAR(
        "C2: enterFeed com CPF valido mas nao logado => FEED_CPF_INVALIDO",
        enterFeed(db, 12345678901LL) == FEED_CPF_INVALIDO
    );

    /* C2b - CPF diferente do logado */
    db->cpfLogado = 11111111111LL;
    VERIFICAR(
        "C2b: enterFeed com CPF diferente do logado => FEED_CPF_INVALIDO",
        enterFeed(db, 99999999999LL) == FEED_CPF_INVALIDO
    );

    /* C3 - Parametro invalido
     * Retorno esperado: FEED_PARAM_INVALIDO (2) */
    VERIFICAR(
        "C3a: enterFeed com cpf=0 => FEED_PARAM_INVALIDO",
        enterFeed(db, 0LL) == FEED_PARAM_INVALIDO
    );
    VERIFICAR(
        "C3b: enterFeed com cpf negativo => FEED_PARAM_INVALIDO",
        enterFeed(db, -1LL) == FEED_PARAM_INVALIDO
    );
    VERIFICAR(
        "C3c: enterFeed com db=NULL => FEED_PARAM_INVALIDO",
        enterFeed(NULL, 12345678901LL) == FEED_PARAM_INVALIDO
    );

    free(db);
}

/* ---------------------------------------------------------------
 * MAIN
 * --------------------------------------------------------------- */
int main(void) {
    printf("TESTES - Modulo Feed\n");

    suite_enterFeed();

    printf("  RESULTADO: %d/%d testes passaram\n",
           testesPassou, totalTestes);

    return EXIT_SUCCESS;
}