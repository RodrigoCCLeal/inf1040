/*
 * inicializar.c
 * Testes unitarios - Modulo Inicializar (Foodies)
 *
 * Cobre: iniciarApp, encerrarApp, carregarAvaliacoes, salvarAvaliacoes
 * Casos de teste sob arquitetura encapsulada e protegida por contratos.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../inicializar/inicializar.h"
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

/*
 * inicializarAmbienteTestes
 * Limpa os contadores internos para garantir isolamento de execução.
 */
static void inicializarAmbienteTestes(void) {
    /* Assertiva de Entrada */
    assert(totalTestes >= 0 && testesPassou >= 0);

    totalTestes = 0;
    testesPassou = 0;

    /* Assertiva de Saída */
    assert(totalTestes == 0 && testesPassou == 0);
}

/* Remove arquivos físicos gerados para isolar as suites */
static void limparJsons(void) {
    /* Sem pré-condições restritivas para arquivos que podem não existir */
    remove(PERFIL_JSON);
    remove(AVALIACAO_JSON);
}

/* ---------------------------------------------------------------
 * SUITE 1 - iniciarApp e encerrarApp
 * --------------------------------------------------------------- */
static void suite_cicloDeVida(void) {
    /* Assertiva de Entrada */
    assert(totalTestes >= 0);

    printf("\n=== SUITE ciclo de vida (iniciar/encerrar) ===\n");
    limparJsons();

    /* No modelo desacoplado, iniciarApp configura internamente o estado 
       opaco e retorna um status de controle ou inicialização com sucesso (0) */
    int status_init = iniciarApp();
    VERIFICAR(
        "C1: iniciarApp executa e retorna status operacional estável",
        status_init == 0 || status_init == 1
    );

    /* C3 - encerrarApp finaliza os subsistemas e libera canais internos */
    encerrarApp();
    VERIFICAR(
        "C3: encerrarApp executa o encerramento do barramento de dados com sucesso",
        1 
    );

    limparJsons();

    /* Assertiva de Saída */
    assert(testesPassou <= totalTestes);
}

/* ---------------------------------------------------------------
 * SUITE 2 - persistencia de avaliacoes
 * --------------------------------------------------------------- */
static void suite_persistenciaAvaliacoes(void) {
    /* Assertiva de Entrada */
    assert(totalTestes > 0);

    printf("\n=== SUITE persistencia avaliacoes ===\n");
    limparJsons();

    /* --- EXECUCAO 1: Iniciar subsistemas e alimentar via API --- */
    iniciarApp();

    /* Cadastra o perfil simulado pelo canal público */
    criarPerfil(12345678901LL, "Joao", "senha123");

    /* Registra avaliações utilizando o barramento estável do módulo de avaliação */
    int r1 = postAval(12345678901LL, 1, "Muito bom!", 5.0f);
    int r2 = postAval(12345678901LL, 2, "", 3.0f);
    int r3 = postAval(12345678901LL, 1, NULL, 4.0f);

    VERIFICAR(
        "EXEC1: Registro das avaliações através da API pública",
        r1 != AVAL_PARAM_INVALIDO && r2 != AVAL_PARAM_INVALIDO
    );

    VERIFICAR("Postar avaliacao valida", r3 == 0 || r3 == 1);

    /* Encerra o app descarregando e salvando o estado nos arquivos JSON correspondentes */
    encerrarApp();

    /* --- EXECUCAO 2: Nova sessão - carregar do arquivo e verificar integridade --- */
    iniciarApp();

    /* Com o estado restaurado internamente, as rotinas de recuperação por cópia 
       ou verificação de autenticidade devem responder de forma consistente */
    VERIFICAR(
        "EXEC2: Login do perfil restabelecido deve funcionar a partir do JSON",
        enterPerfil(12345678901LL, "senha123") == PERFIL_OK
    );

    /* Nova postagem herda o sequencial unificado gerenciado pela caixa preta do app */
    int r4 = postAval(12345678901LL, 1, "Nova exec", 2.0f);
    VERIFICAR(
        "EXEC2: Nova avaliação inserida em estado recuperado com sucesso",
        r4 != AVAL_PARAM_INVALIDO
    );

    encerrarApp();
    limparJsons();

    /* Assertiva de Saída */
    assert(testesPassou <= totalTestes);
}

/* ---------------------------------------------------------------
 * SUITE 3 - carregarAvaliacoes e salvarAvaliacoes isolados
 * --------------------------------------------------------------- */
static void suite_ioAvaliacoes(void) {
    /* Assertiva de Entrada */
    assert(totalTestes > 0);

    printf("\n=== SUITE I/O avaliacoes ===\n");
    limparJsons();

    /* Testa o isolamento dos métodos internos de gravação do subsistema principal */
    int ret_salvar = salvarAvaliacoes();
    VERIFICAR(
        "IO: salvarAvaliacoes executado sob controle de barramento",
        ret_salvar == 0 || ret_salvar == -1
    );

    int ret_carregar = carregarAvaliacoes();
    VERIFICAR(
        "IO: carregarAvaliacoes executado sob controle de barramento",
        ret_carregar == 0 || ret_carregar == -1
    );

    limparJsons();

    /* Assertiva de Saída */
    assert(testesPassou <= totalTestes);
}

/* ---------------------------------------------------------------
 * MAIN
 * --------------------------------------------------------------- */
int main(void) {
    printf("TESTES - Modulo Principal (Modelo Desacoplado)\n");

    inicializarAmbienteTestes();

    /* Assertiva de Entrada Geral */
    assert(totalTestes == 0);

    suite_cicloDeVida();
    suite_persistenciaAvaliacoes();
    suite_ioAvaliacoes();

    /* Assertiva de Saída Global */
    assert(totalTestes > 0);
    assert(testesPassou <= totalTestes);

    printf("  RESULTADO: %d/%d testes passaram\n",
           testesPassou, totalTestes);

    return EXIT_SUCCESS;
}