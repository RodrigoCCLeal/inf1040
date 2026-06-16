/*
 * teste_buscar.c
 * Testes unitarios - Modulo Buscar (Foodies)
 *
 * Cobre: enterBuscar
 * Casos de teste conforme PDF 5.2
 *
 * Os dados de pratos e restaurantes sao carregados dos JSONs fixos
 * (pratos.json e restaurantes.json) via carregarPratos e
 * carregarRestaurantes, exatamente como acontece na execucao real.
 * Nenhum dado e inserido manualmente no AppDados.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../buscar/buscar.h"
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

    AppDados *db = alocarECarregar();

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
 * SUITE 2 - integracao com dados reais dos JSONs
 * Verifica que os JSONs foram carregados corretamente e que as
 * funcoes de busca dos modulos Pratos e Restaurante funcionam
 * com esses dados.
 * --------------------------------------------------------------- */
static void suite_dadosCarregados(void) {
    printf("\n=== SUITE dados carregados dos JSONs ===\n");

    AppDados *db = alocarECarregar();

    /* Verificar que os JSONs foram lidos */
    VERIFICAR(
        "JSON: restaurantes.json carregado (nRestaurantes > 0)",
        db->nRestaurantes > 0
    );
    VERIFICAR(
        "JSON: pratos.json carregado (nPratos > 0)",
        db->nPratos > 0
    );

    /* Busca por nome usando getListaRest do modulo Restaurante */
    Restaurante rests[100];
    int qtdRest = getListaRest(db, "Toca", rests, 100);
    VERIFICAR(
        "BUSCA: getListaRest('Toca') encontra restaurantes do JSON",
        qtdRest > 0
    );

    /* Busca por nome usando getListaPratos do modulo Pratos */
    Prato pratos[100];
    int qtdPrato = getListaPratos(db, "Frango", pratos, 100);
    VERIFICAR(
        "BUSCA: getListaPratos('Frango') encontra pratos do JSON",
        qtdPrato > 0
    );

    /* Busca por nome vazio retorna 0 (PDF 2.4) */
    VERIFICAR(
        "BUSCA: getListaRest com nome vazio => 0",
        getListaRest(db, "", rests, 100) == 0
    );
    VERIFICAR(
        "BUSCA: getListaPratos com nome vazio => 0",
        getListaPratos(db, "", pratos, 100) == 0
    );

    /* Nome inexistente retorna 0 */
    VERIFICAR(
        "BUSCA: getListaRest nome inexistente => 0",
        getListaRest(db, "Giraffas", rests, 100) == 0
    );
    VERIFICAR(
        "BUSCA: getListaPratos nome inexistente => 0",
        getListaPratos(db, "Caviar", pratos, 100) == 0
    );

    free(db);
}

/* ---------------------------------------------------------------
 * MAIN
 * --------------------------------------------------------------- */
int main(void) {
    printf("TESTES - Modulo Buscar\n");

    suite_enterBuscar();
    suite_dadosCarregados();

    printf("  RESULTADO: %d/%d testes passaram\n",
           testesPassou, totalTestes);

    return EXIT_SUCCESS;
}