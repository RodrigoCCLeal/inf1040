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

/*
 * popularPequeno
 *   Insere 3 pratos e 2 restaurantes - insuficiente para o feed.
 */
static void popularPequeno(AppDados *db) {
    for (int i = 0; i < 2; i++) {
        db->restaurantes[i].cnpj = 10000000000100LL + i;
        snprintf(db->restaurantes[i].nome, TAM_NOME, "Rest %d", i + 1);
    }
    db->nRestaurantes = 2;

    for (int i = 0; i < 3; i++) {
        db->pratos[i].idPrato = db->proximoIdPrato++;
        snprintf(db->pratos[i].nome, TAM_NOME, "Prato %d", i + 1);
        db->pratos[i].cnpjRestaurante = db->restaurantes[i % 2].cnpj;
    }
    db->nPratos = 3;
}

/*
 * temRepetidosPrato
 *   Retorna 1 se houver pratos com idPrato duplicado no array.
 */
static int temRepetidosPrato(Prato *arr, int n) {
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (arr[i].idPrato == arr[j].idPrato) return 1;
    return 0;
}

/*
 * temRepetidosRest
 *   Retorna 1 se houver restaurantes com CNPJ duplicado no array.
 */
static int temRepetidosRest(Restaurante *arr, int n) {
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (arr[i].cnpj == arr[j].cnpj) return 1;
    return 0;
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
 * SUITE 2 - getFeedPratos
 * Ref. PDF 5.6 getFeedPratos
 *
 * Retornos esperados:
 *   20  - banco suficiente sem avaliacoes  (lista_pratos)
 *  -1   - menos de 20 pratos elegiveis    (FEED_INSUFICIENTE)
 *  -2   - parametro invalido              (FEED_ERRO_PARAM)
 * --------------------------------------------------------------- */
static void suite_getFeedPratos(void) {
    printf("\n=== SUITE getFeedPratos ===\n");

    Prato resultado[50];
    long long int cpfTeste = 12345678901LL;

    /* C1 - Banco com 25 pratos, usuario sem avaliacoes
     * Retorno esperado: 20 */
    AppDados *db = alocarBanco();
    popularGrande(db);

    int r1 = getFeedPratos(db, cpfTeste, resultado, 50);
    VERIFICAR(
        "C1: getFeedPratos com banco suficiente deve retornar 20",
        r1 == FEED_QTD_PRATOS
    );
    VERIFICAR(
        "C1: resultado nao deve conter pratos repetidos",
        temRepetidosPrato(resultado, r1) == 0
    );

    /* C1b - Pratos ja avaliados nao devem aparecer (PDF 2.7)
     * Marcar pratos 1-5 como avaliados: 25 - 5 = 20 elegiveis */
    for (int i = 0; i < 5; i++) {
        db->avaliacoes[i].idAval  = db->proximoIdAval++;
        db->avaliacoes[i].cpf     = cpfTeste;
        db->avaliacoes[i].idPrato = i + 1;
        db->avaliacoes[i].nota    = 4.0f;
    }
    db->nAvaliacoes = 5;

    int r1b = getFeedPratos(db, cpfTeste, resultado, 50);
    VERIFICAR(
        "C1b: com 5 avaliados e 25 total, retorna 20 elegiveis",
        r1b == FEED_QTD_PRATOS
    );

    int contemAvaliado = 0;
    for (int i = 0; i < r1b; i++) {
        if (resultado[i].idPrato >= 1 && resultado[i].idPrato <= 5)
            contemAvaliado = 1;
    }
    VERIFICAR(
        "C1b: feed nao deve conter pratos ja avaliados pelo usuario",
        contemAvaliado == 0
    );

    free(db);

    /* C2 - Banco insuficiente (3 pratos)
     * Retorno esperado: FEED_INSUFICIENTE (-1) */
    AppDados *dbPeq = alocarBanco();
    popularPequeno(dbPeq);
    VERIFICAR(
        "C2: getFeedPratos com 3 pratos => FEED_INSUFICIENTE",
        getFeedPratos(dbPeq, cpfTeste, resultado, 50) == FEED_INSUFICIENTE
    );
    free(dbPeq);

    /* C2b - Todos os pratos ja avaliados (0 elegiveis) */
    AppDados *dbTudo = alocarBanco();
    popularGrande(dbTudo);
    for (int i = 0; i < 25; i++) {
        dbTudo->avaliacoes[i].idAval  = i + 1;
        dbTudo->avaliacoes[i].cpf     = cpfTeste;
        dbTudo->avaliacoes[i].idPrato = i + 1;
        dbTudo->avaliacoes[i].nota    = 5.0f;
    }
    dbTudo->nAvaliacoes   = 25;
    dbTudo->proximoIdAval = 26;
    VERIFICAR(
        "C2b: com todos os 25 pratos avaliados => FEED_INSUFICIENTE",
        getFeedPratos(dbTudo, cpfTeste, resultado, 50) == FEED_INSUFICIENTE
    );
    free(dbTudo);

    /* C3 - Parametros invalidos */
    AppDados *dbV = alocarBanco();
    popularGrande(dbV);
    VERIFICAR(
        "C3a: getFeedPratos com db=NULL => FEED_ERRO_PARAM",
        getFeedPratos(NULL, cpfTeste, resultado, 50) == FEED_ERRO_PARAM
    );
    VERIFICAR(
        "C3b: getFeedPratos com resultado=NULL => FEED_ERRO_PARAM",
        getFeedPratos(dbV, cpfTeste, NULL, 50) == FEED_ERRO_PARAM
    );
    VERIFICAR(
        "C3c: getFeedPratos com cpf=0 => FEED_ERRO_PARAM",
        getFeedPratos(dbV, 0LL, resultado, 50) == FEED_ERRO_PARAM
    );
    free(dbV);
}

/* ---------------------------------------------------------------
 * SUITE 3 - getFeedRest
 * Ref. PDF 5.5 getFeedRest
 *
 * Retornos esperados:
 *    6  - banco com >= 6 restaurantes  (lista_rest)
 *   -1  - menos de 6 restaurantes     (FEED_INSUFICIENTE)
 *   -2  - parametro invalido          (FEED_ERRO_PARAM)
 * --------------------------------------------------------------- */
static void suite_getFeedRest(void) {
    printf("\n=== SUITE getFeedRest ===\n");

    Restaurante resultado[20];

    /* C1 - Banco com 8 restaurantes
     * Retorno esperado: 6 */
    AppDados *dbG = alocarBanco();
    popularGrande(dbG);

    int r1 = getFeedRest(dbG, resultado, 20);
    VERIFICAR(
        "C1: getFeedRest com 8 restaurantes deve retornar 6",
        r1 == FEED_QTD_RESTAURANTES
    );
    VERIFICAR(
        "C1: resultado nao deve conter restaurantes repetidos",
        temRepetidosRest(resultado, r1) == 0
    );

    int r1b = getFeedRest(dbG, resultado, 20);
    VERIFICAR(
        "C1b: segunda chamada tambem retorna 6",
        r1b == FEED_QTD_RESTAURANTES
    );
    VERIFICAR(
        "C1b: segunda chamada sem restaurantes repetidos",
        temRepetidosRest(resultado, r1b) == 0
    );
    free(dbG);

    /* C2 - Banco insuficiente (2 restaurantes)
     * Retorno esperado: FEED_INSUFICIENTE (-1) */
    AppDados *dbP = alocarBanco();
    popularPequeno(dbP);
    VERIFICAR(
        "C2: getFeedRest com 2 restaurantes => FEED_INSUFICIENTE",
        getFeedRest(dbP, resultado, 20) == FEED_INSUFICIENTE
    );
    free(dbP);

    /* C2b - Banco vazio */
    AppDados *dbVazio = alocarBanco();
    VERIFICAR(
        "C2b: getFeedRest com 0 restaurantes => FEED_INSUFICIENTE",
        getFeedRest(dbVazio, resultado, 20) == FEED_INSUFICIENTE
    );
    free(dbVazio);

    /* C2c - Exatamente 6 restaurantes (limite exato) */
    AppDados *dbExato = alocarBanco();
    for (int i = 0; i < 6; i++) {
        dbExato->restaurantes[i].cnpj = 10000000000100LL + i;
        snprintf(dbExato->restaurantes[i].nome, TAM_NOME, "Rest %d", i + 1);
    }
    dbExato->nRestaurantes = 6;
    int r2c = getFeedRest(dbExato, resultado, 20);
    VERIFICAR(
        "C2c: getFeedRest com exatamente 6 restaurantes deve retornar 6",
        r2c == FEED_QTD_RESTAURANTES
    );
    VERIFICAR(
        "C2c: resultado sem repetidos com limite exato",
        temRepetidosRest(resultado, r2c) == 0
    );
    free(dbExato);

    /* C3 - Parametros invalidos */
    AppDados *dbV = alocarBanco();
    popularGrande(dbV);
    VERIFICAR(
        "C3a: getFeedRest com db=NULL => FEED_ERRO_PARAM",
        getFeedRest(NULL, resultado, 20) == FEED_ERRO_PARAM
    );
    VERIFICAR(
        "C3b: getFeedRest com resultado=NULL => FEED_ERRO_PARAM",
        getFeedRest(dbV, NULL, 20) == FEED_ERRO_PARAM
    );
    free(dbV);
}

/* ---------------------------------------------------------------
 * MAIN
 * --------------------------------------------------------------- */
int main(void) {
    printf("TESTES - Modulo Feed\n");

    suite_enterFeed();
    suite_getFeedPratos();
    suite_getFeedRest();

    printf("  RESULTADO: %d/%d testes passaram\n",
           testesPassou, totalTestes);

    return EXIT_SUCCESS;
}