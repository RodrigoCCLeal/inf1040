/*
 * restaurante.c
 * Modulo Restaurante - Foodies
 *
 * Implementacao das funcoes declaradas em restaurante.h.
 *
 * DADOS FIXOS: este modulo apenas LE restaurantes.json.
 * O arquivo nunca e reescrito — restaurantes sao imutaveis durante
 * toda a execucao do programa (usuarios nao podem adicionar ou remover).
 *
 * Formato do JSON (uma linha por registro):
 *   {"cnpj":11111111000101,"nome":"Toca da Traira","endereco":"Av. Barra, 100"}
 */

#include "restaurante.h"

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
 *   Retorna 1 se contem, 0 caso contrario.
 *   Utilizada por getListaRest para busca parcial pelo nome.
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
 *   Algoritmo Fisher-Yates: embaralha indices[] in-place.
 *   Utilizada por getFeedRest para selecao aleatoria sem repeticao.
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

/* =================================================================
 * FUNCAO DE CARGA DO JSON (chamada exclusivamente pelo Principal)
 * ================================================================= */

/*
 * carregarRestaurantes
 *   Le restaurantes.json e popula db->restaurantes[].
 *   Chamada uma unica vez no inicio. O arquivo nao e alterado depois.
 *   Ver contrato em restaurante.h.
 */
int carregarRestaurantes(AppDados *db) {
    if (db == NULL) return -1;

    db->nRestaurantes = 0;

    FILE *fp = fopen(RESTAURANTE_JSON, "r");
    if (fp == NULL) return 0;  /* arquivo nao existe ainda, sem erro */

    char linha[500];
    while (fgets(linha, sizeof(linha), fp) &&
           db->nRestaurantes < MAX_RESTAURANTES) {

        Restaurante *r = &db->restaurantes[db->nRestaurantes];
        char nomeBuf[TAM_NOME]     = "";
        char endBuf [TAM_ENDERECO] = "";

        int lidos = sscanf(linha,
            " {\"cnpj\":%lld,\"nome\":\"%100[^\"]\","
            "\"endereco\":\"%200[^\"]\"}",
            &r->cnpj, nomeBuf, endBuf);

        if (lidos < 3) continue;

        strncpy(r->nome,     nomeBuf, TAM_NOME     - 1);
        strncpy(r->endereco, endBuf,  TAM_ENDERECO - 1);
        r->nome    [TAM_NOME     - 1] = '\0';
        r->endereco[TAM_ENDERECO - 1] = '\0';

        db->nRestaurantes++;
    }

    fclose(fp);
    return 0;
}

/* =================================================================
 * FUNCOES DE ACESSO PUBLICAS (PDF 3.4.6)
 * ================================================================= */

/*
 * getListaRest
 *   Busca restaurantes por substring no nome (case-insensitive).
 *   Ver contrato em restaurante.h.
 */
int getListaRest(AppDados *db, const char *nome_rest,
                 Restaurante *resultado, int maxResultados) {

    if (db == NULL || nome_rest == NULL ||
        resultado == NULL || maxResultados <= 0)
        return REST_NOME_INVALIDO;

    /* PDF 2.4: nome vazio nao retorna nada */
    if (nome_rest[0] == '\0') return 0;

    int encontrados = 0;
    for (int i = 0; i < db->nRestaurantes && encontrados < maxResultados; i++) {
        if (contemSubstring(db->restaurantes[i].nome, nome_rest)) {
            resultado[encontrados] = db->restaurantes[i];
            encontrados++;
        }
    }

    return encontrados;
}

/*
 * getFeedRest
 *   Seleciona 6 restaurantes aleatorios via Fisher-Yates.
 *   Ver contrato em restaurante.h.
 */
int getFeedRest(AppDados *db, Restaurante *resultado, int maxResultados) {

    if (db == NULL || resultado == NULL) return REST_PARAM_INVALIDO;
    if (db->nRestaurantes < REST_FEED_QTD) return REST_INSUFICIENTE;

    inicializarSemente();

    int indices[MAX_RESTAURANTES];
    for (int i = 0; i < db->nRestaurantes; i++) indices[i] = i;
    shuffleIndices(indices, db->nRestaurantes);

    int copiar = REST_FEED_QTD;
    if (copiar > maxResultados) copiar = maxResultados;

    for (int i = 0; i < copiar; i++) {
        resultado[i] = db->restaurantes[indices[i]];
    }

    return copiar;
}

/*
 * getMenuRestaurante
 *   Retorna todos os pratos do restaurante identificado por cnpj.
 *   Ver contrato em restaurante.h.
 */
int getMenuRestaurante(AppDados *db, long long int cnpj,
                       Prato *resultado, int maxResultados) {

    if (db == NULL || cnpj <= 0 || resultado == NULL || maxResultados <= 0)
        return REST_PARAM_INVALIDO;

    int encontrados = 0;
    for (int i = 0; i < db->nPratos && encontrados < maxResultados; i++) {
        if (db->pratos[i].cnpjRestaurante == cnpj) {
            resultado[encontrados] = db->pratos[i];
            encontrados++;
        }
    }

    return encontrados;
}