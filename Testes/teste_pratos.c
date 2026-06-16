/*
 * teste_pratos.c
 * Testes unitarios - Modulo Pratos (Foodies)
 *
 * Cobre: carregarPratos, getPratos, getListaPratos, getMenu, getFeedPratos
 * Casos de teste conforme PDF 5.6
 *
 * NOTA: pratos.json e somente leitura — nao ha teste de salvar.
 * O teste de carga cria o arquivo manualmente, carrega e verifica.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../pratos/pratos.h"

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
 * popularPratos
 *   Insere 25 pratos diretamente na memoria (sem arquivo).
 *   Usado pelas suites de getPratos, getListaPratos, getMenu, getFeedPratos.
 */
static void popularPratos(AppDados *db) {
    struct { long long int cnpj; const char *nome; const char *desc; } pratos[] = {
        { 11111111000101LL, "Frango Grelhado",    "Frango grelhado com ervas"    },
        { 11111111000101LL, "Frango Frito",       "Frango crocante frito"        },
        { 11111111000101LL, "Salada Caesar",      "Salada com croutons"          },
        { 11111111000101LL, "Picanha",            "Picanha ao carvao"            },
        { 11111111000101LL, "Costela",            "Costela assada lenta"         },
        { 22222222000102LL, "Soba Tradicional",   "Macarrao japones"             },
        { 22222222000102LL, "Gyoza",              "Pastel japones grelhado"      },
        { 22222222000102LL, "Temaki Salmao",      "Cone de alga com salmao"      },
        { 22222222000102LL, "Yakisoba",           "Macarrao frito com legumes"   },
        { 22222222000102LL, "Missoshiru",         "Sopa de miso"                 },
        { 33333333000103LL, "Peixe Frito",        "Peixe fresco frito"           },
        { 33333333000103LL, "Camarao Grelhado",   "Camarao grelhado com alho"    },
        { 33333333000103LL, "Moqueca de Peixe",   "Moqueca baiana"               },
        { 33333333000103LL, "Caldeirada",         "Caldeirada de frutos do mar"  },
        { 33333333000103LL, "Wrap de Frango",     "Wrap com frango e salada"     },
        { 11111111000101LL, "X-Burguer",          "Hamburguer artesanal"         },
        { 11111111000101LL, "Batata Frita",       "Batatas crocantes fritas"     },
        { 22222222000102LL, "Sashimi",            "Fatias de peixe cru"          },
        { 22222222000102LL, "Edamame",            "Vagem de soja cozida"         },
        { 33333333000103LL, "Lagosta Grelhada",   "Lagosta grelhada na manteiga" },
        { 11111111000101LL, "Fraldinha",          "Fraldinha com chimichurri"    },
        { 22222222000102LL, "Katsu Don",          "Porco empanado com ovo"       },
        { 33333333000103LL, "Siri Mole",          "Siri mole frito"              },
        { 11111111000101LL, "Linguica Calabresa", "Calabresa acebolada"          },
        { 33333333000103LL, "Polvo Grelhado",     "Polvo grelhado com azeite"    },
    };
    int n = (int)(sizeof(pratos) / sizeof(pratos[0]));
    for (int i = 0; i < n; i++) {
        db->pratos[i].idPrato         = db->proximoIdPrato++;
        db->pratos[i].cnpjRestaurante = pratos[i].cnpj;
        strncpy(db->pratos[i].nome,      pratos[i].nome, TAM_NOME       - 1);
        strncpy(db->pratos[i].descricao, pratos[i].desc, TAM_COMENTARIO - 1);
    }
    db->nPratos = n;
}

/* ---------------------------------------------------------------
 * SUITE 1 - getPratos
 * Ref. PDF 5.6 getPratos
 * --------------------------------------------------------------- */
static void suite_getPratos(void) {
    printf("\n=== SUITE getPratos ===\n");

    AppDados *db = alocarBanco();
    popularPratos(db);

    /* C1 - ID existente
     * Retorno esperado: ponteiro valido (dici_prato) */
    Prato *p = getPratos(db, 1);
    VERIFICAR(
        "C1: getPratos com idPrato=1 retorna ponteiro valido",
        p != NULL
    );
    VERIFICAR(
        "C1: idPrato do resultado confere",
        p != NULL && p->idPrato == 1
    );
    VERIFICAR(
        "C1: nome do resultado confere",
        p != NULL && strcmp(p->nome, "Frango Grelhado") == 0
    );

    /* C2 - ID nao existente
     * Retorno esperado: NULL */
    VERIFICAR(
        "C2: getPratos com idPrato inexistente => NULL",
        getPratos(db, 9999) == NULL
    );

    /* C3 - Parametros invalidos
     * Retorno esperado: NULL */
    VERIFICAR(
        "C3a: getPratos com db=NULL => NULL",
        getPratos(NULL, 1) == NULL
    );
    VERIFICAR(
        "C3b: getPratos com idPrato=0 => NULL",
        getPratos(db, 0) == NULL
    );
    VERIFICAR(
        "C3c: getPratos com idPrato negativo => NULL",
        getPratos(db, -1) == NULL
    );

    free(db);
}

/* ---------------------------------------------------------------
 * SUITE 2 - getListaPratos
 * Ref. PDF 5.6 getListaPratos
 * --------------------------------------------------------------- */
static void suite_getListaPratos(void) {
    printf("\n=== SUITE getListaPratos ===\n");

    AppDados *db = alocarBanco();
    popularPratos(db);
    Prato resultado[100];

    /* C1 - Nome existente: "Frango" encontra 3
     * Retorno esperado: 3 (lista_pratos) */
    int r1 = getListaPratos(db, "Frango", resultado, 100);
    VERIFICAR(
        "C1: getListaPratos('Frango') deve encontrar 3 pratos",
        r1 == 3
    );

    /* C1b - Busca case-insensitive */
    VERIFICAR(
        "C1b: getListaPratos('frango') minusculo deve encontrar 3",
        getListaPratos(db, "frango", resultado, 100) == 3
    );

    /* C1c - Nome exato */
    VERIFICAR(
        "C1c: getListaPratos('Picanha') deve encontrar 1",
        getListaPratos(db, "Picanha", resultado, 100) == 1
    );

    /* C1d - Nome inexistente */
    VERIFICAR(
        "C1d: getListaPratos('Pizza') nao encontra nada => 0",
        getListaPratos(db, "Pizza", resultado, 100) == 0
    );

    /* C2 - Nome nulo
     * Retorno esperado: PRATOS_NOME_INVALIDO (-1) */
    VERIFICAR(
        "C2: getListaPratos com nome=NULL => PRATOS_NOME_INVALIDO",
        getListaPratos(db, NULL, resultado, 100) == PRATOS_NOME_INVALIDO
    );

    /* C3 - Nome vazio (PDF 2.4: nada deve aparecer)
     * Retorno esperado: 0 (lista_vazia) */
    VERIFICAR(
        "C3: getListaPratos com nome='' => 0 (lista_vazia)",
        getListaPratos(db, "", resultado, 100) == 0
    );
    VERIFICAR(
        "C3b: getListaPratos com resultado=NULL => PRATOS_NOME_INVALIDO",
        getListaPratos(db, "Frango", NULL, 100) == PRATOS_NOME_INVALIDO
    );
    VERIFICAR(
        "C3c: getListaPratos com db=NULL => PRATOS_NOME_INVALIDO",
        getListaPratos(NULL, "Frango", resultado, 100) == PRATOS_NOME_INVALIDO
    );
    VERIFICAR(
        "C3d: getListaPratos com maxResultados=0 => PRATOS_NOME_INVALIDO",
        getListaPratos(db, "Frango", resultado, 0) == PRATOS_NOME_INVALIDO
    );

    /* C4 - maxResultados limita retorno */
    VERIFICAR(
        "C4: getListaPratos com maxResultados=1 retorna no maximo 1",
        getListaPratos(db, "Frango", resultado, 1) == 1
    );

    free(db);
}

/* ---------------------------------------------------------------
 * SUITE 3 - getMenu
 * Ref. PDF 5.6 getMenu
 * --------------------------------------------------------------- */
static void suite_getMenu(void) {
    printf("\n=== SUITE getMenu ===\n");

    AppDados *db = alocarBanco();
    popularPratos(db);
    Prato resultado[100];

    /* C1 - CNPJ com 9 pratos
     * Retorno esperado: 9 (lista_pratos) */
    int r1 = getMenu(db, 11111111000101LL, resultado, 100);
    VERIFICAR(
        "C1: getMenu CNPJ com pratos deve retornar 9",
        r1 == 9
    );
    VERIFICAR(
        "C1: todos os pratos pertencem ao CNPJ correto",
        resultado[0].cnpjRestaurante == 11111111000101LL
    );

    /* C2 - CNPJ sem pratos
     * Retorno esperado: 0 */
    VERIFICAR(
        "C2: getMenu com CNPJ sem pratos => 0",
        getMenu(db, 99999999000199LL, resultado, 100) == 0
    );

    /* C3 - Parametros invalidos
     * Retorno esperado: PRATOS_PARAM_INVALIDO (-2) */
    VERIFICAR(
        "C3a: getMenu com db=NULL => PRATOS_PARAM_INVALIDO",
        getMenu(NULL, 11111111000101LL, resultado, 100) == PRATOS_PARAM_INVALIDO
    );
    VERIFICAR(
        "C3b: getMenu com cnpj=0 => PRATOS_PARAM_INVALIDO",
        getMenu(db, 0LL, resultado, 100) == PRATOS_PARAM_INVALIDO
    );
    VERIFICAR(
        "C3c: getMenu com resultado=NULL => PRATOS_PARAM_INVALIDO",
        getMenu(db, 11111111000101LL, NULL, 100) == PRATOS_PARAM_INVALIDO
    );
    VERIFICAR(
        "C3d: getMenu com maxResultados=0 => PRATOS_PARAM_INVALIDO",
        getMenu(db, 11111111000101LL, resultado, 0) == PRATOS_PARAM_INVALIDO
    );

    free(db);
}

/* ---------------------------------------------------------------
 * SUITE 4 - getFeedPratos
 * Ref. PDF 5.6 getFeedPratos
 * --------------------------------------------------------------- */
static void suite_getFeedPratos(void) {
    printf("\n=== SUITE getFeedPratos ===\n");

    Prato resultado[50];
    long long int cpfTeste = 12345678901LL;

    /* C1 - Banco com 25 pratos, sem avaliacoes
     * Retorno esperado: 20 (lista_pratos) */
    AppDados *db = alocarBanco();
    popularPratos(db);

    int r1 = getFeedPratos(db, cpfTeste, resultado, 50);
    VERIFICAR(
        "C1: getFeedPratos com banco suficiente deve retornar 20",
        r1 == PRATOS_FEED_QTD
    );

    int repeticao = 0;
    for (int i = 0; i < r1; i++)
        for (int j = i + 1; j < r1; j++)
            if (resultado[i].idPrato == resultado[j].idPrato) repeticao = 1;
    VERIFICAR(
        "C1: resultado nao deve conter pratos repetidos",
        repeticao == 0
    );

    /* C1b - Pratos ja avaliados nao aparecem (PDF 2.7) */
    for (int i = 0; i < 5; i++) {
        db->avaliacoes[i].idAval  = i + 1;
        db->avaliacoes[i].cpf     = cpfTeste;
        db->avaliacoes[i].idPrato = i + 1;
        db->avaliacoes[i].nota    = 4.0f;
    }
    db->nAvaliacoes = 5;

    int r1b = getFeedPratos(db, cpfTeste, resultado, 50);
    VERIFICAR(
        "C1b: com 5 avaliados e 25 total deve retornar 20 elegiveis",
        r1b == PRATOS_FEED_QTD
    );

    int contemAvaliado = 0;
    for (int i = 0; i < r1b; i++)
        if (resultado[i].idPrato >= 1 && resultado[i].idPrato <= 5)
            contemAvaliado = 1;
    VERIFICAR(
        "C1b: feed nao deve conter pratos ja avaliados",
        contemAvaliado == 0
    );
    free(db);

    /* C2 - Banco insuficiente
     * Retorno esperado: PRATOS_INSUFICIENTE (-4) */
    AppDados *dbP = alocarBanco();
    for (int i = 0; i < 5; i++) {
        dbP->pratos[i].idPrato = dbP->proximoIdPrato++;
        snprintf(dbP->pratos[i].nome, TAM_NOME, "Prato %d", i + 1);
    }
    dbP->nPratos = 5;
    VERIFICAR(
        "C2: getFeedPratos com 5 pratos => PRATOS_INSUFICIENTE",
        getFeedPratos(dbP, cpfTeste, resultado, 50) == PRATOS_INSUFICIENTE
    );
    free(dbP);

    /* C2b - Todos os pratos avaliados */
    AppDados *dbTudo = alocarBanco();
    popularPratos(dbTudo);
    for (int i = 0; i < 25; i++) {
        dbTudo->avaliacoes[i].idAval  = i + 1;
        dbTudo->avaliacoes[i].cpf     = cpfTeste;
        dbTudo->avaliacoes[i].idPrato = i + 1;
        dbTudo->avaliacoes[i].nota    = 5.0f;
    }
    dbTudo->nAvaliacoes = 25;
    VERIFICAR(
        "C2b: com todos os 25 pratos avaliados => PRATOS_INSUFICIENTE",
        getFeedPratos(dbTudo, cpfTeste, resultado, 50) == PRATOS_INSUFICIENTE
    );
    free(dbTudo);

    /* C3 - Parametros invalidos */
    AppDados *dbV = alocarBanco();
    popularPratos(dbV);
    VERIFICAR(
        "C3a: getFeedPratos com db=NULL => PRATOS_PARAM_INVALIDO",
        getFeedPratos(NULL, cpfTeste, resultado, 50) == PRATOS_PARAM_INVALIDO
    );
    VERIFICAR(
        "C3b: getFeedPratos com resultado=NULL => PRATOS_PARAM_INVALIDO",
        getFeedPratos(dbV, cpfTeste, NULL, 50) == PRATOS_PARAM_INVALIDO
    );
    VERIFICAR(
        "C3c: getFeedPratos com cpf=0 => PRATOS_PARAM_INVALIDO",
        getFeedPratos(dbV, 0LL, resultado, 50) == PRATOS_PARAM_INVALIDO
    );
    free(dbV);
}

/* ---------------------------------------------------------------
 * SUITE 5 - carregarPratos
 * Testa a carga a partir de um JSON criado manualmente.
 * Confirma que os dados fixos sao lidos corretamente no inicio.
 * --------------------------------------------------------------- */
static void suite_carregarPratos(void) {
    printf("\n=== SUITE carregarPratos ===\n");

    /* Criar arquivo JSON de teste com 3 pratos */
    FILE *fp = fopen(PRATOS_JSON, "w");
    assert(fp != NULL);
    fprintf(fp, "{\"idPrato\":1,\"nome\":\"Frango Grelhado\",\"descricao\":\"Grelhado com ervas\",\"cnpj\":11111111000101,\"endereco\":\"Av. Barra, 100\"}\n");
    fprintf(fp, "{\"idPrato\":2,\"nome\":\"Soba Tradicional\",\"descricao\":\"Macarrao japones\",\"cnpj\":22222222000102,\"endereco\":\"R. Bambina, 124\"}\n");
    fprintf(fp, "{\"idPrato\":3,\"nome\":\"Picanha\",\"descricao\":\"Picanha ao carvao\",\"cnpj\":33333333000103,\"endereco\":\"R. Aprazivel, 62\"}\n");
    fclose(fp);

    /* Carregar em banco novo */
    AppDados *db = alocarBanco();
    int ret = carregarPratos(db);

    VERIFICAR(
        "CARGA: carregarPratos retorna 0 (sucesso)",
        ret == 0
    );
    VERIFICAR(
        "CARGA: banco deve ter 3 pratos apos carregar",
        db->nPratos == 3
    );

    /* Verificar integridade dos dados */
    Prato *p = getPratos(db, 1);
    VERIFICAR(
        "CARGA: prato idPrato=1 recuperado corretamente",
        p != NULL && p->idPrato == 1
    );
    VERIFICAR(
        "CARGA: nome do prato 1 correto",
        p != NULL && strcmp(p->nome, "Frango Grelhado") == 0
    );
    VERIFICAR(
        "CARGA: CNPJ do prato 1 correto",
        p != NULL && p->cnpjRestaurante == 11111111000101LL
    );

    /* Busca deve funcionar com dados carregados */
    Prato resultado[10];
    VERIFICAR(
        "CARGA: getListaPratos funciona apos carga do JSON",
        getListaPratos(db, "Frango", resultado, 10) == 1
    );

    free(db);

    /* Arquivo nao deve ter sido alterado (dados fixos) */
    FILE *fp2 = fopen(PRATOS_JSON, "r");
    assert(fp2 != NULL);
    char linha[800];
    int linhas = 0;
    while (fgets(linha, sizeof(linha), fp2)) linhas++;
    fclose(fp2);
    VERIFICAR(
        "CARGA: arquivo JSON continua com 3 linhas (nao foi alterado)",
        linhas == 3
    );

    /* Parametros invalidos */
    VERIFICAR(
        "CARGA: carregarPratos com db=NULL retorna -1",
        carregarPratos(NULL) == -1
    );

    remove(PRATOS_JSON);
}

/* ---------------------------------------------------------------
 * MAIN
 * --------------------------------------------------------------- */
int main(void) {
    printf("TESTES - Modulo Pratos\n");

    suite_getPratos();
    suite_getListaPratos();
    suite_getMenu();
    suite_getFeedPratos();
    suite_carregarPratos();

    printf("  RESULTADO: %d/%d testes passaram\n",
           testesPassou, totalTestes);

    return EXIT_SUCCESS;
}