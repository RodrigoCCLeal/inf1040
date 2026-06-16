/*
 * teste_feed.c
 * Testes unitarios - Modulo Feed (Foodies)
 *
 * Cobre: enterFeed
 * Casos de teste conforme PDF 5.3
 *
 * Os dados de pratos e restaurantes sao carregados dos JSONs fixos
 * (pratos.json e restaurantes.json) via carregarPratos e
 * carregarRestaurantes, exatamente como acontece na execucao real.
 * Nenhum dado e inserido manualmente no AppDados.
 *
 * As funcoes getFeedPratos e getFeedRest pertencem aos modulos
 * Pratos e Restaurante respectivamente e sao testadas la.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../feed/feed.h"
#include "../pratos/pratos.h"
#include "../restaurante/restaurante.h"

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
 * alocarECarregar
 *   Aloca AppDados no heap e carrega os JSONs fixos de pratos e
 *   restaurantes, simulando o comportamento real do iniciarApp().
 */
static AppDados *alocarECarregar(void) {
    AppDados *db = malloc(sizeof(AppDados));
    assert(db != NULL);
    memset(db, 0, sizeof(AppDados));
    db->proximoIdPrato = 1;
    db->proximoIdAval  = 1;
    db->cpfLogado      = 0;

    /* Carregar dados fixos dos JSONs, como faz o Principal */
    carregarRestaurantes(db);
    carregarPratos(db);

    return db;
}

/* ---------------------------------------------------------------
 * SUITE 1 - enterFeed
 * Ref. PDF 5.3
 *
 * Retornos esperados:
 *   0 - Entrou na pagina Feed     (FEED_OK)
 *   1 - CPF invalido / nao logado (FEED_CPF_INVALIDO)
 *   2 - Parametro invalido        (FEED_PARAM_INVALIDO)
 * --------------------------------------------------------------- */
static void suite_enterFeed(void) {
    printf("\n=== SUITE enterFeed ===\n");

    AppDados *db = alocarECarregar();

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
 * SUITE 2 - integracao com dados reais dos JSONs
 * Verifica que getFeedPratos e getFeedRest dos modulos Pratos e
 * Restaurante funcionam com os dados carregados dos JSONs fixos.
 * --------------------------------------------------------------- */
static void suite_dadosCarregados(void) {
    printf("\n=== SUITE dados carregados dos JSONs ===\n");

    AppDados *db = alocarECarregar();

    /* Verificar que os JSONs foram lidos */
    VERIFICAR(
        "JSON: restaurantes.json carregado (nRestaurantes >= 6)",
        db->nRestaurantes >= 6
    );
    VERIFICAR(
        "JSON: pratos.json carregado (nPratos >= 20)",
        db->nPratos >= 20
    );

    /* getFeedRest via modulo Restaurante */
    Restaurante rests[10];
    int qtdRest = getFeedRest(db, rests, 10);
    VERIFICAR(
        "FEED: getFeedRest retorna 6 restaurantes do JSON",
        qtdRest == 6
    );

    /* Sem repeticoes */
    int repRest = 0;
    for (int i = 0; i < qtdRest; i++)
        for (int j = i + 1; j < qtdRest; j++)
            if (rests[i].cnpj == rests[j].cnpj) repRest = 1;
    VERIFICAR(
        "FEED: getFeedRest sem restaurantes repetidos",
        repRest == 0
    );

    /* getFeedPratos via modulo Pratos */
    Prato pratos[25];
    int qtdPrato = getFeedPratos(db, 12345678901LL, pratos, 25);
    VERIFICAR(
        "FEED: getFeedPratos retorna 20 pratos do JSON",
        qtdPrato == PRATOS_FEED_QTD
    );

    /* Sem repeticoes */
    int repPrato = 0;
    for (int i = 0; i < qtdPrato; i++)
        for (int j = i + 1; j < qtdPrato; j++)
            if (pratos[i].idPrato == pratos[j].idPrato) repPrato = 1;
    VERIFICAR(
        "FEED: getFeedPratos sem pratos repetidos",
        repPrato == 0
    );

    /* Pratos avaliados nao aparecem no feed (PDF 2.7) */
    db->avaliacoes[0].idAval  = 1;
    db->avaliacoes[0].cpf     = 12345678901LL;
    db->avaliacoes[0].idPrato = 1;
    db->avaliacoes[0].nota    = 5.0f;
    db->nAvaliacoes = 1;

    Prato pratosFiltrados[25];
    int qtdFiltrado = getFeedPratos(db, 12345678901LL, pratosFiltrados, 25);
    int contemAvaliado = 0;
    for (int i = 0; i < qtdFiltrado; i++)
        if (pratosFiltrados[i].idPrato == 1) contemAvaliado = 1;
    VERIFICAR(
        "FEED: prato ja avaliado nao aparece no getFeedPratos",
        contemAvaliado == 0
    );

    free(db);
}

/* ---------------------------------------------------------------
 * MAIN
 * --------------------------------------------------------------- */
int main(void) {
    printf("TESTES - Modulo Feed\n");

    suite_enterFeed();
    suite_dadosCarregados();

    printf("  RESULTADO: %d/%d testes passaram\n",
           testesPassou, totalTestes);

    return EXIT_SUCCESS;
}