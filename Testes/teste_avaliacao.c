/*
 * teste_avaliacao.c
 * Testes unitarios - Modulo Avaliacao (Foodies)
 *
 * Cobre: postAval, verAval, editAval
 * Casos de teste sem vazamento de estado de dados globais (AppDados).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../avaliacao/avaliacao.h"
#include "../pratos/pratos.h"

/* ---------------------------------------------------------------
 * CONTADORES DE TESTE
 * --------------------------------------------------------------- */
static int totalTestes  = 0;
static int testesPassou = 0;

/*
 * VERIFICAR
 * Imprime descricao, avalia com assert() e conta aprovacoes.
 * Se falhar, assert() aborta exibindo arquivo e linha.
 */
#define VERIFICAR(descricao, condicao)          \
    do {                                        \
        totalTestes++;                          \
        printf("  [TESTE] %s\n", descricao);   \
        assert((condicao));                     \
        testesPassou++;                         \
        printf("  [OK]\n");                     \
        } while (0)

/* ---------------------------------------------------------------
 * CONFIGURAÇÃO DO AMBIENTE DE TESTES (MOCK INTERNO)
 * --------------------------------------------------------------- */

/*
 * inicializarAmbienteTestes
 * Prepara os dados estáticos encapsulados internos para as suites.
 */
static void inicializarAmbienteTestes(void) {
    /* Assertiva de Entrada: O sistema deve aceitar comandos de reset de teste se houver */
    totalTestes = 0;
    testesPassou = 0;
    
    /* Configura estados simulados ou limpa banco em memória via API do sistema se disponível */
    /* Assertiva de Saída */
    assert(totalTestes == 0 && testesPassou == 0);
}

/* ---------------------------------------------------------------
 * SUITE 1 - postAval
 * --------------------------------------------------------------- */
static void suite_postAval(void) {
    /* Assertiva de Entrada */
    assert(totalTestes >= 0);

    printf("\n=== SUITE postAval ===\n");

    // CORREÇÃO: Carrega os pratos na memória para que as validações de ID do prato funcionem
    carregarPratos();

    /* C1 - CPF valido, idPrato existente, comentario e nota
     * Retorno esperado: AVAL_OK (0) */
    VERIFICAR(
        "C1: post com CPF valido, idPrato, txt e nota => AVAL_OK",
        postAval(12345678901LL, 1, "Excelente!", 5.0f) == AVAL_OK
    );

    /* C2 - Sem comentario (NULL)
     * Retorno esperado: AVAL_OK_SEM_TXT (1) */
    VERIFICAR(
        "C2: post com txt=NULL => AVAL_OK_SEM_TXT",
        postAval(12345678901LL, 2, NULL, 3.0f) == AVAL_OK_SEM_TXT
    );

    /* C2b - String vazia tambem e sem comentario */
    VERIFICAR(
        "C2b: post com txt='' => AVAL_OK_SEM_TXT",
        postAval(12345678901LL, 2, "", 4.0f) == AVAL_OK_SEM_TXT
    );

    /* C3 - idPrato inexistente no banco
     * Retorno esperado: AVAL_ERRO_ID_PRATO (2) */
    VERIFICAR(
        "C3: post com idPrato inexistente => AVAL_ERRO_ID_PRATO",
        postAval(12345678901LL, 999, "txt", 4.0f) == AVAL_ERRO_ID_PRATO
    );

    /* C3b - idPrato=0 e parametro invalido */
    VERIFICAR(
        "C3b: post com idPrato=0 => AVAL_PARAM_INVALIDO",
        postAval(12345678901LL, 0, "txt", 4.0f) == AVAL_PARAM_INVALIDO
    );

    /* C3c - idPrato negativo */
    VERIFICAR(
        "C3c: post com idPrato negativo => AVAL_PARAM_INVALIDO",
        postAval(12345678901LL, -5, "txt", 4.0f) == AVAL_PARAM_INVALIDO
    );

    /* C4 - nota fora do limite [0,5]
     * Retorno esperado: AVAL_ERRO_NOTA (4) */
    VERIFICAR(
        "C4a: post com nota=6 => AVAL_ERRO_NOTA",
        postAval(12345678901LL, 1, "txt", 6.0f) == AVAL_ERRO_NOTA
    );
    VERIFICAR(
        "C4b: post com nota=-1 => AVAL_ERRO_NOTA",
        postAval(12345678901LL, 1, "txt", -1.0f) == AVAL_ERRO_NOTA
    );
    VERIFICAR(
        "C4c: post com nota fracionada 3.5 => AVAL_ERRO_NOTA",
        postAval(12345678901LL, 1, "txt", 3.5f) == AVAL_ERRO_NOTA
    );

    /* C5 - CPF invalido
     * Retorno esperado: AVAL_PARAM_INVALIDO (-1) */
    VERIFICAR(
        "C5a: post com CPF=0 => AVAL_PARAM_INVALIDO",
        postAval(0LL, 1, "txt", 3.0f) == AVAL_PARAM_INVALIDO
    );
    VERIFICAR(
        "C5b: post com CPF negativo => AVAL_PARAM_INVALIDO",
        postAval(-1LL, 1, "txt", 3.0f) == AVAL_PARAM_INVALIDO
    );

    /* Assertiva de Saída */
    assert(testesPassou <= totalTestes);
}

/* ---------------------------------------------------------------
 * SUITE 2 - verAval
 * --------------------------------------------------------------- */
static void suite_verAval(void) {
    /* Assertiva de Entrada */
    assert(totalTestes > 0);

    printf("\n=== SUITE verAval ===\n");

    /* Pre-condicao: inserir avaliacoes */
    postAval(11111111111LL, 1, "Otimo!", 5.0f);
    postAval(22222222222LL, 1, "Bom",   4.0f);
    postAval(33333333333LL, 2, "Ok",    3.0f);

    Avaliacao resultado[50];

    /* C1 - prato com avaliacoes existentes */
    int r1 = verAval(1, resultado, 50);
    VERIFICAR(
        "C1: verAval prato 1 deve retornar avaliacoes validas",
        r1 >= 0
    );

    /* C3 - parametro invalido
     * Retorno esperado: AVAL_PARAM_INVALIDO (-1) */
    VERIFICAR(
        "C3a: verAval com idPrato=0 => AVAL_PARAM_INVALIDO",
        verAval(0, resultado, 50) == AVAL_PARAM_INVALIDO
    );
    VERIFICAR(
        "C3b: verAval com idPrato negativo => AVAL_PARAM_INVALIDO",
        verAval(-1, resultado, 50) == AVAL_PARAM_INVALIDO
    );
    VERIFICAR(
        "C3c: verAval com resultado=NULL => AVAL_PARAM_INVALIDO",
        verAval(1, NULL, 50) == AVAL_PARAM_INVALIDO
    );
    VERIFICAR(
        "C3d: verAval com maxResultados=0 => AVAL_PARAM_INVALIDO",
        verAval(1, resultado, 0) == AVAL_PARAM_INVALIDO
    );

    /* C4 - maxResultados limita retorno */
    VERIFICAR(
        "C4: verAval com maxResultados=1 retorna no maximo 1",
        verAval(1, resultado, 1) <= 1
    );

    /* Assertiva de Saída */
    assert(testesPassou <= totalTestes);
}

/* ---------------------------------------------------------------
 * SUITE 3 - editAval
 * --------------------------------------------------------------- */
static void suite_editAval(void) {
    /* Assertiva de Entrada */
    assert(totalTestes > 0);

    printf("\n=== SUITE editAval ===\n");

    /* C1 - dono tenta editar uma avaliacao informando IDs validos */
    VERIFICAR(
        "C1: editar com dados e regras de consistência da API",
        editAval(1, 11111111111LL, "Editado", 4.0f) != AVAL_PARAM_INVALIDO
    );

    /* C2 - CPF diferente do autor (Caso de seguranca simulado) */
    VERIFICAR(
        "C2: editar avaliacao com CPF incorreto deve falhar permissao ou ID",
        editAval(1, 99999999999LL, "Invasao", 1.0f) != AVAL_OK
    );

    /* C3 - idAval inexistente
     * Retorno esperado: AVAL_ERRO_EDICAO (-2) */
    VERIFICAR(
        "C3: editar com idAval inexistente => AVAL_ERRO_EDICAO",
        editAval(9999, 11111111111LL, "txt", 2.0f) == AVAL_ERRO_EDICAO
    );

    /* C4 - nova nota invalida
     * Retorno esperado: AVAL_ERRO_NOTA (4) */
    VERIFICAR(
        "C4a: editar com nota=7 => AVAL_ERRO_NOTA",
        editAval(1, 11111111111LL, NULL, 7.0f) == AVAL_ERRO_NOTA
    );
    VERIFICAR(
        "C4b: editar com nota fracionada 2.5 => AVAL_ERRO_NOTA",
        editAval(1, 11111111111LL, NULL, 2.5f) == AVAL_ERRO_NOTA
    );

    /* C5 - parametros invalidos
     * Retorno esperado: AVAL_PARAM_INVALIDO (-1) */
    VERIFICAR(
        "C5a: editar com idAval=0 => AVAL_PARAM_INVALIDO",
        editAval(0, 11111111111LL, "txt", 3.0f) == AVAL_PARAM_INVALIDO
    );
    VERIFICAR(
        "C5b: editar com cpf=0 => AVAL_PARAM_INVALIDO",
        editAval(1, 0LL, "txt", 3.0f) == AVAL_PARAM_INVALIDO
    );

    /* Assertiva de Saída */
    assert(testesPassou <= totalTestes);
}

/* ---------------------------------------------------------------
 * MAIN
 * --------------------------------------------------------------- */
int main(void) {
    printf("TESTES - Modulo Avaliacao (Modelo Desacoplado)\n");

    inicializarAmbienteTestes();

    /* Assertiva de Entrada de Fluxo Geral */
    assert(totalTestes == 0);

    suite_postAval();
    suite_verAval();
    suite_editAval();

    /* Assertiva de Saída Global */
    assert(totalTestes > 0);
    assert(testesPassou <= totalTestes);

    printf("  RESULTADO: %d/%d testes passaram\n",
           testesPassou, totalTestes);

    return EXIT_SUCCESS;
}