/*
 * pratos.c
 * Modulo Pratos - Foodies
 *
 * Implementacao das funcoes declaradas em pratos.h.
 *
 * DADOS FIXOS: este modulo apenas LE pratos.json.
 * O arquivo nunca e reescrito — pratos sao imutaveis durante
 * toda a execucao do programa (usuarios nao podem adicionar ou remover).
 *
 * Formato do JSON (uma linha por registro):
 *   {"idPrato":1,"nome":"Frango Grelhado","descricao":"...","cnpj":11111111000101,"endereco":"Av. Barra"}
 */

#include "pratos.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

/* =================================================================
 * FUNCOES AUXILIARES PRIVADAS (static)
 * ================================================================= */

/*
 * contemSubstring
 *   Verifica se haystack contem needle como substring, case-insensitive.
 *   Utilizada por getListaPratos para busca parcial pelo nome.
 */
static int contemSubstring(const char *haystack, const char *needle) {
    if (needle[0] == '\0') return 0;
    for (int i = 0; haystack[i] != '\0'; i++) {
        int j = 0;
        while (needle[j] != '\0' &&
               tolower((unsigned char)haystack[i + j]) ==
               tolower((unsigned char)needle[j])) {
            j++;
        }
        if (needle[j] == '\0') return 1;
    }
    return 0;
}

/*
 * shuffleIndices
 *   Fisher-Yates shuffle sobre indices[] de tamanho n, in-place.
 *   Utilizada por getFeedPratos para selecao aleatoria sem repeticao.
 */
static void shuffleIndices(int *indices, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j      = rand() % (i + 1);
        int tmp    = indices[i];
        indices[i] = indices[j];
        indices[j] = tmp;
    }
}

/* Garante que srand() seja chamado apenas uma vez */
static int sementeInicializada = 0;

static void inicializarSemente(void) {
    if (!sementeInicializada) {
        srand((unsigned int)time(NULL));
        sementeInicializada = 1;
    }
}

/*
 * pratoJaAvaliado
 *   Verifica se o usuario (cpf) ja avaliou o prato (idPrato).
 *   Retorna 1 se sim, 0 se nao.
 *   Utilizada por getFeedPratos para filtrar pratos avaliados (PDF 2.7).
 */
static int pratoJaAvaliado(const AppDados *db, long long int cpf, int idPrato) {
    for (int i = 0; i < db->nAvaliacoes; i++) {
        if (db->avaliacoes[i].cpf     == cpf &&
            db->avaliacoes[i].idPrato == idPrato)
            return 1;
    }
    return 0;
}

/* =================================================================
 * FUNCAO DE CARGA DO JSON (chamada exclusivamente pelo Principal)
 * ================================================================= */

/*
 * carregarPratos
 *   Le pratos.json e popula db->pratos[].
 *   Chamada uma unica vez no inicio. O arquivo nao e alterado depois.
 *   Ver contrato em pratos.h.
 */
int carregarPratos(AppDados *db) {
    if (db == NULL) return -1;

    db->nPratos = 0;

    FILE *fp = fopen(PRATOS_JSON, "r");
    if (fp == NULL) return 0;  /* arquivo nao existe ainda, sem erro */

    char linha[800];
    while (fgets(linha, sizeof(linha), fp) &&
           db->nPratos < MAX_PRATOS) {

        Prato *p = &db->pratos[db->nPratos];
        char nomeBuf[TAM_NOME]       = "";
        char descBuf[TAM_COMENTARIO] = "";
        char endBuf [TAM_ENDERECO]   = "";

        int lidos = sscanf(linha,
            " {\"idPrato\":%d,\"nome\":\"%100[^\"]\","
            "\"descricao\":\"%400[^\"]\","
            "\"cnpj\":%lld,\"endereco\":\"%200[^\"]\"}",
            &p->idPrato, nomeBuf, descBuf,
            &p->cnpjRestaurante, endBuf);

        if (lidos < 4) continue;

        strncpy(p->nome,                nomeBuf, TAM_NOME       - 1);
        strncpy(p->descricao,           descBuf, TAM_COMENTARIO - 1);
        strncpy(p->enderecoRestaurante, endBuf,  TAM_ENDERECO   - 1);
        p->nome               [TAM_NOME       - 1] = '\0';
        p->descricao          [TAM_COMENTARIO - 1] = '\0';
        p->enderecoRestaurante[TAM_ENDERECO   - 1] = '\0';

        db->nPratos++;
    }

    fclose(fp);
    return 0;
}

/* =================================================================
 * FUNCOES DE ACESSO PUBLICAS (PDF 3.4.7)
 * ================================================================= */

/*
 * getPratos
 *   Retorna ponteiro para o prato com o idPrato dado.
 *   Ver contrato em pratos.h.
 */
Prato *getPratos(AppDados *db, int idPrato) {
    if (db == NULL || idPrato <= 0) return NULL;

    for (int i = 0; i < db->nPratos; i++) {
        if (db->pratos[i].idPrato == idPrato)
            return &db->pratos[i];
    }
    return NULL;
}

/*
 * getListaPratos
 *   Busca pratos por substring no nome (case-insensitive).
 *   Ver contrato em pratos.h.
 */
int getListaPratos(AppDados *db, const char *nome_prato,
                   Prato *resultado, int maxResultados) {

    if (db == NULL || nome_prato == NULL ||
        resultado == NULL || maxResultados <= 0)
        return PRATOS_NOME_INVALIDO;

    /* PDF 2.4: nome vazio nao retorna nada */
    if (nome_prato[0] == '\0') return 0;

    int encontrados = 0;
    for (int i = 0; i < db->nPratos && encontrados < maxResultados; i++) {
        if (contemSubstring(db->pratos[i].nome, nome_prato)) {
            resultado[encontrados] = db->pratos[i];
            encontrados++;
        }
    }

    return encontrados;
}

/*
 * getMenu
 *   Retorna todos os pratos do restaurante identificado por cnpj.
 *   Ver contrato em pratos.h.
 */
int getMenu(AppDados *db, long long int cnpj,
            Prato *resultado, int maxResultados) {

    if (db == NULL || cnpj <= 0 || resultado == NULL || maxResultados <= 0)
        return PRATOS_PARAM_INVALIDO;

    int encontrados = 0;
    for (int i = 0; i < db->nPratos && encontrados < maxResultados; i++) {
        if (db->pratos[i].cnpjRestaurante == cnpj) {
            resultado[encontrados] = db->pratos[i];
            encontrados++;
        }
    }

    return encontrados;
}

/*
 * getFeedPratos
 *   Seleciona 20 pratos aleatorios nao avaliados pelo usuario.
 *   Ver contrato em pratos.h.
 */
int getFeedPratos(AppDados *db, long long int cpf,
                  Prato *resultado, int maxResultados) {

    if (db == NULL || resultado == NULL || cpf <= 0)
        return PRATOS_PARAM_INVALIDO;

    inicializarSemente();

    /* Montar lista de indices elegiveis (nao avaliados pelo usuario) */
    int elegiveis[MAX_PRATOS];
    int nElegiveis = 0;
    for (int i = 0; i < db->nPratos; i++) {
        if (!pratoJaAvaliado(db, cpf, db->pratos[i].idPrato))
            elegiveis[nElegiveis++] = i;
    }

    /* PDF 5.6: menos de 20 elegiveis = insuficiente */
    if (nElegiveis < PRATOS_FEED_QTD)
        return PRATOS_INSUFICIENTE;

    shuffleIndices(elegiveis, nElegiveis);

    int copiar = PRATOS_FEED_QTD;
    if (copiar > maxResultados) copiar = maxResultados;

    for (int i = 0; i < copiar; i++) {
        resultado[i] = db->pratos[elegiveis[i]];
    }

    return copiar;
}