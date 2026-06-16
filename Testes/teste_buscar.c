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
 * MAIN
 * --------------------------------------------------------------- */
int main(void) {
    printf("TESTES - Modulo Buscar\n");

    suite_enterBuscar();

    printf("  RESULTADO: %d/%d testes passaram\n",
           testesPassou, totalTestes);

    return EXIT_SUCCESS;
}