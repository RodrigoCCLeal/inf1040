/*
 * principal.c
 * Modulo Principal - Foodies
 *
 * Implementacao das funcoes declaradas em principal.h.
 *
 * Este modulo e o UNICO ponto de abertura e fechamento de arquivos JSON.
 * Fluxo garantido:
 *   iniciarApp()  -> aloca db -> carregarPerfis(db) -> carregarAvaliacoes(db)
 *   encerrarApp() -> salvarPerfis(db) -> salvarAvaliacoes(db) -> free(db)
 *
 * Formato do JSON de avaliacoes (uma linha por registro):
 *   {"idAval":1,"nota":5,"comentario":"Otimo","cpf":12345678901,"idPrato":1}
 */

#include "principal.h"
#include "../perfil/perfil.h"
#include "../restaurante/restaurante.h"
#include "../pratos/pratos.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* =================================================================
 * FUNCOES AUXILIARES PRIVADAS (static)
 * ================================================================= */

/*
 * escaparString
 *   Copia src para dst escapando aspas duplas para JSON valido.
 *   Utilizada por salvarAvaliacoes para o campo comentario.
 */
static void escaparString(char *dst, const char *src, int maxDst) {
    int j = 0;
    for (int i = 0; src[i] != '\0' && j < maxDst - 2; i++) {
        if (src[i] == '"' || src[i] == '\\') {
            dst[j++] = '\\';
        }
        dst[j++] = src[i];
    }
    dst[j] = '\0';
}

/* =================================================================
 * FUNCOES DE I/O DO JSON DE AVALIACOES
 * ================================================================= */

/*
 * carregarAvaliacoes
 *   Le avaliacoes.json e popula db->avaliacoes[].
 *   Tambem recalcula db->proximoIdAval para continuar a sequencia.
 *   Ver contrato em principal.h.
 */
int carregarAvaliacoes(AppDados *db) {
    if (db == NULL) return -1;

    db->nAvaliacoes   = 0;
    db->proximoIdAval = 1;

    FILE *fp = fopen(AVALIACOES_JSON, "r");
    if (fp == NULL) {
        /* Arquivo ainda nao existe - primeira execucao, sem erro */
        return 0;
    }

    char linha[600];
    while (fgets(linha, sizeof(linha), fp) &&
           db->nAvaliacoes < MAX_AVALIACOES) {

        Avaliacao *a = &db->avaliacoes[db->nAvaliacoes];
        char comentarioBuf[TAM_COMENTARIO] = "";
        float nota = 0.0f;

        /*
         * Tentar parsing com comentario primeiro:
         *   {"idAval":N,"nota":N,"comentario":"...","cpf":N,"idPrato":N}
         */
        int lidos = sscanf(linha,
            " {\"idAval\":%d,\"nota\":%f,\"comentario\":\"%400[^\"]\","
            "\"cpf\":%lld,\"idPrato\":%d}",
            &a->idAval, &nota, comentarioBuf, &a->cpf, &a->idPrato);

        if (lidos < 4) {
            /* Tentar sem comentario (campo vazio): */
            int lidosSemTxt = sscanf(linha,
                " {\"idAval\":%d,\"nota\":%f,\"comentario\":\"\","
                "\"cpf\":%lld,\"idPrato\":%d}",
                &a->idAval, &nota, &a->cpf, &a->idPrato);

            if (lidosSemTxt < 4) continue;  /* linha invalida, pula */
            comentarioBuf[0] = '\0';
        }

        a->nota = nota;
        strncpy(a->comentario, comentarioBuf, TAM_COMENTARIO - 1);
        a->comentario[TAM_COMENTARIO - 1] = '\0';

        /* Atualizar gerador de ID para continuar a sequencia */
        if (a->idAval >= db->proximoIdAval)
            db->proximoIdAval = a->idAval + 1;

        db->nAvaliacoes++;
    }

    fclose(fp);
    return 0;
}

/*
 * salvarAvaliacoes
 *   Serializa db->avaliacoes[] para avaliacoes.json.
 *   Ver contrato em principal.h.
 */
int salvarAvaliacoes(AppDados *db) {
    if (db == NULL) return -1;

    FILE *fp = fopen(AVALIACOES_JSON, "w");
    if (fp == NULL) return -1;

    char comentarioEsc[TAM_COMENTARIO * 2];

    for (int i = 0; i < db->nAvaliacoes; i++) {
        escaparString(comentarioEsc,
                      db->avaliacoes[i].comentario,
                      sizeof(comentarioEsc));

        fprintf(fp,
            "{\"idAval\":%d,\"nota\":%.1f,\"comentario\":\"%s\","
            "\"cpf\":%lld,\"idPrato\":%d}\n",
            db->avaliacoes[i].idAval,
            db->avaliacoes[i].nota,
            comentarioEsc,
            db->avaliacoes[i].cpf,
            db->avaliacoes[i].idPrato);
    }

    fclose(fp);
    return 0;
}

/* =================================================================
 * FUNCOES PUBLICAS DE CICLO DE VIDA DO APP
 * ================================================================= */

/*
 * iniciarApp
 *   Aloca o banco em memoria e carrega todos os JSONs.
 *   Deve ser a primeira chamada de qualquer programa que use o sistema.
 *   Ver contrato em principal.h.
 */
AppDados *iniciarApp(void) {

    /* Alocar banco no heap (AppDados ~3.6 MB, grande demais para pilha) */
    AppDados *db = malloc(sizeof(AppDados));
    if (db == NULL) return NULL;

    /* Zerar tudo antes de carregar */
    memset(db, 0, sizeof(AppDados));
    db->proximoIdPrato = 1;
    db->proximoIdAval  = 1;
    db->cpfLogado      = 0;

    /*
     * Carregar cada JSON pelo modulo responsavel.
     * A ordem nao importa pois os dados sao independentes,
     * mas perfis vem primeiro por convencao.
     */
    carregarPerfis(db);
    carregarRestaurantes(db);
    carregarPratos(db);
    carregarAvaliacoes(db);  /* modulo Principal le avaliacoes.json */

    return db;
}

/*
 * encerrarApp
 *   Salva todos os JSONs e libera a memoria alocada.
 *   Deve ser a ultima chamada antes de o programa terminar.
 *   Ver contrato em principal.h.
 */
void encerrarApp(AppDados *db) {
    if (db == NULL) return;

    /*
     * Salvar cada JSON pelo modulo responsavel.
     * Mesmo que algum salvar falhe, continuamos para liberar a memoria.
     */
    salvarPerfis(db);
    salvarAvaliacoes(db);  /* modulo Principal escreve avaliacoes.json */

    /* Liberar a memoria do banco */
    free(db);
}