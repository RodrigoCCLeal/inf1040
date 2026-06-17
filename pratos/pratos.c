/*
 * pratos.c
 * Modulo Pratos - Foodies
 *
 * Implementacao das funcoes declaradas in pratos.h.
 *
 * Este modulo protege a memoria global de pratos usando variaveis 
 * de escopo de arquivo (static). Ele se comunica com o modulo avaliacao
 * por meio de funcoes de interface limpas.
 */

#include "pratos.h"
#include "../avaliacao/avaliacao.h" /* Necessario para verificar se prato foi avaliado */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

/* =================================================================
 * ARMAZENAMENTO INTERNO PROTEGIDO (Ocultamento de Informacao)
 * ================================================================= */
static Prato pratos[MAX_PRATOS];
static int nPratos = 0;

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

/*
 * pratoJaAvaliado
 * Nao acessa mais db->avaliacoes diretamente. Agora ele faz uma chamada
 * para a API publica do modulo de Avaliacao (ex: verificarSeAvaliado).
 */
static int pratoJaAvaliado(long long int cpf, int idPrato) {
    return verificarSeAvaliado(cpf, idPrato);
}

/* =================================================================
 * FUNCAO DE CARGA DO JSON (chamada exclusivamente pelo Principal)
 * ================================================================= */

int carregarPratos(void) {
    nPratos = 0;

    FILE *fp = fopen(PRATOS_JSON, "r");
    if (fp == NULL) return 0;

    char linha[800];
    while (fgets(linha, sizeof(linha), fp) && nPratos < MAX_PRATOS) {

        Prato *p = &pratos[nPratos];
        char nomeBuf[TAM_NOME]       = "";
        char descBuf[TAM_COMENTARIO] = "";
        char endBuf [TAM_ENDERECO]   = "";

        int lidos = sscanf(linha,
            " {\"idPrato\":%d,\"nome\":\"%100[^\"]\","
            " \"descricao\":\"%400[^\"]\","
            " \"cnpj\":%lld,\"endereco\":\"%200[^\"]\"}",
            &p->idPrato, nomeBuf, descBuf,
            &p->cnpjRestaurante, endBuf);

        if (lidos < 4) continue;

        strncpy(p->nome,                nomeBuf, TAM_NOME       - 1);
        strncpy(p->descricao,           descBuf, TAM_COMENTARIO - 1);
        strncpy(p->enderecoRestaurante, endBuf,  TAM_ENDERECO   - 1);
        p->nome               [TAM_NOME       - 1] = '\0';
        p->descricao          [TAM_COMENTARIO - 1] = '\0';
        p->enderecoRestaurante[TAM_ENDERECO   - 1] = '\0';

        nPratos++;
    }

    fclose(fp);
    return 0;
}

/* =================================================================
 * FUNCOES DE ACESSO PUBLICAS
 * ================================================================= */

Prato *getPratos(int idPrato) {
    if (idPrato <= 0) return NULL;

    for (int i = 0; i < nPratos; i++) {
        if (pratos[i].idPrato == idPrato)
            return &pratos[i];
    }
    return NULL;
}

int getListaPratos(const char *nome_prato, Prato *resultado, int maxResultados) {
    if (nome_prato == NULL || resultado == NULL || maxResultados <= 0)
        return PRATOS_NOME_INVALIDO;

    if (nome_prato[0] == '\0') return 0;

    int encontrados = 0;
    for (int i = 0; i < nPratos && encontrados < maxResultados; i++) {
        if (contemSubstring(pratos[i].nome, nome_prato)) {
            resultado[encontrados] = pratos[i];
            encontrados++;
        }
    }

    return encontrados;
}

/*
 * getPratosPorCnpj / getMenu
 * Ambas realizam a mesma operacao varrendo a colecao privada local
 */
int getPratosPorCnpj(long long int cnpj, Prato *resultado, int maxResultados) {
    if (cnpj <= 0 || resultado == NULL || maxResultados <= 0)
        return PRATOS_PARAM_INVALIDO;

    int encontrados = 0;
    for (int i = 0; i < nPratos && encontrados < maxResultados; i++) {
        if (pratos[i].cnpjRestaurante == cnpj) {
            resultado[encontrados] = pratos[i];
            encontrados++;
        }
    }
    return encontrados;
}

int getMenu(long long int cnpj, Prato *resultado, int maxResultados) {
    return getPratosPorCnpj(cnpj, resultado, maxResultados);
}

int getFeedPratos(long long int cpf, Prato *resultado, int maxResultados) {
    if (resultado == NULL || cpf <= 0 || maxResultados <= 0)
        return PRATOS_PARAM_INVALIDO;

    inicializarSemente();

    /* Montar lista de indices elegiveis baseado no modulo de avaliacoes */
    int elegiveis[MAX_PRATOS];
    int nElegiveis = 0;
    for (int i = 0; i < nPratos; i++) {
        if (!pratoJaAvaliado(cpf, pratos[i].idPrato))
            elegiveis[nElegiveis++] = i;
    }

    if (nElegiveis < PRATOS_FEED_QTD)
        return PRATOS_INSUFICIENTE;

    shuffleIndices(elegiveis, nElegiveis);

    int copiar = PRATOS_FEED_QTD;
    if (copiar > maxResultados) copiar = maxResultados;

    for (int i = 0; i < copiar; i++) {
        resultado[i] = pratos[elegiveis[i]];
    }

    return copiar;
}

/* ---------------------------------------------------------------
 * listarNomesPratos
 * Exibe na saida padrao o ID e o Nome de todos os pratos cadastrados.
 * --------------------------------------------------------------- */
void listarNomesPratos(void) {
    printf("\n--- Pratos Disponiveis ---\n");
    for (int i = 0; i < nPratos; i++) {
        printf("ID: %d | %s\n", pratos[i].idPrato, pratos[i].nome);
    }
}

/* ---------------------------------------------------------------
 * obterMediaPrato
 * Calcula a media aritmetica das notas de um prato pelo seu ID.
 * Modifica o float apontado por mediaDestino e retorna a quantidade
 * total de avaliacoes computadas para o calculo.
 * --------------------------------------------------------------- */
int obterMediaPrato(int idPrato, float *mediaDestino) {
    assert(mediaDestino != NULL);

    if (idPrato <= 0) {
        *mediaDestino = 0.0f;
        return 0;
    }

    // Criamos um buffer temporário para puxar as avaliações deste prato específico
    // usando a função pública do módulo de avaliação
    Avaliacao buffer[100]; 
    
    // Supondo que verAval preencha o buffer e retorne a quantidade (ou código de sucesso)
    // Se verAval retornar a quantidade de itens preenchidos:
    int qtdAvaliacoes = verAval(idPrato, buffer, 100);

    // Se verAval retornar um código de erro/sucesso (ex: AVAL_OK = 0) e a quantidade 
    // for controlada de outra forma, ajuste aqui. Mas se ela retornar a contagem:
    if (qtdAvaliacoes < 0) qtdAvaliacoes = 0; // Tratamento de erro seguro

    float somaNotas = 0.0f;
    for (int i = 0; i < qtdAvaliacoes; i++) {
        somaNotas += buffer[i].nota;
    }

    if (qtdAvaliacoes > 0) {
        *mediaDestino = somaNotas / (float)qtdAvaliacoes;
    } else {
        *mediaDestino = 0.0f;
    }

    assert(*mediaDestino >= 0.0f && *mediaDestino <= 5.0f);

    return qtdAvaliacoes;
}