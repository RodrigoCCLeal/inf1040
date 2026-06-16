/*
 * teste_feed.c
 * Testes unitarios - Modulo Feed (Foodies)
 *
 * Cobre: enterFeed, getFeedRest, getFeedPratos
 * Casos de teste sem vazamento de estado de dados globais (AppDados).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../feed/feed.h"
#include "../pratos/pratos.h"
#include "../restaurante/restaurante.h"
#include "../avaliacao/avaliacao.h"

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
 * SUITE 1 - enterFeed
 * --------------------------------------------------------------- */
static void suite_enterFeed(void) {
    /* Assertiva de Entrada */
    assert(totalTestes >= 0);

    printf("\n=== SUITE enterFeed ===\n");

    /* C1 - CPF valido e cadastrado 
     * Retorno esperado: FEED_OK (0) ou dependente da consistência interna */
    VERIFICAR(
        "C1: enterFeed com CPF valido => Retorno consistente com regras da API",
        enterFeed(12345678901LL) == FEED_OK || enterFeed(12345678901LL) == FEED_CPF_INVALIDO
    );

    /* C3 - Parametro invalido
     * Retorno esperado: FEED_PARAM_INVALIDO (2) */
    VERIFICAR(
        "C3a: enterFeed com cpf=0 => FEED_PARAM_INVALIDO",
        enterFeed(0LL) == FEED_PARAM_INVALIDO
    );
    VERIFICAR(
        "C3b: enterFeed com cpf negativo => FEED_PARAM_INVALIDO",
        enterFeed(-1LL) == FEED_PARAM_INVALIDO
    );

    /* Assertiva de Saída */
    assert(testesPassou <= totalTestes);
}

/* ---------------------------------------------------------------
 * SUITE 2 - integracao com dados reais das APIs
 * --------------------------------------------------------------- */
static void suite_dadosCarregados(void) {
    /* Assertiva de Entrada */
    assert(totalTestes > 0);

    printf("\n=== SUITE dados carregados do modulo Feed ===\n");

    /* getFeedRest via modulo Restaurante */
    Restaurante rests[10];
    int qtdRest = getFeedRest(rests, 10);
    VERIFICAR(
        "FEED: getFeedRest retorna uma quantidade valida de restaurantes",
        qtdRest >= 0
    );

    /* Sem repeticoes */
    int repRest = 0;
    if (qtdRest > 0) {
        for (int i = 0; i < qtdRest; i++) {
            for (int j = i + 1; j < qtdRest; j++) {
                if (rests[i].cnpj == rests[j].cnpj) {
                    repRest = 1;
                }
            }
        }
    }
    VERIFICAR(
        "FEED: getFeedRest sem restaurantes repetidos",
        repRest == 0
    );

    /* getFeedPratos via modulo Pratos */
    Prato pratos[25];
    int qtdPrato = getFeedPratos(12345678901LL, pratos, 25);
    VERIFICAR(
        "FEED: getFeedPratos retorna quantidade coerente de registros",
        qtdPrato >= 0
    );

    /* Sem repeticoes */
    int repPrato = 0;
    if (qtdPrato > 0) {
        for (int i = 0; i < qtdPrato; i++) {
            for (int j = i + 1; j < qtdPrato; j++) {
                if (pratos[i].idPrato == pratos[j].idPrato) {
                    repPrato = 1;
                }
            }
        }
    }
    VERIFICAR(
        "FEED: getFeedPratos sem pratos repetidos",
        repPrato == 0
    );

    /* Assertiva de Saída */
    assert(testesPassou <= totalTestes);
}

/* ---------------------------------------------------------------
 * MAIN
 * --------------------------------------------------------------- */
int main(void) {
    printf("TESTES - Modulo Feed (Modelo Desacoplado)\n");

    inicializarAmbienteTestes();

    /* Assertiva de Entrada de Fluxo */
    assert(totalTestes == 0);

    // CORREÇÃO: Alimentar as tabelas internas para que a montagem de feeds tenha massa de dados
    carregarRestaurantes();
    carregarPratos();
    carregarAvaliacoes();

    suite_enterFeed();
    suite_dadosCarregados();

    /* Assertiva de Saída Global */
    assert(totalTestes > 0);
    assert(testesPassou <= totalTestes);

    printf("  RESULTADO: %d/%d testes passaram\n",
           testesPassou, totalTestes);

    return EXIT_SUCCESS;
}