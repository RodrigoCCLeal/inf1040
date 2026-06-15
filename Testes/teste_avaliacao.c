/*
 * teste_avaliacao.c
 * Testes unitarios - Modulo Avaliacao (Foodies)
 *
 * Cobre: postAval, verAval, editAval
 * Casos de teste conforme PDF 5.4
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../avaliacao/avaliacao.h"

/* ---------------------------------------------------------------
 * CONTADORES DE TESTE
 * --------------------------------------------------------------- */
static int totalTestes  = 0;
static int testesPassou = 0;

/*
 * VERIFICAR
 *   Imprime descricao, avalia com assert() e conta aprovacoes.
 *   Se falhar, assert() aborta exibindo arquivo e linha.
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
 * HELPER
 *   AppDados tem ~3.6 MB - aloca no heap para evitar stack overflow
 *   no Windows. O chamador deve liberar com free().
 * --------------------------------------------------------------- */
static AppDados *alocarBanco(void) {
    AppDados *db = malloc(sizeof(AppDados));
    assert(db != NULL);
    memset(db, 0, sizeof(AppDados));
    db->proximoIdPrato = 1;
    db->proximoIdAval  = 1;
    db->cpfLogado      = 0;

    /* Prato idPrato=1 */
    db->pratos[0].idPrato         = db->proximoIdPrato++;
    db->pratos[0].cnpjRestaurante = 11222333000181LL;
    strncpy(db->pratos[0].nome, "Frango Grelhado", TAM_NOME - 1);

    /* Prato idPrato=2 */
    db->pratos[1].idPrato         = db->proximoIdPrato++;
    db->pratos[1].cnpjRestaurante = 11222333000181LL;
    strncpy(db->pratos[1].nome, "Salada Caesar", TAM_NOME - 1);

    db->nPratos = 2;
    return db;
}

/* ---------------------------------------------------------------
 * SUITE 1 - postAval
 * Ref. PDF 5.4
 *
 * Retornos esperados:
 *   0  - Postado com sucesso com comentario    (AVAL_OK)
 *   1  - Postado com sucesso sem comentario    (AVAL_OK_SEM_TXT)
 *   2  - Falha: idPrato inexistente            (AVAL_ERRO_ID_PRATO)
 *   4  - Falha: nota invalida                  (AVAL_ERRO_NOTA)
 *  -1  - Parametro invalido                    (AVAL_PARAM_INVALIDO)
 * --------------------------------------------------------------- */
static void suite_postAval(void) {
    printf("\n=== SUITE postAval ===\n");

    AppDados *db = alocarBanco();

    /* C1 - CPF valido, idPrato existente, comentario e nota
     * Retorno esperado: AVAL_OK (0) */
    VERIFICAR(
        "C1: post com CPF valido, idPrato, txt e nota => AVAL_OK",
        postAval(db, 12345678901LL, 1, "Excelente!", 5.0f) == AVAL_OK
    );
    VERIFICAR(
        "C1: banco deve ter 1 avaliacao",
        db->nAvaliacoes == 1
    );
    VERIFICAR(
        "C1: nota gravada deve ser 5",
        (int)db->avaliacoes[0].nota == 5
    );

    /* C2 - Sem comentario (NULL)
     * Retorno esperado: AVAL_OK_SEM_TXT (1) */
    VERIFICAR(
        "C2: post com txt=NULL => AVAL_OK_SEM_TXT",
        postAval(db, 12345678901LL, 2, NULL, 3.0f) == AVAL_OK_SEM_TXT
    );

    /* C2b - String vazia tambem e sem comentario */
    VERIFICAR(
        "C2b: post com txt='' => AVAL_OK_SEM_TXT",
        postAval(db, 12345678901LL, 2, "", 4.0f) == AVAL_OK_SEM_TXT
    );

    /* C3 - idPrato inexistente no banco
     * Retorno esperado: AVAL_ERRO_ID_PRATO (2) */
    VERIFICAR(
        "C3: post com idPrato inexistente => AVAL_ERRO_ID_PRATO",
        postAval(db, 12345678901LL, 999, "txt", 4.0f) == AVAL_ERRO_ID_PRATO
    );

    /* C3b - idPrato=0 e parametro invalido */
    VERIFICAR(
        "C3b: post com idPrato=0 => AVAL_PARAM_INVALIDO",
        postAval(db, 12345678901LL, 0, "txt", 4.0f) == AVAL_PARAM_INVALIDO
    );

    /* C3c - idPrato negativo */
    VERIFICAR(
        "C3c: post com idPrato negativo => AVAL_PARAM_INVALIDO",
        postAval(db, 12345678901LL, -5, "txt", 4.0f) == AVAL_PARAM_INVALIDO
    );

    /* C4 - nota fora do limite [0,5]
     * Retorno esperado: AVAL_ERRO_NOTA (4) */
    VERIFICAR(
        "C4a: post com nota=6 => AVAL_ERRO_NOTA",
        postAval(db, 12345678901LL, 1, "txt", 6.0f) == AVAL_ERRO_NOTA
    );
    VERIFICAR(
        "C4b: post com nota=-1 => AVAL_ERRO_NOTA",
        postAval(db, 12345678901LL, 1, "txt", -1.0f) == AVAL_ERRO_NOTA
    );
    VERIFICAR(
        "C4c: post com nota fracionada 3.5 => AVAL_ERRO_NOTA",
        postAval(db, 12345678901LL, 1, "txt", 3.5f) == AVAL_ERRO_NOTA
    );

    /* C5 - CPF invalido
     * Retorno esperado: AVAL_PARAM_INVALIDO (-1) */
    VERIFICAR(
        "C5a: post com CPF=0 => AVAL_PARAM_INVALIDO",
        postAval(db, 0LL, 1, "txt", 3.0f) == AVAL_PARAM_INVALIDO
    );
    VERIFICAR(
        "C5b: post com CPF negativo => AVAL_PARAM_INVALIDO",
        postAval(db, -1LL, 1, "txt", 3.0f) == AVAL_PARAM_INVALIDO
    );
    VERIFICAR(
        "C5c: post com db=NULL => AVAL_PARAM_INVALIDO",
        postAval(NULL, 12345678901LL, 1, "txt", 3.0f) == AVAL_PARAM_INVALIDO
    );

    free(db);
}

/* ---------------------------------------------------------------
 * SUITE 2 - verAval
 * Ref. PDF 5.4
 *
 * Retornos esperados:
 *   >= 0  - quantidade de avaliacoes encontradas (lista_aval)
 *    0    - prato sem avaliacoes                 (lista_vazia)
 *   -1    - parametro invalido                   (AVAL_PARAM_INVALIDO)
 * --------------------------------------------------------------- */
static void suite_verAval(void) {
    printf("\n=== SUITE verAval ===\n");

    AppDados *db = alocarBanco();

    /* Pre-condicao: inserir avaliacoes */
    postAval(db, 11111111111LL, 1, "Otimo!", 5.0f);
    postAval(db, 22222222222LL, 1, "Bom",   4.0f);
    postAval(db, 33333333333LL, 2, "Ok",    3.0f);

    Avaliacao resultado[50];

    /* C1 - prato com avaliacoes existentes
     * Retorno esperado: 2 */
    int r1 = verAval(db, 1, resultado, 50);
    VERIFICAR(
        "C1: verAval prato 1 deve retornar 2 avaliacoes",
        r1 == 2
    );
    VERIFICAR(
        "C1: nota da primeira avaliacao deve ser 5",
        (int)resultado[0].nota == 5
    );
    VERIFICAR(
        "C1: nota da segunda avaliacao deve ser 4",
        (int)resultado[1].nota == 4
    );

    /* C2 - prato existente sem avaliacoes
     * Retorno esperado: 0 (lista_vazia) */
    db->pratos[db->nPratos].idPrato = db->proximoIdPrato++;
    strncpy(db->pratos[db->nPratos].nome, "Prato Novo", TAM_NOME - 1);
    db->nPratos++;
    int idSemAval = db->pratos[db->nPratos - 1].idPrato;

    VERIFICAR(
        "C2: verAval prato sem avaliacoes => 0 (lista_vazia)",
        verAval(db, idSemAval, resultado, 50) == 0
    );

    /* C3 - parametro invalido
     * Retorno esperado: AVAL_PARAM_INVALIDO (-1) */
    VERIFICAR(
        "C3a: verAval com idPrato=0 => AVAL_PARAM_INVALIDO",
        verAval(db, 0, resultado, 50) == AVAL_PARAM_INVALIDO
    );
    VERIFICAR(
        "C3b: verAval com idPrato negativo => AVAL_PARAM_INVALIDO",
        verAval(db, -1, resultado, 50) == AVAL_PARAM_INVALIDO
    );
    VERIFICAR(
        "C3c: verAval com resultado=NULL => AVAL_PARAM_INVALIDO",
        verAval(db, 1, NULL, 50) == AVAL_PARAM_INVALIDO
    );
    VERIFICAR(
        "C3d: verAval com maxResultados=0 => AVAL_PARAM_INVALIDO",
        verAval(db, 1, resultado, 0) == AVAL_PARAM_INVALIDO
    );
    VERIFICAR(
        "C3e: verAval com db=NULL => AVAL_PARAM_INVALIDO",
        verAval(NULL, 1, resultado, 50) == AVAL_PARAM_INVALIDO
    );

    /* C4 - maxResultados limita retorno */
    VERIFICAR(
        "C4: verAval com maxResultados=1 retorna 1",
        verAval(db, 1, resultado, 1) == 1
    );

    free(db);
}

/* ---------------------------------------------------------------
 * SUITE 3 - editAval
 * Ref. PDF 2.6
 *
 * Retornos esperados:
 *    0  - edicao aplicada           (AVAL_OK)
 *   -2  - avaliacao nao encontrada  (AVAL_ERRO_EDICAO)
 *   -3  - CPF nao e o autor         (AVAL_ERRO_PERMISSAO)
 *    4  - nova nota invalida         (AVAL_ERRO_NOTA)
 *   -1  - parametro invalido         (AVAL_PARAM_INVALIDO)
 * --------------------------------------------------------------- */
static void suite_editAval(void) {
    printf("\n=== SUITE editAval ===\n");

    AppDados *db = alocarBanco();

    /* Pre-condicao: inserir avaliacao para editar */
    postAval(db, 11111111111LL, 1, "Comentario original", 3.0f);
    int idAvalCriado = db->avaliacoes[0].idAval;

    /* C1 - dono edita sua avaliacao com dados validos
     * Retorno esperado: AVAL_OK (0) */
    VERIFICAR(
        "C1: editar avaliacao propria => AVAL_OK",
        editAval(db, idAvalCriado, 11111111111LL, "Editado", 4.0f) == AVAL_OK
    );
    VERIFICAR(
        "C1: nota apos edicao deve ser 4",
        (int)db->avaliacoes[0].nota == 4
    );
    VERIFICAR(
        "C1: comentario apos edicao deve ser o novo texto",
        strcmp(db->avaliacoes[0].comentario, "Editado") == 0
    );

    /* C2 - CPF diferente do autor
     * Retorno esperado: AVAL_ERRO_PERMISSAO (-3) */
    VERIFICAR(
        "C2: editar avaliacao de outro usuario => AVAL_ERRO_PERMISSAO",
        editAval(db, idAvalCriado, 99999999999LL, "Invasao", 1.0f)
            == AVAL_ERRO_PERMISSAO
    );

    /* C3 - idAval inexistente
     * Retorno esperado: AVAL_ERRO_EDICAO (-2) */
    VERIFICAR(
        "C3: editar com idAval inexistente => AVAL_ERRO_EDICAO",
        editAval(db, 9999, 11111111111LL, "txt", 2.0f) == AVAL_ERRO_EDICAO
    );

    /* C4 - nova nota invalida
     * Retorno esperado: AVAL_ERRO_NOTA (4) */
    VERIFICAR(
        "C4a: editar com nota=7 => AVAL_ERRO_NOTA",
        editAval(db, idAvalCriado, 11111111111LL, NULL, 7.0f) == AVAL_ERRO_NOTA
    );
    VERIFICAR(
        "C4b: editar com nota fracionada 2.5 => AVAL_ERRO_NOTA",
        editAval(db, idAvalCriado, 11111111111LL, NULL, 2.5f) == AVAL_ERRO_NOTA
    );

    /* C5 - parametros invalidos
     * Retorno esperado: AVAL_PARAM_INVALIDO (-1) */
    VERIFICAR(
        "C5a: editar com idAval=0 => AVAL_PARAM_INVALIDO",
        editAval(db, 0, 11111111111LL, "txt", 3.0f) == AVAL_PARAM_INVALIDO
    );
    VERIFICAR(
        "C5b: editar com cpf=0 => AVAL_PARAM_INVALIDO",
        editAval(db, idAvalCriado, 0LL, "txt", 3.0f) == AVAL_PARAM_INVALIDO
    );
    VERIFICAR(
        "C5c: editar com db=NULL => AVAL_PARAM_INVALIDO",
        editAval(NULL, idAvalCriado, 11111111111LL, "txt", 3.0f)
            == AVAL_PARAM_INVALIDO
    );

    /* C6 - editar so comentario (novaNota=-1 mantem anterior)
     * Retorno esperado: AVAL_OK (0) */
    float notaAntes = db->avaliacoes[0].nota;
    VERIFICAR(
        "C6: editar so comentario (novaNota=-1) => AVAL_OK",
        editAval(db, idAvalCriado, 11111111111LL, "So txt", -1.0f) == AVAL_OK
    );
    VERIFICAR(
        "C6: nota deve permanecer inalterada",
        db->avaliacoes[0].nota == notaAntes
    );

    /* C7 - editar so nota (novaTxt=NULL mantem anterior)
     * Retorno esperado: AVAL_OK (0) */
    char txtAntes[TAM_COMENTARIO];
    strncpy(txtAntes, db->avaliacoes[0].comentario, TAM_COMENTARIO - 1);
    VERIFICAR(
        "C7: editar so nota (novaTxt=NULL) => AVAL_OK",
        editAval(db, idAvalCriado, 11111111111LL, NULL, 2.0f) == AVAL_OK
    );
    VERIFICAR(
        "C7: comentario deve permanecer inalterado",
        strcmp(db->avaliacoes[0].comentario, txtAntes) == 0
    );
    VERIFICAR(
        "C7: nota deve ter sido atualizada para 2",
        (int)db->avaliacoes[0].nota == 2
    );

    free(db);
}

/* ---------------------------------------------------------------
 * MAIN
 * --------------------------------------------------------------- */
int main(void) {
    printf("TESTES - Modulo Avaliacao\n");

    suite_postAval();
    suite_verAval();
    suite_editAval();

    printf("  RESULTADO: %d/%d testes passaram\n",
           testesPassou, totalTestes);

    return EXIT_SUCCESS;
}