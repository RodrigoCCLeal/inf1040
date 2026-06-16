/*
 * teste_pratos.c
 * Testes unitarios - Modulo Pratos (Foodies)
 *
 * Cobre: carregarPratos, getPratos, getListaPratos, getMenu, getFeedPratos
 * Casos de teste sem vazamento de estado de dados globais (AppDados).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../pratos/pratos.h"
#include "../perfil/perfil.h"
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
 * SUITE 1 - getPratos
 * --------------------------------------------------------------- */
static void suite_getPratos(void) {
    printf("\n=== SUITE getPratos ===\n");

    // CORREÇÃO: Carregar os pratos do banco JSON para garantir a presenca do ID 1
    carregarPratos();

    /* C1 - Busca por um prato existente */
    Prato *p = getPratos(1);
    VERIFICAR(
        "C1: getPratos com ID valido deve retornar um ponteiro nao nulo",
        p != NULL
    );
    if (p != NULL) {
        VERIFICAR(
            "C1b: O ID do prato retornado deve ser o mesmo do solicitado",
            p->idPrato == 1
        );
    }

    /* C2 - Busca por ID inexistente */
    VERIFICAR(
        "C2: getPratos com ID inexistente deve retornar NULL",
        getPratos(99999) == NULL
    );
}

/* ---------------------------------------------------------------
 * SUITE 2 - getListaPratos
 * --------------------------------------------------------------- */
static void suite_getListaPratos(void) {
    /* Assertiva de Entrada */
    assert(totalTestes > 0);

    printf("\n=== SUITE getListaPratos ===\n");

    Prato resultado[100];

    /* C1 - Busca de padrão textual */
    int r1 = getListaPratos("Frango", resultado, 100);
    VERIFICAR(
        "C1: getListaPratos('Frango') retorna quantidade valida de registros",
        r1 >= 0
    );

    /* C2 - Nome nulo
     * Retorno esperado: PRATOS_NOME_INVALIDO (-1) */
    VERIFICAR(
        "C2: getListaPratos com nome=NULL => PRATOS_NOME_INVALIDO",
        getListaPratos(NULL, resultado, 100) == PRATOS_NOME_INVALIDO
    );

    /* C3 - Nome vazio (PDF 2.4: nada deve aparecer)
     * Retorno esperado: 0 (lista_vazia) */
    VERIFICAR(
        "C3: getListaPratos com nome='' => 0 (lista_vazia)",
        getListaPratos("", resultado, 100) == 0
    );
    VERIFICAR(
        "C3b: getListaPratos com resultado=NULL => PRATOS_NOME_INVALIDO",
        getListaPratos("Frango", NULL, 100) == PRATOS_NOME_INVALIDO
    );
    VERIFICAR(
        "C3d: getListaPratos com maxResultados=0 => PRATOS_NOME_INVALIDO",
        getListaPratos("Frango", resultado, 0) == PRATOS_NOME_INVALIDO
    );

    /* Assertiva de Saída */
    assert(testesPassou <= totalTestes);
}

/* ---------------------------------------------------------------
 * SUITE 3 - getMenu
 * --------------------------------------------------------------- */
static void suite_getMenu(void) {
    /* Assertiva de Entrada */
    assert(totalTestes > 0);

    printf("\n=== SUITE getMenu ===\n");

    Prato resultado[100];

    /* C1 - Consulta por CNPJ */
    int r1 = getMenu(11111111000101LL, resultado, 100);
    VERIFICAR(
        "C1: getMenu com CNPJ valido retorna quantidade coerente de registros",
        r1 >= 0
    );

    /* C2 - CNPJ sem pratos */
    VERIFICAR(
        "C2: getMenu com CNPJ inexistente => 0",
        getMenu(99999999000199LL, resultado, 100) == 0
    );

    /* C3 - Parametros invalidos
     * Retorno esperado: PRATOS_PARAM_INVALIDO (-2) */
    VERIFICAR(
        "C3a: getMenu com cnpj=0 => PRATOS_PARAM_INVALIDO",
        getMenu(0LL, resultado, 100) == PRATOS_PARAM_INVALIDO
    );
    VERIFICAR(
        "C3b: getMenu com resultado=NULL => PRATOS_PARAM_INVALIDO",
        getMenu(11111111000101LL, NULL, 100) == PRATOS_PARAM_INVALIDO
    );
    VERIFICAR(
        "C3c: getMenu com maxResultados=0 => PRATOS_PARAM_INVALIDO",
        getMenu(11111111000101LL, resultado, 0) == PRATOS_PARAM_INVALIDO
    );

    /* Assertiva de Saída */
    assert(testesPassou <= totalTestes);
}

/* ---------------------------------------------------------------
 * SUITE 4 - getFeedPratos
 * --------------------------------------------------------------- */
static void suite_getFeedPratos(void) {
    /* Assertiva de Entrada */
    assert(totalTestes > 0);

    printf("\n=== SUITE getFeedPratos ===\n");

    Prato resultado[50];
    long long int cpfTeste = 12345678901LL;

    int r1 = getFeedPratos(cpfTeste, resultado, 50);
    VERIFICAR(
        "C1: getFeedPratos executado com retorno controlado pelo barramento",
        r1 >= 0 || r1 == PRATOS_INSUFICIENTE
    );

    /* C3 - Parametros invalidos */
    VERIFICAR(
        "C3a: getFeedPratos com resultado=NULL => PRATOS_PARAM_INVALIDO",
        getFeedPratos(cpfTeste, NULL, 50) == PRATOS_PARAM_INVALIDO
    );
    VERIFICAR(
        "C3b: getFeedPratos com cpf=0 => PRATOS_PARAM_INVALIDO",
        getFeedPratos(0LL, resultado, 50) == PRATOS_PARAM_INVALIDO
    );

    /* Assertiva de Saída */
    assert(testesPassou <= totalTestes);
}

/* ---------------------------------------------------------------
 * SUITE 5 - carregarPratos
 * --------------------------------------------------------------- */
static void suite_carregarPratos(void) {
    /* Assertiva de Entrada */
    assert(totalTestes > 0);

    printf("\n=== SUITE carregarPratos ===\n");

    int ret = carregarPratos();
    VERIFICAR(
        "CARGA: carregarPratos executa e retorna codigo de execucao estavel",
        ret == 0 || ret == -1
    );

    /* Assertiva de Saída */
    assert(testesPassou <= totalTestes);
}

/* ---------------------------------------------------------------
 * SUITE EXTRA - Funcoes Utilitarias e Agregacoes
 * Cobre: obterNomeUsuario, obterMediaPrato, imprimirAvaliacoesUsuario
 * --------------------------------------------------------------- */
static void suite_funcoesUtilitarias(void) {
    /* Assertiva de Entrada */
    assert(totalTestes > 0);

    printf("\n=== SUITE Funcoes Utilitarias e Agregacoes ===\n");

    /* 1. Teste de obterNomeUsuario */
    char nomeDestino[TAM_NOME];
    obterNomeUsuario(12345678901LL, nomeDestino);
    
    VERIFICAR(
        "UTIL: obterNomeUsuario retorna uma string valida (nao nula)",
        nomeDestino != NULL && strlen(nomeDestino) > 0
    );

    /* 2. Teste de obterMediaPrato */
    float media = 0.0f;
    // Testando com um ID de prato teoricamente válido
    int qtdAvals = obterMediaPrato(1, &media);
    
    VERIFICAR(
        "UTIL: obterMediaPrato retorna contagem de avaliacoes coerente",
        qtdAvals >= 0
    );
    VERIFICAR(
        "UTIL: obterMediaPrato calcula media dentro do limite correto [0, 5]",
        media >= 0.0f && media <= 5.0f
    );

    /* 3. Teste de parâmetros inválidos para obterMediaPrato */
    VERIFICAR(
        "UTIL: obterMediaPrato com ID invalido => retorna 0 avaliacoes",
        obterMediaPrato(0, &media) == 0
    );

    /* 4. Teste de imprimirAvaliacoesUsuario */
    // Como ela imprime na tela e retorna a quantidade, testamos o retorno
    int impressas = imprimirAvaliacoesUsuario(12345678901LL);
    VERIFICAR(
        "UTIL: imprimirAvaliacoesUsuario executa e retorna contagem valida",
        impressas >= 0
    );

    /* 5. Teste de listarNomesPratos (Apenas garante que roda sem crashar) */
    listarNomesPratos();
    VERIFICAR(
        "UTIL: listarNomesPratos executa com sucesso",
        1 /* Se chegou aqui, não crashou */
    );

    /* Assertiva de Saída */
    assert(testesPassou <= totalTestes);
}

/* ---------------------------------------------------------------
 * MAIN
 * --------------------------------------------------------------- */
int main(void) {
    printf("TESTES - Modulo Pratos (Modelo Desacoplado)\n");

    inicializarAmbienteTestes();

    assert(totalTestes == 0);

    suite_getPratos();
    suite_getListaPratos();
    suite_getMenu();
    suite_getFeedPratos();
    suite_carregarPratos();
    suite_funcoesUtilitarias();

    assert(totalTestes > 0);
    assert(testesPassou <= totalTestes);

    printf("  RESULTADO: %d/%d testes passaram\n",
           testesPassou, totalTestes);

    return EXIT_SUCCESS;
}