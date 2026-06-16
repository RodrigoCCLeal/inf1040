/*
 * restaurante.c
 * Modulo Restaurante - Foodies
 *
 * Implementacao das funcoes declaradas em restaurante.h.
 *
 * Este modulo centraliza e protege a memoria global de restaurantes
 * usando variaveis de escopo de arquivo (static).
 */

#include "restaurante.h"
#include "../pratos/pratos.h"  

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

/* =================================================================
 * ARMAZENAMENTO INTERNO PROTEGIDO (Ocultamento de Informacao)
 * ================================================================= */
static Restaurante restaurantes[MAX_RESTAURANTES];
static int nRestaurantes = 0;

/* =================================================================
 * FUNCOES AUXILIARES PRIVADAS (static)
 * ================================================================= */

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

static void shuffleIndices(int *indices, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j      = rand() % (i + 1);
        int tmp    = indices[i];
        indices[i] = indices[j];
        indices[j] = tmp;
    }
}

static int sementeInicializada = 0;

static void inicializarSemente(void) {
    if (!sementeInicializada) {
        srand((unsigned int)time(NULL));
        sementeInicializada = 1;
    }
}

/* =================================================================
 * IMPLEMENTACAO DAS FUNCOES DE INTERFACE DE API
 * ================================================================= */

int obterTotalRestaurantes(void) {
    return nRestaurantes;
}

Restaurante obterRestaurantePorIndice(int indice) {
    Restaurante vazio = {0, "", ""};
    if (indice < 0 || indice >= nRestaurantes) {
        return vazio;
    }
    return restaurantes[indice];
}

/* =================================================================
 * FUNCAO DE CARGA DO JSON (chamada exclusivamente pelo Principal)
 * ================================================================= */

int carregarRestaurantes(void) {
    nRestaurantes = 0;

    FILE *fp = fopen(RESTAURANTE_JSON, "r");
    if (fp == NULL) return 0;

    char linha[500];
    while (fgets(linha, sizeof(linha), fp) && nRestaurantes < MAX_RESTAURANTES) {

        Restaurante *r = &restaurantes[nRestaurantes];
        char nomeBuf[TAM_NOME]     = "";
        char endBuf [TAM_ENDERECO] = "";

        int lidos = sscanf(linha,
            " {\"cnpj\":%lld,\"nome\":\"%100[^\"]\","
            " \"endereco\":\"%200[^\"]\"}",
            &r->cnpj, nomeBuf, endBuf);

        if (lidos < 3) continue;

        strncpy(r->nome,     nomeBuf, TAM_NOME     - 1);
        strncpy(r->endereco, endBuf,  TAM_ENDERECO - 1);
        r->nome    [TAM_NOME     - 1] = '\0';
        r->endereco[TAM_ENDERECO - 1] = '\0';

        nRestaurantes++;
    }

    fclose(fp);
    return 0;
}

/* =================================================================
 * FUNCOES DE ACESSO PUBLICAS
 * ================================================================= */

int getListaRest(const char *nome_rest, Restaurante *resultado, int maxResultados) {
    if (nome_rest == NULL || resultado == NULL || maxResultados <= 0)
        return REST_NOME_INVALIDO;

    if (nome_rest[0] == '\0') return 0;

    int encontrados = 0;
    for (int i = 0; i < nRestaurantes && encontrados < maxResultados; i++) {
        if (contemSubstring(restaurantes[i].nome, nome_rest)) {
            resultado[encontrados] = restaurantes[i];
            encontrados++;
        }
    }

    return encontrados;
}

int getFeedRest(Restaurante *resultado, int maxResultados) {
    if (resultado == NULL || maxResultados <= 0) return REST_PARAM_INVALIDO;
    if (nRestaurantes < REST_FEED_QTD) return REST_INSUFICIENTE;

    inicializarSemente();

    int indices[MAX_RESTAURANTES];
    for (int i = 0; i < nRestaurantes; i++) indices[i] = i;
    shuffleIndices(indices, nRestaurantes);

    int copiar = REST_FEED_QTD;
    if (copiar > maxResultados) copiar = maxResultados;

    for (int i = 0; i < copiar; i++) {
        resultado[i] = restaurantes[indices[i]];
    }

    return copiar;
}

/*
 * getMenuRestaurante
 * Como 'db->pratos' nao existe mais, esta funcao delega a varredura
 * para a API publica do modulo Pratos (ex: getPratosPorCnpj), mantendo o encapsulamento.
 */
int getMenuRestaurante(long long int cnpj, Prato *resultado, int maxResultados) {
    if (cnpj <= 0 || resultado == NULL || maxResultados <= 0)
        return REST_PARAM_INVALIDO;

    /* * Encaminha o pedido diretamente para a nova funcao de interface de pratos, 
     * a qual varrera os pratos dela isoladamente.
     */
    return getPratosPorCnpj(cnpj, resultado, maxResultados);
}