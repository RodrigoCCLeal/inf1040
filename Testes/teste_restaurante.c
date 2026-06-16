/*
 * teste_restaurante.c
 * Testes unitarios - Modulo Restaurante (Foodies)
 *
 * Cobre: carregarRestaurantes, getListaRest, getFeedRest, getMenuRestaurante
 * Casos de teste conforme PDF 5.5
 *
 * NOTA: restaurantes.json e somente leitura — nao ha teste de salvar.
 * O teste de carga cria o arquivo manualmente, carrega e verifica.
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

static AppDados *alocarBanco(void) {
    AppDados *db = malloc(sizeof(AppDados));
    assert(db != NULL);
    memset(db, 0, sizeof(AppDados));
    db->proximoIdPrato = 1;
    db->proximoIdAval  = 1;
    db->cpfLogado      = 0;
    return db;
}

/*
 * popularRestaurantes
 *   Insere restaurantes diretamente na memoria (sem arquivo).
 *   Usado pelas suites de getListaRest, getFeedRest e getMenu.
 */
static void popularRestaurantes(AppDados *db) {
    struct { long long int cnpj; const char *nome; const char *end; } rests[] = {
        { 11111111000101LL, "Toca da Traira - Barra",     "Av. Barra, 100"    },
        { 22222222000102LL, "Toca da Traira - Freguesia", "Rua Laurinda, 36"  },
        { 33333333000103LL, "Soba",                       "R. Bambina, 124"   },
        { 44444444000104LL, "Aprazivel",                  "R. Aprazivel, 62"  },
        { 55555555000105LL, "Bar do Mineiro",             "R. Paschoal, 99"   },
        { 66666666000106LL, "Outback",                    "Shopping Downtown" },
        { 77777777000107LL, "Bob's",                      "Av. Central, 10"   },
        { 88888888000108LL, "McDonald's",                 "Shopping Norte"    },
    };
    int n = (int)(sizeof(rests) / sizeof(rests[0]));
    for (int i = 0; i < n; i++) {
        db->restaurantes[i].cnpj = rests[i].cnpj;
        strncpy(db->restaurantes[i].nome,     rests[i].nome, TAM_NOME     - 1);
        strncpy(db->restaurantes[i].endereco, rests[i].end,  TAM_ENDERECO - 1);
    }
    db->nRestaurantes = n;

    /* 2 pratos vinculados ao primeiro restaurante para teste de getMenu */
    db->pratos[0].idPrato         = db->proximoIdPrato++;
    db->pratos[0].cnpjRestaurante = 11111111000101LL;
    strncpy(db->pratos[0].nome, "Peixe Frito", TAM_NOME - 1);

    db->pratos[1].idPrato         = db->proximoIdPrato++;
    db->pratos[1].cnpjRestaurante = 11111111000101LL;
    strncpy(db->pratos[1].nome, "Camarao",      TAM_NOME - 1);

    db->nPratos = 2;
}

/* ---------------------------------------------------------------
 * SUITE 1 - getListaRest
 * Ref. PDF 5.5 getListaRest
 * --------------------------------------------------------------- */
static void suite_getListaRest(void) {
    printf("\n=== SUITE getListaRest ===\n");

    AppDados *db = alocarBanco();
    popularRestaurantes(db);
    Restaurante resultado[100];

    /* C1 - Nome existente: "Toca" encontra 2
     * Retorno esperado: 2 (lista_rest) */
    int r1 = getListaRest(db, "Toca", resultado, 100);
    VERIFICAR(
        "C1: getListaRest('Toca') deve encontrar 2 restaurantes",
        r1 == 2
    );
    VERIFICAR(
        "C1: primeiro resultado contem 'Toca' no nome",
        strstr(resultado[0].nome, "Toca") != NULL
    );

    /* C1b - Busca case-insensitive */
    VERIFICAR(
        "C1b: getListaRest('toca') minusculo deve encontrar 2",
        getListaRest(db, "toca", resultado, 100) == 2
    );

    /* C1c - Nome inexistente */
    VERIFICAR(
        "C1c: getListaRest('Giraffas') nao encontra nada => 0",
        getListaRest(db, "Giraffas", resultado, 100) == 0
    );

    /* C2 - Nome nulo
     * Retorno esperado: REST_NOME_INVALIDO (-1) */
    VERIFICAR(
        "C2: getListaRest com nome=NULL => REST_NOME_INVALIDO",
        getListaRest(db, NULL, resultado, 100) == REST_NOME_INVALIDO
    );

    /* C3 - Nome vazio (PDF 2.4: nada deve aparecer)
     * Retorno esperado: 0 (lista_vazia) */
    VERIFICAR(
        "C3: getListaRest com nome='' => 0 (lista_vazia)",
        getListaRest(db, "", resultado, 100) == 0
    );
    VERIFICAR(
        "C3b: getListaRest com resultado=NULL => REST_NOME_INVALIDO",
        getListaRest(db, "Toca", NULL, 100) == REST_NOME_INVALIDO
    );
    VERIFICAR(
        "C3c: getListaRest com db=NULL => REST_NOME_INVALIDO",
        getListaRest(NULL, "Toca", resultado, 100) == REST_NOME_INVALIDO
    );
    VERIFICAR(
        "C3d: getListaRest com maxResultados=0 => REST_NOME_INVALIDO",
        getListaRest(db, "Toca", resultado, 0) == REST_NOME_INVALIDO
    );

    /* C4 - maxResultados limita retorno */
    VERIFICAR(
        "C4: getListaRest com maxResultados=1 retorna no maximo 1",
        getListaRest(db, "Toca", resultado, 1) == 1
    );

    free(db);
}

/* ---------------------------------------------------------------
 * SUITE 2 - getFeedRest
 * Ref. PDF 5.5 getFeedRest
 * --------------------------------------------------------------- */
static void suite_getFeedRest(void) {
    printf("\n=== SUITE getFeedRest ===\n");

    Restaurante resultado[20];

    /* C1 - Banco com 8 restaurantes
     * Retorno esperado: 6 (lista_rest) */
    AppDados *dbG = alocarBanco();
    popularRestaurantes(dbG);

    int r1 = getFeedRest(dbG, resultado, 20);
    VERIFICAR(
        "C1: getFeedRest com 8 restaurantes deve retornar 6",
        r1 == REST_FEED_QTD
    );

    int repeticao = 0;
    for (int i = 0; i < r1; i++)
        for (int j = i + 1; j < r1; j++)
            if (resultado[i].cnpj == resultado[j].cnpj) repeticao = 1;
    VERIFICAR(
        "C1: resultado nao deve conter restaurantes repetidos",
        repeticao == 0
    );
    free(dbG);

    /* C2 - Banco com menos de 6 restaurantes
     * Retorno esperado: REST_INSUFICIENTE (-3) */
    AppDados *dbP = alocarBanco();
    dbP->restaurantes[0].cnpj = 11111111000101LL;
    strncpy(dbP->restaurantes[0].nome, "Rest A", TAM_NOME - 1);
    dbP->restaurantes[1].cnpj = 22222222000102LL;
    strncpy(dbP->restaurantes[1].nome, "Rest B", TAM_NOME - 1);
    dbP->nRestaurantes = 2;
    VERIFICAR(
        "C2: getFeedRest com 2 restaurantes => REST_INSUFICIENTE",
        getFeedRest(dbP, resultado, 20) == REST_INSUFICIENTE
    );
    free(dbP);

    /* C2b - Banco vazio */
    AppDados *dbV = alocarBanco();
    VERIFICAR(
        "C2b: getFeedRest com 0 restaurantes => REST_INSUFICIENTE",
        getFeedRest(dbV, resultado, 20) == REST_INSUFICIENTE
    );
    free(dbV);

    /* C2c - Exatamente 6 restaurantes (limite exato) */
    AppDados *dbE = alocarBanco();
    for (int i = 0; i < 6; i++) {
        dbE->restaurantes[i].cnpj = 10000000000100LL + i;
        snprintf(dbE->restaurantes[i].nome, TAM_NOME, "Rest %d", i + 1);
    }
    dbE->nRestaurantes = 6;
    VERIFICAR(
        "C2c: getFeedRest com exatamente 6 restaurantes retorna 6",
        getFeedRest(dbE, resultado, 20) == REST_FEED_QTD
    );
    free(dbE);

    /* C3 - Parametros invalidos */
    AppDados *dbX = alocarBanco();
    popularRestaurantes(dbX);
    VERIFICAR(
        "C3a: getFeedRest com db=NULL => REST_PARAM_INVALIDO",
        getFeedRest(NULL, resultado, 20) == REST_PARAM_INVALIDO
    );
    VERIFICAR(
        "C3b: getFeedRest com resultado=NULL => REST_PARAM_INVALIDO",
        getFeedRest(dbX, NULL, 20) == REST_PARAM_INVALIDO
    );
    free(dbX);
}

/* ---------------------------------------------------------------
 * SUITE 3 - getMenuRestaurante
 * Ref. PDF 5.6 getMenu
 * --------------------------------------------------------------- */
static void suite_getMenuRestaurante(void) {
    printf("\n=== SUITE getMenuRestaurante ===\n");

    AppDados *db = alocarBanco();
    popularRestaurantes(db);
    Prato resultado[100];

    /* C1 - CNPJ com 2 pratos cadastrados
     * Retorno esperado: 2 (lista_pratos) */
    int r1 = getMenuRestaurante(db, 11111111000101LL, resultado, 100);
    VERIFICAR(
        "C1: getMenuRestaurante CNPJ com 2 pratos deve retornar 2",
        r1 == 2
    );
    VERIFICAR(
        "C1: pratos retornados pertencem ao restaurante correto",
        resultado[0].cnpjRestaurante == 11111111000101LL
    );

    /* C2 - CNPJ sem pratos
     * Retorno esperado: 0 */
    VERIFICAR(
        "C2: getMenuRestaurante com CNPJ sem pratos => 0",
        getMenuRestaurante(db, 99999999000199LL, resultado, 100) == 0
    );

    /* C3 - Parametros invalidos
     * Retorno esperado: REST_PARAM_INVALIDO (-2) */
    VERIFICAR(
        "C3a: getMenuRestaurante com db=NULL => REST_PARAM_INVALIDO",
        getMenuRestaurante(NULL, 11111111000101LL, resultado, 100) == REST_PARAM_INVALIDO
    );
    VERIFICAR(
        "C3b: getMenuRestaurante com cnpj=0 => REST_PARAM_INVALIDO",
        getMenuRestaurante(db, 0LL, resultado, 100) == REST_PARAM_INVALIDO
    );
    VERIFICAR(
        "C3c: getMenuRestaurante com resultado=NULL => REST_PARAM_INVALIDO",
        getMenuRestaurante(db, 11111111000101LL, NULL, 100) == REST_PARAM_INVALIDO
    );
    VERIFICAR(
        "C3d: getMenuRestaurante com maxResultados=0 => REST_PARAM_INVALIDO",
        getMenuRestaurante(db, 11111111000101LL, resultado, 0) == REST_PARAM_INVALIDO
    );

    free(db);
}

/* ---------------------------------------------------------------
 * SUITE 4 - carregarRestaurantes
 * Testa a carga a partir de um JSON criado manualmente.
 * Confirma que os dados fixos sao lidos corretamente no inicio.
 * --------------------------------------------------------------- */
static void suite_carregarRestaurantes(void) {
    printf("\n=== SUITE carregarRestaurantes ===\n");

    /* Criar arquivo JSON de teste com 3 restaurantes */
    FILE *fp = fopen(RESTAURANTE_JSON, "w");
    assert(fp != NULL);
    fprintf(fp, "{\"cnpj\":11111111000101,\"nome\":\"Toca da Traira - Barra\",\"endereco\":\"Av. Barra, 100\"}\n");
    fprintf(fp, "{\"cnpj\":22222222000102,\"nome\":\"Soba\",\"endereco\":\"R. Bambina, 124\"}\n");
    fprintf(fp, "{\"cnpj\":33333333000103,\"nome\":\"Aprazivel\",\"endereco\":\"R. Aprazivel, 62\"}\n");
    fclose(fp);

    /* Carregar em banco novo */
    AppDados *db = alocarBanco();
    int ret = carregarRestaurantes(db);

    VERIFICAR(
        "CARGA: carregarRestaurantes retorna 0 (sucesso)",
        ret == 0
    );
    VERIFICAR(
        "CARGA: banco deve ter 3 restaurantes apos carregar",
        db->nRestaurantes == 3
    );
    VERIFICAR(
        "CARGA: CNPJ do primeiro restaurante correto",
        db->restaurantes[0].cnpj == 11111111000101LL
    );
    VERIFICAR(
        "CARGA: nome do primeiro restaurante correto",
        strcmp(db->restaurantes[0].nome, "Toca da Traira - Barra") == 0
    );
    VERIFICAR(
        "CARGA: endereco do primeiro restaurante correto",
        strcmp(db->restaurantes[0].endereco, "Av. Barra, 100") == 0
    );

    /* Busca deve funcionar com dados carregados */
    Restaurante resultado[10];
    VERIFICAR(
        "CARGA: getListaRest funciona apos carga do JSON",
        getListaRest(db, "Toca", resultado, 10) == 1
    );

    free(db);

    /* Arquivo nao deve ter sido alterado (dados fixos) */
    FILE *fp2 = fopen(RESTAURANTE_JSON, "r");
    assert(fp2 != NULL);
    char linha[300];
    int linhas = 0;
    while (fgets(linha, sizeof(linha), fp2)) linhas++;
    fclose(fp2);
    VERIFICAR(
        "CARGA: arquivo JSON continua com 3 linhas (nao foi alterado)",
        linhas == 3
    );

    /* Parametros invalidos */
    VERIFICAR(
        "CARGA: carregarRestaurantes com db=NULL retorna -1",
        carregarRestaurantes(NULL) == -1
    );

    remove(RESTAURANTE_JSON);
}

/* ---------------------------------------------------------------
 * MAIN
 * --------------------------------------------------------------- */
int main(void) {
    printf("TESTES - Modulo Restaurante\n");

    suite_getListaRest();
    suite_getFeedRest();
    suite_getMenuRestaurante();
    suite_carregarRestaurantes();

    printf("  RESULTADO: %d/%d testes passaram\n",
           testesPassou, totalTestes);

    return EXIT_SUCCESS;
}