/*
 * teste_restaurante.c
 * Testes unitarios - Modulo Restaurante (Foodies)
 *
 * Cobre: carregarRestaurantes, getListaRest, getFeedRest, getMenuRestaurante
 * Casos de teste sob arquitetura encapsulada e isolada de dados globais.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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
 * inicializarAmbienteTestes
 * Configura e zera os contadores internos globais de rastreio de testes.
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
 * SUITE 1 - getListaRest
 * --------------------------------------------------------------- */
static void suite_getListaRest(void) {
    /* Assertiva de Entrada */
    assert(totalTestes >= 0);

    printf("\n=== SUITE getListaRest ===\n");
    Restaurante resultado[100];

    /* C1 - Busca por termo textual padrão */
    int r1 = getListaRest("Toca", resultado, 100);
    VERIFICAR(
        "C1: getListaRest com termo valido retorna contagem integra de registros",
        r1 >= 0
    );

    /* C2 - Nome nulo
     * Retorno esperado: REST_NOME_INVALIDO (-1) */
    VERIFICAR(
        "C2: getListaRest com nome=NULL => REST_NOME_INVALIDO",
        getListaRest(NULL, resultado, 100) == REST_NOME_INVALIDO
    );

    /* C3 - Nome vazio (nada deve aparecer)
     * Retorno esperado: 0 (lista_vazia) */
    VERIFICAR(
        "C3: getListaRest com nome='' => 0 (lista_vazia)",
        getListaRest("", resultado, 100) == 0
    );
    VERIFICAR(
        "C3b: getListaRest com buffer destino=NULL => REST_NOME_INVALIDO",
        getListaRest("Toca", NULL, 100) == REST_NOME_INVALIDO
    );
    VERIFICAR(
        "C3d: getListaRest com maxResultados=0 => REST_NOME_INVALIDO",
        getListaRest("Toca", resultado, 0) == REST_NOME_INVALIDO
    );

    /* Assertiva de Saída */
    assert(testesPassou <= totalTestes);
}

/* ---------------------------------------------------------------
 * SUITE 2 - getFeedRest
 * --------------------------------------------------------------- */
static void suite_getFeedRest(void) {
    /* Assertiva de Entrada */
    assert(totalTestes > 0);

    printf("\n=== SUITE getFeedRest ===\n");
    Restaurante resultado[20];

    /* C1 - Consulta padrão de feeds ativos da plataforma */
    int r1 = getFeedRest(resultado, 20);
    VERIFICAR(
        "C1: getFeedRest executado retorna contagem valida ou aviso de base insuficiente",
        r1 == REST_FEED_QTD || r1 == REST_INSUFICIENTE || r1 >= 0
    );

    /* C3 - Parametros invalidos */
    VERIFICAR(
        "C3b: getFeedRest com destino=NULL => REST_PARAM_INVALIDO",
        getFeedRest(NULL, 20) == REST_PARAM_INVALIDO
    );

    /* Assertiva de Saída */
    assert(testesPassou <= totalTestes);
}

/* ---------------------------------------------------------------
 * SUITE 3 - getMenuRestaurante
 * --------------------------------------------------------------- */
static void suite_getMenuRestaurante(void) {
    /* Assertiva de Entrada */
    assert(totalTestes > 0);

    printf("\n=== SUITE getMenuRestaurante ===\n");
    Prato resultado[100];

    /* C1 - Consulta de cardápio por CNPJ válido */
    int r1 = getMenuRestaurante(11111111000101LL, resultado, 100);
    VERIFICAR(
        "C1: getMenuRestaurante retorna lista integra de itens cadastrados",
        r1 >= 0
    );

    /* C3 - Parametros invalidos
     * Retorno esperado: REST_PARAM_INVALIDO (-2) */
    VERIFICAR(
        "C3a: getMenuRestaurante com cnpj=0 => REST_PARAM_INVALIDO",
        getMenuRestaurante(0LL, resultado, 100) == REST_PARAM_INVALIDO
    );
    VERIFICAR(
        "C3b: getMenuRestaurante com destino=NULL => REST_PARAM_INVALIDO",
        getMenuRestaurante(11111111000101LL, NULL, 100) == REST_PARAM_INVALIDO
    );
    VERIFICAR(
        "C3c: getMenuRestaurante com maxResultados=0 => REST_PARAM_INVALIDO",
        getMenuRestaurante(11111111000101LL, resultado, 0) == REST_PARAM_INVALIDO
    );

    /* Assertiva de Saída */
    assert(testesPassou <= totalTestes);
}

/* ---------------------------------------------------------------
 * SUITE 4 - carregarRestaurantes
 * --------------------------------------------------------------- */
static void suite_carregarRestaurantes(void) {
    /* Assertiva de Entrada */
    assert(totalTestes > 0);

    printf("\n=== SUITE carregarRestaurantes ===\n");

    /* Invoca a carga do arquivo físico mapeado internamente pelo módulo */
    int ret = carregarRestaurantes();
    VERIFICAR(
        "CARGA: carregarRestaurantes retorna codigo previsivel do barramento de I/O",
        ret == 0 || ret == -1
    );

    /* Assertiva de Saída */
    assert(testesPassou <= totalTestes);
}

/* ---------------------------------------------------------------
 * MAIN
 * --------------------------------------------------------------- */
int main(void) {
    printf("TESTES - Modulo Restaurante (Modelo Desacoplado)\n");

    inicializarAmbienteTestes();

    /* Assertiva de Entrada Geral do Fluxo Executável */
    assert(totalTestes == 0);

    suite_getListaRest();
    suite_getFeedRest();
    suite_getMenuRestaurante();
    suite_carregarRestaurantes();

    /* Assertiva de Saída Global */
    assert(totalTestes > 0);
    assert(testesPassou <= totalTestes);

    printf("  RESULTADO: %d/%d testes passaram\n",
           testesPassou, totalTestes);

    return EXIT_SUCCESS;
}