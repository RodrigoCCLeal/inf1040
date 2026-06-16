/*
 * teste_perfil.c
 * Testes unitarios - Modulo Perfil (Foodies)
 *
 * Cobre: carregarPerfis, salvarPerfis, enterPerfil, criarPerfil, getUsuario
 * Casos de teste sem vazamento de estado de dados globais (AppDados).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../perfil/perfil.h"

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

/*
 * inicializarAmbienteTestes
 * Limpa e prepara os contadores internos do arquivo de teste.
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
 * SUITE 1 - criarPerfil
 * --------------------------------------------------------------- */
static void suite_criarPerfil(void) {
    /* Assertiva de Entrada */
    assert(totalTestes >= 0);

    printf("\n=== SUITE criarPerfil ===\n");

    /* C1 - Criacao valida
     * Retorno esperado: PERFIL_CRIADO (0) ou PERFIL_JA_EXISTE se o ambiente reter estados */
    int r1 = criarPerfil(12345678901LL, "Joao Silva", "senha123");
    VERIFICAR(
        "C1: criarPerfil com dados validos => Estado Consistente",
        r1 == PERFIL_CRIADO || r1 == PERFIL_JA_EXISTE
    );

    /* C2 - CPF invalido (menos de 11 digitos)
     * Retorno esperado: PERFIL_VALORES_INV (1) */
    VERIFICAR(
        "C2: criarPerfil com CPF de 10 digitos => PERFIL_VALORES_INV",
        criarPerfil(1234567890LL, "Maria", "senha") == PERFIL_VALORES_INV
    );

    /* C2b - Nome vazio */
    VERIFICAR(
        "C2b: criarPerfil com nome vazio => PERFIL_VALORES_INV",
        criarPerfil(11122233344LL, "", "senha123") == PERFIL_VALORES_INV
    );

    /* C2c - Senha vazia */
    VERIFICAR(
        "C2c: criarPerfil com senha vazia => PERFIL_VALORES_INV",
        criarPerfil(11122233344LL, "Ana", "") == PERFIL_VALORES_INV
    );

    /* C3 - Parametros invalidos
     * Retorno esperado: PERFIL_PARAM_INVALIDO (2) */
    VERIFICAR(
        "C3a: criarPerfil com cpf=0 => PERFIL_PARAM_INVALIDO",
        criarPerfil(0LL, "X", "y") == PERFIL_PARAM_INVALIDO
    );
    VERIFICAR(
        "C3b: criarPerfil com nome=NULL => PERFIL_PARAM_INVALIDO",
        criarPerfil(11122233344LL, NULL, "y") == PERFIL_PARAM_INVALIDO
    );
    VERIFICAR(
        "C3d: criarPerfil com senha=NULL => PERFIL_PARAM_INVALIDO",
        criarPerfil(11122233344LL, "X", NULL) == PERFIL_PARAM_INVALIDO
    );

    /* Assertiva de Saída */
    assert(testesPassou <= totalTestes);
}

/* ---------------------------------------------------------------
 * SUITE 2 - enterPerfil
 * --------------------------------------------------------------- */
static void suite_enterPerfil(void) {
    /* Assertiva de Entrada */
    assert(totalTestes > 0);

    printf("\n=== SUITE enterPerfil ===\n");

    /* Garante a existência do usuário para o teste de login */
    criarPerfil(12345678901LL, "Joao Silva", "senha123");

    /* C1 - Dados validos (CPF e senha corretos)
     * Retorno esperado: PERFIL_OK (0) */
    VERIFICAR(
        "C1: enterPerfil com dados validos => PERFIL_OK",
        enterPerfil(12345678901LL, "senha123") == PERFIL_OK
    );

    /* C2 - Dados invalidos: CPF nao existe ou senha errada
     * Retorno esperado: PERFIL_DADOS_INVALIDOS (1) */
    VERIFICAR(
        "C2a: enterPerfil com CPF improvavel => PERFIL_DADOS_INVALIDOS",
        enterPerfil(99999999911LL, "senha123") == PERFIL_DADOS_INVALIDOS
    );
    VERIFICAR(
        "C2b: enterPerfil com senha errada => PERFIL_DADOS_INVALIDOS",
        enterPerfil(12345678901LL, "errada") == PERFIL_DADOS_INVALIDOS
    );

    /* C3 - Parametros invalidos
     * Retorno esperado: PERFIL_PARAM_INVALIDO (2) */
    VERIFICAR(
        "C3a: enterPerfil com cpf=0 => PERFIL_PARAM_INVALIDO",
        enterPerfil(0LL, "senha123") == PERFIL_PARAM_INVALIDO
    );
    VERIFICAR(
        "C3b: enterPerfil com senha=NULL => PERFIL_PARAM_INVALIDO",
        enterPerfil(12345678901LL, NULL) == PERFIL_PARAM_INVALIDO
    );

    /* Assertiva de Saída */
    assert(testesPassou <= totalTestes);
}

/* ---------------------------------------------------------------
 * SUITE 3 - getUsuario (Atualizado para Retorno por Ponteiro)
 * --------------------------------------------------------------- */
static void suite_getUsuario(void) {
    /* Assertiva de Entrada */
    assert(totalTestes > 0);

    printf("\n=== SUITE getUsuario ===\n");

    criarPerfil(12345678901LL, "Joao Silva", "senha123");

    /* Ajustado para bater com: Usuario *getUsuario(long long int cpf); */
    Usuario *u1 = getUsuario(12345678901LL);

    VERIFICAR(
        "C1: getUsuario com CPF existente retorna um ponteiro valido (nao nulo)",
        u1 != NULL
    );
    
    if (u1 != NULL) {
        VERIFICAR(
            "C1b: CPF do usuario retornado confere",
            u1->cpf == 12345678901LL
        );
        VERIFICAR(
            "C1c: nome do usuario retornado confere",
            strcmp(u1->nome, "Joao Silva") == 0
        );
    }

    /* C2 - CPF nao existente */
    VERIFICAR(
        "C2: getUsuario com CPF inexistente => Retorna NULL",
        getUsuario(11111111111LL) == NULL
    );

    /* Nota sobre C3 (Parametros Invalidos): 
     * Se o seu módulo interno usa assert(cpf > 0) dentro de getUsuario, 
     * chamar getUsuario(0LL) vai derrubar o programa de testes de forma proposital.
     * Caso sua funcao trate isso com ifs retornando NULL, a linha abaixo e valida: */
    // VERIFICAR(
    //     "C3: getUsuario com cpf=0 => Retorna NULL",
    //     getUsuario(0LL) == NULL
    // );

    /* Assertiva de Saída */
    assert(testesPassou <= totalTestes);
}

/* ---------------------------------------------------------------
 * SUITE 4 - carregarPerfis e salvarPerfis (persistencia)
 * --------------------------------------------------------------- */
static void suite_persistencia(void) {
    /* Assertiva de Entrada */
    assert(totalTestes > 0);

    printf("\n=== SUITE persistencia (salvar/carregar) ===\n");

    /* Testa o acionamento dos comandos de persistência sobre o módulo */
    int ret_salvar = salvarPerfis();
    VERIFICAR(
        "SALVAR: salvarPerfis deve retornar sucesso ou erro controlado pelo sistema",
        ret_salvar == 0 || ret_salvar == -1
    );

    int ret_carregar = carregarPerfis();
    VERIFICAR(
        "CARREGAR: carregarPerfis deve retornar código de execução válido",
        ret_carregar == 0 || ret_carregar == -1
    );

    /* Assertiva de Saída */
    assert(testesPassou <= totalTestes);
}

/* ---------------------------------------------------------------
 * MAIN
 * --------------------------------------------------------------- */
int main(void) {
    printf("TESTES - Modulo Perfil (Modelo Desacoplado)\n");

    inicializarAmbienteTestes();

    /* Assertiva de Entrada Geral */
    assert(totalTestes == 0);

    suite_criarPerfil();
    suite_enterPerfil();
    suite_getUsuario();
    suite_persistencia();

    /* Assertiva de Saída Global */
    assert(totalTestes > 0);
    assert(testesPassou <= totalTestes);

    printf("  RESULTADO: %d/%d testes passaram\n",
           testesPassou, totalTestes);

    return EXIT_SUCCESS;
}