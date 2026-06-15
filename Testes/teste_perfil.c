/*
 * teste_perfil.c
 * Testes unitarios - Modulo Perfil (Foodies)
 *
 * Cobre: carregarPerfis, salvarPerfis, enterPerfil, criarPerfil, getUsuario
 * Casos de teste conforme PDF 5.7 Modulo Perfil
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

/* ---------------------------------------------------------------
 * HELPER
 *   Aloca AppDados no heap e zera tudo.
 *   Nenhum JSON e lido aqui - os testes de carga tem suas proprias
 *   funcoes auxiliares.
 * --------------------------------------------------------------- */
static AppDados *alocarBanco(void) {
    AppDados *db = malloc(sizeof(AppDados));
    assert(db != NULL);
    memset(db, 0, sizeof(AppDados));
    db->proximoIdPrato = 1;
    db->proximoIdAval  = 1;
    db->cpfLogado      = 0;
    return db;
}


/* ---------------------------------------------------------------
 * SUITE 1 - criarPerfil
 * Ref. PDF 5.7 criarPerfil
 *
 * Retornos esperados:
 *   0  - Perfil criado                    (PERFIL_CRIADO)
 *   1  - Valores invalidos                (PERFIL_VALORES_INV)
 *   2  - Parametro invalido               (PERFIL_PARAM_INVALIDO)
 *   3  - CPF ja cadastrado                (PERFIL_JA_EXISTE)
 * --------------------------------------------------------------- */
static void suite_criarPerfil(void) {
    printf("\n=== SUITE criarPerfil ===\n");

    AppDados *db = alocarBanco();

    /* C1 - Criacao valida
     * Retorno esperado: PERFIL_CRIADO (0) */
    VERIFICAR(
        "C1: criarPerfil com dados validos => PERFIL_CRIADO",
        criarPerfil(db, 12345678901LL, "Joao Silva", "senha123")
            == PERFIL_CRIADO
    );
    VERIFICAR(
        "C1: banco deve ter 1 usuario apos criar",
        db->nUsuarios == 1
    );
    VERIFICAR(
        "C1: CPF gravado deve ser o informado",
        db->usuarios[0].cpf == 12345678901LL
    );
    VERIFICAR(
        "C1: nome gravado deve ser o informado",
        strcmp(db->usuarios[0].nome, "Joao Silva") == 0
    );

    /* C2 - CPF invalido (menos de 11 digitos)
     * Retorno esperado: PERFIL_VALORES_INV (1) */
    VERIFICAR(
        "C2: criarPerfil com CPF de 10 digitos => PERFIL_VALORES_INV",
        criarPerfil(db, 1234567890LL, "Maria", "senha") == PERFIL_VALORES_INV
    );

    /* C2b - Nome vazio */
    VERIFICAR(
        "C2b: criarPerfil com nome vazio => PERFIL_VALORES_INV",
        criarPerfil(db, 11122233344LL, "", "senha123") == PERFIL_VALORES_INV
    );

    /* C2c - Senha vazia */
    VERIFICAR(
        "C2c: criarPerfil com senha vazia => PERFIL_VALORES_INV",
        criarPerfil(db, 11122233344LL, "Ana", "") == PERFIL_VALORES_INV
    );

    /* C3 - Parametros invalidos
     * Retorno esperado: PERFIL_PARAM_INVALIDO (2) */
    VERIFICAR(
        "C3a: criarPerfil com db=NULL => PERFIL_PARAM_INVALIDO",
        criarPerfil(NULL, 12345678901LL, "X", "y") == PERFIL_PARAM_INVALIDO
    );
    VERIFICAR(
        "C3b: criarPerfil com cpf=0 => PERFIL_PARAM_INVALIDO",
        criarPerfil(db, 0LL, "X", "y") == PERFIL_PARAM_INVALIDO
    );
    VERIFICAR(
        "C3c: criarPerfil com nome=NULL => PERFIL_PARAM_INVALIDO",
        criarPerfil(db, 11122233344LL, NULL, "y") == PERFIL_PARAM_INVALIDO
    );
    VERIFICAR(
        "C3d: criarPerfil com senha=NULL => PERFIL_PARAM_INVALIDO",
        criarPerfil(db, 11122233344LL, "X", NULL) == PERFIL_PARAM_INVALIDO
    );

    /* C4 - CPF ja cadastrado (duplicata)
     * Retorno esperado: PERFIL_JA_EXISTE (3) */
    VERIFICAR(
        "C4: criarPerfil com CPF ja existente => PERFIL_JA_EXISTE",
        criarPerfil(db, 12345678901LL, "Outro", "outrasenha")
            == PERFIL_JA_EXISTE
    );
    VERIFICAR(
        "C4: banco deve continuar com 1 usuario (nao duplicou)",
        db->nUsuarios == 1
    );

    free(db);
}

/* ---------------------------------------------------------------
 * SUITE 2 - enterPerfil
 * Ref. PDF 5.7 enterPerfil
 *
 * Retornos esperados:
 *   0  - Entrou na pagina Perfil (autenticado) (PERFIL_OK)
 *   1  - CPF ou senha invalidos               (PERFIL_DADOS_INVALIDOS)
 *   2  - Parametro invalido                    (PERFIL_PARAM_INVALIDO)
 * --------------------------------------------------------------- */
static void suite_enterPerfil(void) {
    printf("\n=== SUITE enterPerfil ===\n");

    AppDados *db = alocarBanco();

    /* Pre-condicao: criar usuario para testar login */
    criarPerfil(db, 12345678901LL, "Joao Silva", "senha123");

    /* C1 - Dados validos (CPF e senha corretos)
     * Retorno esperado: PERFIL_OK (0) */
    VERIFICAR(
        "C1: enterPerfil com dados validos => PERFIL_OK",
        enterPerfil(db, 12345678901LL, "senha123") == PERFIL_OK
    );
    VERIFICAR(
        "C1: cpfLogado deve ser atualizado apos login",
        db->cpfLogado == 12345678901LL
    );

    /* C2 - Dados invalidos: CPF nao existe ou senha errada
     * Retorno esperado: PERFIL_DADOS_INVALIDOS (1) */
    VERIFICAR(
        "C2a: enterPerfil com CPF inexistente => PERFIL_DADOS_INVALIDOS",
        enterPerfil(db, 99999999999LL, "senha123") == PERFIL_DADOS_INVALIDOS
    );
    VERIFICAR(
        "C2b: enterPerfil com senha errada => PERFIL_DADOS_INVALIDOS",
        enterPerfil(db, 12345678901LL, "errada") == PERFIL_DADOS_INVALIDOS
    );

    /* C3 - Parametros invalidos
     * Retorno esperado: PERFIL_PARAM_INVALIDO (2) */
    VERIFICAR(
        "C3a: enterPerfil com db=NULL => PERFIL_PARAM_INVALIDO",
        enterPerfil(NULL, 12345678901LL, "senha123") == PERFIL_PARAM_INVALIDO
    );
    VERIFICAR(
        "C3b: enterPerfil com cpf=0 => PERFIL_PARAM_INVALIDO",
        enterPerfil(db, 0LL, "senha123") == PERFIL_PARAM_INVALIDO
    );
    VERIFICAR(
        "C3c: enterPerfil com senha=NULL => PERFIL_PARAM_INVALIDO",
        enterPerfil(db, 12345678901LL, NULL) == PERFIL_PARAM_INVALIDO
    );

    free(db);
}

/* ---------------------------------------------------------------
 * SUITE 3 - getUsuario
 * Ref. PDF 5.7 getUsuario
 *
 * Retornos esperados:
 *   Usuario*  - ponteiro para o registro encontrado (dici_usu)
 *   NULL      - CPF nao encontrado ou parametro invalido
 * --------------------------------------------------------------- */
static void suite_getUsuario(void) {
    printf("\n=== SUITE getUsuario ===\n");

    AppDados *db = alocarBanco();

    criarPerfil(db, 12345678901LL, "Joao Silva", "senha123");
    criarPerfil(db, 98765432100LL, "Ana Costa",  "outrasenha");

    /* C1 - CPF existente
     * Retorno esperado: ponteiro valido (dici_usu) */
    Usuario *u1 = getUsuario(db, 12345678901LL);
    VERIFICAR(
        "C1: getUsuario com CPF existente retorna ponteiro valido",
        u1 != NULL
    );
    VERIFICAR(
        "C1: CPF do usuario retornado confere",
        u1 != NULL && u1->cpf == 12345678901LL
    );
    VERIFICAR(
        "C1: nome do usuario retornado confere",
        u1 != NULL && strcmp(u1->nome, "Joao Silva") == 0
    );

    /* C2 - CPF nao existente
     * Retorno esperado: NULL (nulo) */
    VERIFICAR(
        "C2: getUsuario com CPF nao existente => NULL",
        getUsuario(db, 11111111111LL) == NULL
    );

    /* C3 - Parametros invalidos
     * Retorno esperado: NULL */
    VERIFICAR(
        "C3a: getUsuario com db=NULL => NULL",
        getUsuario(NULL, 12345678901LL) == NULL
    );
    VERIFICAR(
        "C3b: getUsuario com cpf=0 => NULL",
        getUsuario(db, 0LL) == NULL
    );
    VERIFICAR(
        "C3c: getUsuario com cpf negativo => NULL",
        getUsuario(db, -1LL) == NULL
    );

    free(db);
}

/* ---------------------------------------------------------------
 * SUITE 4 - carregarPerfis e salvarPerfis (persistencia)
 * Verifica a metrica 5: dados gravados ao final e recuperados
 * em nova execucao.
 * --------------------------------------------------------------- */
static void suite_persistencia(void) {
    printf("\n=== SUITE persistencia (salvar/carregar) ===\n");

    /* --- SALVAR: criar banco, inserir usuarios, salvar --- */
    AppDados *db1 = alocarBanco();

    criarPerfil(db1, 12345678901LL, "Joao Silva",  "senha123");
    criarPerfil(db1, 98765432100LL, "Ana Costa",   "outrasenha");
    criarPerfil(db1, 11122233344LL, "Carlos Melo", "pass456");

    /* Redirecionar para arquivo de teste (nao sobrescreve producao) */
    /* Usamos salvarPerfis diretamente - ele grava em PERFIL_JSON    */
    int ret_salvar = salvarPerfis(db1);
    VERIFICAR(
        "SALVAR: salvarPerfis deve retornar 0 (sucesso)",
        ret_salvar == 0
    );

    free(db1);

    /* --- CARREGAR: novo banco vazio, carregar do arquivo --- */
    AppDados *db2 = alocarBanco();

    int ret_carregar = carregarPerfis(db2);
    VERIFICAR(
        "CARREGAR: carregarPerfis deve retornar 0 (sucesso)",
        ret_carregar == 0
    );
    VERIFICAR(
        "CARREGAR: banco deve ter 3 usuarios apos carregar",
        db2->nUsuarios == 3
    );

    /* Verificar integridade de cada campo */
    Usuario *u = getUsuario(db2, 12345678901LL);
    VERIFICAR(
        "CARREGAR: usuario 1 CPF recuperado corretamente",
        u != NULL && u->cpf == 12345678901LL
    );
    VERIFICAR(
        "CARREGAR: usuario 1 nome recuperado corretamente",
        u != NULL && strcmp(u->nome, "Joao Silva") == 0
    );
    VERIFICAR(
        "CARREGAR: usuario 1 senha recuperada corretamente",
        u != NULL && strcmp(u->senha, "senha123") == 0
    );

    Usuario *u2 = getUsuario(db2, 98765432100LL);
    VERIFICAR(
        "CARREGAR: usuario 2 recuperado corretamente",
        u2 != NULL && strcmp(u2->nome, "Ana Costa") == 0
    );

    Usuario *u3 = getUsuario(db2, 11122233344LL);
    VERIFICAR(
        "CARREGAR: usuario 3 recuperado corretamente",
        u3 != NULL && strcmp(u3->nome, "Carlos Melo") == 0
    );

    /* Login deve funcionar com dados carregados do arquivo */
    VERIFICAR(
        "CARREGAR: enterPerfil funciona com dados carregados do JSON",
        enterPerfil(db2, 12345678901LL, "senha123") == PERFIL_OK
    );

    free(db2);

    /* Limpar arquivo gerado pelo teste */
    remove(PERFIL_JSON);
}

/* ---------------------------------------------------------------
 * MAIN
 * --------------------------------------------------------------- */
int main(void) {
    printf("TESTES - Modulo Perfil\n");

    suite_criarPerfil();
    suite_enterPerfil();
    suite_getUsuario();
    suite_persistencia();

    printf("  RESULTADO: %d/%d testes passaram\n",
           testesPassou, totalTestes);

    return EXIT_SUCCESS;
}