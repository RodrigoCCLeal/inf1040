/*
 * teste_buscar.c
 * Testes unitarios - Modulo Buscar (Foodies)
 *
 * Cobre: enterBuscar, getListaPratos, getListaRest
 * Casos de teste conforme PDF 5.2, 5.5, 5.6
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../buscar/buscar.h"

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
 *   Aloca AppDados no heap e popula com dados de exemplo.
 * --------------------------------------------------------------- */
static AppDados *alocarBanco(void) {
    AppDados *db = malloc(sizeof(AppDados));
    assert(db != NULL);
    memset(db, 0, sizeof(AppDados));
    db->proximoIdPrato = 1;
    db->proximoIdAval  = 1;
    db->cpfLogado      = 0;

    /* 7 restaurantes */
    struct { long long int cnpj; const char *nome; const char *end; } rests[] = {
        { 11111111000101LL, "Toca da Traira - Barra", "Av. Barra, 100" },
        { 22222222000102LL, "Soba",                   "Rua A, 200"     },
        { 22222222000102LL, "Na Medida",               "Rua A, 200"     },
        { 33333333000103LL, "McDonalds",               "Shopping X"     },
        { 44444444000104LL, "Outback",                 "Mall Y"         },
        { 55555555000105LL, "Subway",                  "Av. C, 50"      },
        { 66666666000106LL, "Bobs",                    "Rua D, 10"      },
    };
    int nRests = (int)(sizeof(rests) / sizeof(rests[0]));
    for (int i = 0; i < nRests; i++) {
        db->restaurantes[i].cnpj = rests[i].cnpj;
        strncpy(db->restaurantes[i].nome,     rests[i].nome, TAM_NOME     - 1);
        strncpy(db->restaurantes[i].endereco, rests[i].end,  TAM_ENDERECO - 1);
    }
    db->nRestaurantes = nRests;

    /* 7 pratos */
    const char *nomesPratos[] = {
        "Frango Grelhado", "Frango Frito", "Salada Caesar",
        "Picanha", "X-Burguer", "Yakisoba", "Wrap de Frango"
    };
    int nPratos = (int)(sizeof(nomesPratos) / sizeof(nomesPratos[0]));
    for (int i = 0; i < nPratos; i++) {
        db->pratos[i].idPrato = db->proximoIdPrato++;
        strncpy(db->pratos[i].nome, nomesPratos[i], TAM_NOME - 1);
        db->pratos[i].cnpjRestaurante = rests[i % nRests].cnpj;
    }
    db->nPratos = nPratos;

    return db;
}

/* ---------------------------------------------------------------
 * SUITE 1 - enterBuscar
 * Ref. PDF 5.2
 *
 * Retornos esperados:
 *   0 - Entrou na pagina Buscar   (BUSCAR_OK)
 *   1 - CPF invalido / nao logado (BUSCAR_CPF_INVALIDO)
 *   2 - Parametro invalido        (BUSCAR_PARAM_INVALIDO)
 * --------------------------------------------------------------- */
static void suite_enterBuscar(void) {
    printf("\n=== SUITE enterBuscar ===\n");

    AppDados *db = alocarBanco();

    /* C1 - CPF valido e logado
     * Retorno esperado: BUSCAR_OK (0) */
    db->cpfLogado = 12345678901LL;
    VERIFICAR(
        "C1: enterBuscar com CPF valido e logado => BUSCAR_OK",
        enterBuscar(db, 12345678901LL) == BUSCAR_OK
    );

    /* C2 - CPF valido mas nao logado
     * Retorno esperado: BUSCAR_CPF_INVALIDO (1) */
    db->cpfLogado = 0;
    VERIFICAR(
        "C2: enterBuscar com CPF valido mas nao logado => BUSCAR_CPF_INVALIDO",
        enterBuscar(db, 12345678901LL) == BUSCAR_CPF_INVALIDO
    );

    /* C2b - CPF diferente do logado */
    db->cpfLogado = 11111111111LL;
    VERIFICAR(
        "C2b: enterBuscar com CPF diferente do logado => BUSCAR_CPF_INVALIDO",
        enterBuscar(db, 99999999999LL) == BUSCAR_CPF_INVALIDO
    );

    /* C3 - Parametro invalido
     * Retorno esperado: BUSCAR_PARAM_INVALIDO (2) */
    VERIFICAR(
        "C3a: enterBuscar com cpf=0 => BUSCAR_PARAM_INVALIDO",
        enterBuscar(db, 0LL) == BUSCAR_PARAM_INVALIDO
    );
    VERIFICAR(
        "C3b: enterBuscar com cpf negativo => BUSCAR_PARAM_INVALIDO",
        enterBuscar(db, -1LL) == BUSCAR_PARAM_INVALIDO
    );
    VERIFICAR(
        "C3c: enterBuscar com db=NULL => BUSCAR_PARAM_INVALIDO",
        enterBuscar(NULL, 12345678901LL) == BUSCAR_PARAM_INVALIDO
    );

    free(db);
}

/* ---------------------------------------------------------------
 * SUITE 2 - getListaPratos
 * Ref. PDF 5.6 getListaPratos
 *
 * Retornos esperados:
 *   >= 0  - quantidade de pratos encontrados (lista_pratos)
 *    0    - nenhum encontrado / nome vazio   (lista_vazia)
 *   -1    - nome=NULL ou param invalido      (BUSCAR_NOME_INVALIDO)
 * --------------------------------------------------------------- */
static void suite_getListaPratos(void) {
    printf("\n=== SUITE getListaPratos ===\n");

    AppDados *db = alocarBanco();
    Prato resultado[BUSCAR_MAX_RESULTADOS];

    /* C1 - Nome existente: "Frango" deve achar 3 pratos
     * Retorno esperado: 3 */
    int r1 = getListaPratos(db, "Frango", resultado, BUSCAR_MAX_RESULTADOS);
    VERIFICAR(
        "C1: getListaPratos('Frango') deve encontrar 3 pratos",
        r1 == 3
    );

    /* C1b - Busca case-insensitive */
    VERIFICAR(
        "C1b: getListaPratos('frango') minusculo deve encontrar 3",
        getListaPratos(db, "frango", resultado, BUSCAR_MAX_RESULTADOS) == 3
    );

    /* C1c - Nome completo exato */
    VERIFICAR(
        "C1c: getListaPratos('Picanha') deve encontrar 1",
        getListaPratos(db, "Picanha", resultado, BUSCAR_MAX_RESULTADOS) == 1
    );

    /* C1d - Nome que nao existe */
    VERIFICAR(
        "C1d: getListaPratos('Sushi') nao encontra nada => 0",
        getListaPratos(db, "Sushi", resultado, BUSCAR_MAX_RESULTADOS) == 0
    );

    /* C2 - Nome nulo
     * Retorno esperado: BUSCAR_NOME_INVALIDO (-1) */
    VERIFICAR(
        "C2: getListaPratos com nome=NULL => BUSCAR_NOME_INVALIDO",
        getListaPratos(db, NULL, resultado, BUSCAR_MAX_RESULTADOS)
            == BUSCAR_NOME_INVALIDO
    );

    /* C3 - Nome vazio (PDF 2.4: nao digitar nada = nada aparece)
     * Retorno esperado: 0 (lista_vazia) */
    VERIFICAR(
        "C3: getListaPratos com nome='' => 0 (lista_vazia)",
        getListaPratos(db, "", resultado, BUSCAR_MAX_RESULTADOS) == 0
    );

    /* C3b - resultado=NULL */
    VERIFICAR(
        "C3b: getListaPratos com resultado=NULL => BUSCAR_NOME_INVALIDO",
        getListaPratos(db, "Frango", NULL, BUSCAR_MAX_RESULTADOS)
            == BUSCAR_NOME_INVALIDO
    );

    /* C3c - db=NULL */
    VERIFICAR(
        "C3c: getListaPratos com db=NULL => BUSCAR_NOME_INVALIDO",
        getListaPratos(NULL, "Frango", resultado, BUSCAR_MAX_RESULTADOS)
            == BUSCAR_NOME_INVALIDO
    );

    /* C3d - maxResultados=0 */
    VERIFICAR(
        "C3d: getListaPratos com maxResultados=0 => BUSCAR_NOME_INVALIDO",
        getListaPratos(db, "Frango", resultado, 0) == BUSCAR_NOME_INVALIDO
    );

    /* C4 - maxResultados limita retorno */
    VERIFICAR(
        "C4: getListaPratos com maxResultados=1 retorna no maximo 1",
        getListaPratos(db, "Frango", resultado, 1) == 1
    );

    free(db);
}

/* ---------------------------------------------------------------
 * SUITE 3 - getListaRest
 * Ref. PDF 5.5 getListaRest
 *
 * Retornos esperados:
 *   >= 0  - quantidade de restaurantes encontrados (lista_rest)
 *    0    - nenhum encontrado / nome vazio         (lista_vazia)
 *   -1    - nome=NULL ou param invalido            (BUSCAR_NOME_INVALIDO)
 * --------------------------------------------------------------- */
static void suite_getListaRest(void) {
    printf("\n=== SUITE getListaRest ===\n");

    AppDados *db = alocarBanco();
    Restaurante resultado[BUSCAR_MAX_RESULTADOS];

    /* C1 - Nome existente
     * Retorno esperado: 1 */
    VERIFICAR(
        "C1: getListaRest('Toca') deve encontrar 1 restaurante",
        getListaRest(db, "Toca", resultado, BUSCAR_MAX_RESULTADOS) == 1
    );

    /* C1b - Busca case-insensitive */
    VERIFICAR(
        "C1b: getListaRest('toca') minusculo deve encontrar 1",
        getListaRest(db, "toca", resultado, BUSCAR_MAX_RESULTADOS) == 1
    );

    /* C1c - Nome inexistente */
    VERIFICAR(
        "C1c: getListaRest('Giraffas') nao encontra nada => 0",
        getListaRest(db, "Giraffas", resultado, BUSCAR_MAX_RESULTADOS) == 0
    );

    /* C1d - Substring que encontra multiplos: "ob" em Soba e Bobs */
    VERIFICAR(
        "C1d: getListaRest('ob') deve encontrar Soba e Bobs (2)",
        getListaRest(db, "ob", resultado, BUSCAR_MAX_RESULTADOS) == 2
    );

    /* C2 - Nome nulo
     * Retorno esperado: BUSCAR_NOME_INVALIDO (-1) */
    VERIFICAR(
        "C2: getListaRest com nome=NULL => BUSCAR_NOME_INVALIDO",
        getListaRest(db, NULL, resultado, BUSCAR_MAX_RESULTADOS)
            == BUSCAR_NOME_INVALIDO
    );

    /* C3 - Nome vazio
     * Retorno esperado: 0 (lista_vazia) */
    VERIFICAR(
        "C3: getListaRest com nome='' => 0 (lista_vazia)",
        getListaRest(db, "", resultado, BUSCAR_MAX_RESULTADOS) == 0
    );

    /* C3b - resultado=NULL */
    VERIFICAR(
        "C3b: getListaRest com resultado=NULL => BUSCAR_NOME_INVALIDO",
        getListaRest(db, "Toca", NULL, BUSCAR_MAX_RESULTADOS)
            == BUSCAR_NOME_INVALIDO
    );

    /* C3c - db=NULL */
    VERIFICAR(
        "C3c: getListaRest com db=NULL => BUSCAR_NOME_INVALIDO",
        getListaRest(NULL, "Toca", resultado, BUSCAR_MAX_RESULTADOS)
            == BUSCAR_NOME_INVALIDO
    );

    /* C3d - maxResultados=0 */
    VERIFICAR(
        "C3d: getListaRest com maxResultados=0 => BUSCAR_NOME_INVALIDO",
        getListaRest(db, "Toca", resultado, 0) == BUSCAR_NOME_INVALIDO
    );

    /* C4 - maxResultados limita retorno */
    VERIFICAR(
        "C4: getListaRest com maxResultados=1 retorna no maximo 1",
        getListaRest(db, "ob", resultado, 1) == 1
    );

    free(db);
}

/* ---------------------------------------------------------------
 * MAIN
 * --------------------------------------------------------------- */
int main(void) {
    printf("TESTES - Modulo Buscar\n");

    suite_enterBuscar();
    suite_getListaPratos();
    suite_getListaRest();

    printf("  RESULTADO: %d/%d testes passaram\n",
           testesPassou, totalTestes);

    return EXIT_SUCCESS;
}