/*
 * teste_principal.c
 * Testes unitarios - Modulo Principal (Foodies)
 *
 * Cobre: iniciarApp, encerrarApp, carregarAvaliacoes, salvarAvaliacoes
 * Verifica a metrica 5: persistencia entre execucoes
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../principal/principal.h"
#include "../perfil/perfil.h"
#include "../avaliacao/avaliacao.h"

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

/* Remove arquivos de teste para isolar suites */
static void limparJsons(void) {
    remove(PERFIL_JSON);
    remove(AVALIACOES_JSON);
}

/* ---------------------------------------------------------------
 * SUITE 1 - iniciarApp e encerrarApp
 * Verifica que a alocacao e liberacao funcionam corretamente.
 * --------------------------------------------------------------- */
static void suite_cicloDeVida(void) {
    printf("\n=== SUITE ciclo de vida (iniciar/encerrar) ===\n");

    limparJsons();

    /* C1 - iniciarApp deve retornar ponteiro valido */
    AppDados *db = iniciarApp();
    VERIFICAR(
        "C1: iniciarApp retorna ponteiro valido (nao NULL)",
        db != NULL
    );

    /* C2 - banco deve estar zerado (nenhum JSON existia) */
    VERIFICAR(
        "C2: banco inicia com 0 usuarios (sem JSON previo)",
        db != NULL && db->nUsuarios == 0
    );
    VERIFICAR(
        "C2: banco inicia com 0 avaliacoes (sem JSON previo)",
        db != NULL && db->nAvaliacoes == 0
    );
    VERIFICAR(
        "C2: cpfLogado inicia em 0 (sem sessao)",
        db != NULL && db->cpfLogado == 0
    );
    VERIFICAR(
        "C2: proximoIdAval inicia em 1",
        db != NULL && db->proximoIdAval == 1
    );

    /* C3 - encerrarApp nao deve crashar */
    encerrarApp(db);
    VERIFICAR(
        "C3: encerrarApp com banco valido nao crasha",
        1  /* se chegou aqui, nao crashou */
    );

    /* C4 - encerrarApp com NULL nao deve crashar */
    encerrarApp(NULL);
    VERIFICAR(
        "C4: encerrarApp com NULL nao crasha",
        1
    );

    limparJsons();
}

/* ---------------------------------------------------------------
 * SUITE 2 - persistencia de avaliacoes
 * Verifica a metrica 5: dados gravados ao final e recuperados
 * na proxima execucao.
 * --------------------------------------------------------------- */
static void suite_persistenciaAvaliacoes(void) {
    printf("\n=== SUITE persistencia avaliacoes ===\n");

    limparJsons();

    /* --- EXECUCAO 1: criar dados e encerrar --- */
    AppDados *db1 = iniciarApp();
    assert(db1 != NULL);

    /* Adicionar um prato diretamente para que postAval funcione */
    db1->pratos[0].idPrato = 1;
    strncpy(db1->pratos[0].nome, "Frango Grelhado", TAM_NOME - 1);
    db1->pratos[1].idPrato = 2;
    strncpy(db1->pratos[1].nome, "Salada Caesar", TAM_NOME - 1);
    db1->nPratos       = 2;
    db1->proximoIdPrato = 3;

    /* Criar um usuario */
    criarPerfil(db1, 12345678901LL, "Joao", "senha123");

    /* Criar avaliacoes */
    postAval(db1, 12345678901LL, 1, "Muito bom!", 5.0f);
    postAval(db1, 12345678901LL, 2, "",            3.0f);
    postAval(db1, 12345678901LL, 1, NULL,          4.0f);

    VERIFICAR(
        "EXEC1: banco deve ter 3 avaliacoes antes de encerrar",
        db1->nAvaliacoes == 3
    );
    VERIFICAR(
        "EXEC1: banco deve ter 1 usuario antes de encerrar",
        db1->nUsuarios == 1
    );

    /* Encerrar: salva perfis.json e avaliacoes.json */
    encerrarApp(db1);

    /* --- EXECUCAO 2: iniciar do zero e verificar recuperacao --- */
    AppDados *db2 = iniciarApp();
    assert(db2 != NULL);

    /* Verificar avaliacoes recuperadas */
    VERIFICAR(
        "EXEC2: deve ter 3 avaliacoes carregadas do JSON",
        db2->nAvaliacoes == 3
    );
    VERIFICAR(
        "EXEC2: primeira avaliacao deve ter nota 5",
        (int)db2->avaliacoes[0].nota == 5
    );
    VERIFICAR(
        "EXEC2: primeira avaliacao deve ter comentario correto",
        strcmp(db2->avaliacoes[0].comentario, "Muito bom!") == 0
    );
    VERIFICAR(
        "EXEC2: segunda avaliacao deve ter nota 3",
        (int)db2->avaliacoes[1].nota == 3
    );
    VERIFICAR(
        "EXEC2: segunda avaliacao deve ter comentario vazio",
        db2->avaliacoes[1].comentario[0] == '\0'
    );
    VERIFICAR(
        "EXEC2: terceira avaliacao deve ter nota 4",
        (int)db2->avaliacoes[2].nota == 4
    );
    VERIFICAR(
        "EXEC2: CPF das avaliacoes recuperado corretamente",
        db2->avaliacoes[0].cpf == 12345678901LL
    );
    VERIFICAR(
        "EXEC2: idPrato das avaliacoes recuperado corretamente",
        db2->avaliacoes[0].idPrato == 1
    );

    /* Verificar usuarios recuperados */
    VERIFICAR(
        "EXEC2: deve ter 1 usuario carregado do JSON",
        db2->nUsuarios == 1
    );
    VERIFICAR(
        "EXEC2: CPF do usuario recuperado corretamente",
        db2->usuarios[0].cpf == 12345678901LL
    );
    VERIFICAR(
        "EXEC2: nome do usuario recuperado corretamente",
        strcmp(db2->usuarios[0].nome, "Joao") == 0
    );

    /* proximoIdAval deve continuar da sequencia anterior */
    VERIFICAR(
        "EXEC2: proximoIdAval deve ser 4 (continua sequencia)",
        db2->proximoIdAval == 4
    );

    /* Nova avaliacao na exec 2 deve receber idAval=4 */
    db2->pratos[0].idPrato = 1;
    db2->nPratos = 1;
    postAval(db2, 12345678901LL, 1, "Nova exec", 2.0f);
    VERIFICAR(
        "EXEC2: nova avaliacao deve ter idAval=4",
        db2->avaliacoes[3].idAval == 4
    );

    encerrarApp(db2);

    /* --- EXECUCAO 3: verificar que a 4a avaliacao foi persistida --- */
    AppDados *db3 = iniciarApp();
    assert(db3 != NULL);

    VERIFICAR(
        "EXEC3: deve ter 4 avaliacoes acumuladas",
        db3->nAvaliacoes == 4
    );
    VERIFICAR(
        "EXEC3: quarta avaliacao tem nota 2",
        (int)db3->avaliacoes[3].nota == 2
    );
    VERIFICAR(
        "EXEC3: quarta avaliacao tem comentario correto",
        strcmp(db3->avaliacoes[3].comentario, "Nova exec") == 0
    );

    encerrarApp(db3);
    limparJsons();
}

/* ---------------------------------------------------------------
 * SUITE 3 - carregarAvaliacoes e salvarAvaliacoes isolados
 * Testa as funcoes de I/O diretamente com bancos controlados.
 * --------------------------------------------------------------- */
static void suite_ioAvaliacoes(void) {
    printf("\n=== SUITE I/O avaliacoes ===\n");

    limparJsons();

    /* Montar banco manualmente */
    AppDados *db = malloc(sizeof(AppDados));
    assert(db != NULL);
    memset(db, 0, sizeof(AppDados));
    db->proximoIdAval  = 1;
    db->proximoIdPrato = 1;

    /* Inserir prato e avaliacoes diretamente */
    db->pratos[0].idPrato = 1;
    strncpy(db->pratos[0].nome, "Prato Teste", TAM_NOME - 1);
    db->nPratos = 1;

    postAval(db, 11111111111LL, 1, "Comentario aqui", 5.0f);
    postAval(db, 22222222222LL, 1, NULL,              4.0f);

    /* Salvar */
    int ret_salvar = salvarAvaliacoes(db);
    VERIFICAR(
        "IO: salvarAvaliacoes retorna 0 (sucesso)",
        ret_salvar == 0
    );
    free(db);

    /* Carregar em banco novo */
    AppDados *db2 = malloc(sizeof(AppDados));
    assert(db2 != NULL);
    memset(db2, 0, sizeof(AppDados));
    db2->proximoIdAval = 1;

    int ret_carregar = carregarAvaliacoes(db2);
    VERIFICAR(
        "IO: carregarAvaliacoes retorna 0 (sucesso)",
        ret_carregar == 0
    );
    VERIFICAR(
        "IO: 2 avaliacoes carregadas",
        db2->nAvaliacoes == 2
    );
    VERIFICAR(
        "IO: nota da primeira avaliacao = 5",
        (int)db2->avaliacoes[0].nota == 5
    );
    VERIFICAR(
        "IO: comentario da primeira avaliacao correto",
        strcmp(db2->avaliacoes[0].comentario, "Comentario aqui") == 0
    );
    VERIFICAR(
        "IO: comentario da segunda avaliacao vazio",
        db2->avaliacoes[1].comentario[0] == '\0'
    );
    VERIFICAR(
        "IO: proximoIdAval deve ser 3 apos carregar",
        db2->proximoIdAval == 3
    );

    /* Parametro invalido */
    VERIFICAR(
        "IO: carregarAvaliacoes com db=NULL retorna -1",
        carregarAvaliacoes(NULL) == -1
    );
    VERIFICAR(
        "IO: salvarAvaliacoes com db=NULL retorna -1",
        salvarAvaliacoes(NULL) == -1
    );

    free(db2);
    limparJsons();
}

/* ---------------------------------------------------------------
 * MAIN
 * --------------------------------------------------------------- */
int main(void) {
    printf("TESTES - Modulo Principal\n");

    suite_cicloDeVida();
    suite_persistenciaAvaliacoes();
    suite_ioAvaliacoes();

    printf("  RESULTADO: %d/%d testes passaram\n",
           testesPassou, totalTestes);

    return EXIT_SUCCESS;
}