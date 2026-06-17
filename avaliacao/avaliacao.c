/*
 * avaliacao.c
 * Modulo de Avaliacao – Foodies
 *
 * Implementacao das funcoes de acesso declaradas em avaliacao.h.
 */

#include "avaliacao.h"
#include "../pratos/pratos.h" /* Necessario para validar se o idPrato existe no postAval */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

/* =================================================================
 * ARMAZENAMENTO INTERNO PROTEGIDO (Ocultamento de Informacao)
 * ================================================================= */
static Avaliacao avaliacoes[MAX_AVALIACOES];
static int nAvaliacoes = 0;
static int proximoIdAval = 1;

/* =================================================================
 * FUNCOES AUXILIARES PRIVADAS (static)
 * ================================================================= */

static int notaEhValida(float nota) {
    if (nota < 0.0f || nota > 5.0f) return 0;
    if (fabsf(nota - roundf(nota)) > 0.001f) return 0;
    return 1;
}

static int buscarIndiceAval(int idAval) {
    for (int i = 0; i < nAvaliacoes; i++) {
        if (avaliacoes[i].idAval == idAval)
            return i;
    }
    return -1;
}

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
 * IMPLEMENTACAO DAS FUNCOES DE INTERFACE ENTRE MODULOS
 * ================================================================= */

int verificarSeAvaliado(long long int cpf, int idPrato) {
    for (int i = 0; i < nAvaliacoes; i++) {
        if (avaliacoes[i].cpf == cpf && avaliacoes[i].idPrato == idPrato)
            return 1;
    }
    return 0;
}

/* =================================================================
 * FUNCOES DE I/O DO JSON
 * ================================================================= */

int carregarAvaliacoes(void) {
    nAvaliacoes = 0;
    proximoIdAval = 1;

    FILE *fp = fopen(AVALIACAO_JSON, "r");
    if (fp == NULL) return 0;

    char linha[600];
    while (fgets(linha, sizeof(linha), fp) && nAvaliacoes < MAX_AVALIACOES) {
        Avaliacao *a = &avaliacoes[nAvaliacoes];
        char comBuf[TAM_COMENTARIO] = "";

        int lidos = sscanf(linha,
            " {\"idAval\":%d,\"nota\":%f,\"cpf\":%lld,\"idPrato\":%d,\"comentario\":\"%400[^\"]\"}",
            &a->idAval, &a->nota, &a->cpf, &a->idPrato, comBuf);

        /* Se nao leu o comentario, tenta ler o formato sem comentario */
        if (lidos < 5) {
            lidos = sscanf(linha,
                " {\"idAval\":%d,\"nota\":%f,\"cpf\":%lld,\"idPrato\":%d,\"comentario\":\"\"}",
                &a->idAval, &a->nota, &a->cpf, &a->idPrato);
            if (lidos < 4) continue;
            comBuf[0] = '\0';
        }

        strncpy(a->comentario, comBuf, TAM_COMENTARIO - 1);
        a->comentario[TAM_COMENTARIO - 1] = '\0';

        if (a->idAval >= proximoIdAval) {
            proximoIdAval = a->idAval + 1;
        }

        nAvaliacoes++;
    }

    fclose(fp);
    return 0;
}

int salvarAvaliacoes(void) {
    FILE *fp = fopen(AVALIACAO_JSON, "w");
    if (fp == NULL) return -1;

    char comEsc[TAM_COMENTARIO * 2];

    for (int i = 0; i < nAvaliacoes; i++) {
        escaparString(comEsc, avaliacoes[i].comentario, sizeof(comEsc));

        fprintf(fp,
            "{\"idAval\":%d,\"nota\":%.1f,\"cpf\":%lld,\"idPrato\":%d,\"comentario\":\"%s\"}\n",
            avaliacoes[i].idAval,
            avaliacoes[i].nota,
            avaliacoes[i].cpf,
            avaliacoes[i].idPrato,
            comEsc);
    }

    fclose(fp);
    return 0;
}

/* =================================================================
 * FUNCOES DE ACESSO PUBLICAS
 * ================================================================= */

int postAval(long long int cpf, int idPrato, const char *txt, float nota) {
    if (cpf <= 0 || idPrato <= 0)
        return AVAL_PARAM_INVALIDO;

    if (!notaEhValida(nota))
        return AVAL_ERRO_NOTA;

    /* O prato deve existir — verificado via API do modulo Pratos */
    if (getPratos(idPrato) == NULL)
        return AVAL_ERRO_ID_PRATO;

    if (nAvaliacoes >= MAX_AVALIACOES)
        return AVAL_BANCO_CHEIO;

    Avaliacao *nova = &avaliacoes[nAvaliacoes];
    nova->idAval  = proximoIdAval++;
    nova->nota    = nota;
    nova->cpf     = cpf;
    nova->idPrato = idPrato;

    int semComentario = (txt == NULL || txt[0] == '\0');
    if (semComentario) {
        nova->comentario[0] = '\0';
    } else {
        strncpy(nova->comentario, txt, TAM_COMENTARIO - 1);
        nova->comentario[TAM_COMENTARIO - 1] = '\0';
    }

    nAvaliacoes++;
    return semComentario ? AVAL_OK_SEM_TXT : AVAL_OK;
}

int verAval(int idPrato, Avaliacao *resultado, int maxResultados) {
    if (idPrato <= 0 || resultado == NULL || maxResultados <= 0)
        return AVAL_PARAM_INVALIDO;

    int encontradas = 0;
    for (int i = 0; i < nAvaliacoes && encontradas < maxResultados; i++) {
        if (avaliacoes[i].idPrato != idPrato) continue;

        resultado[encontradas] = avaliacoes[i];
        encontradas++;
    }

    return encontradas;
}

int editAval(int idAval, long long int cpf, const char *novaTxt, float novaNota) {
    if (idAval <= 0 || cpf <= 0)
        return AVAL_PARAM_INVALIDO;

    // CORREÇÃO: Usar a função de validação nativa do seu projeto para pegar notas fracionadas inválidas
    if (novaNota < 0.0f || novaNota > 5.0f || !notaEhValida(novaNota)) {
        return AVAL_ERRO_NOTA; // Retorna 4 e faz o teste passar!
    }

    int idx = buscarIndiceAval(idAval);
    if (idx < 0)
        return AVAL_ERRO_EDICAO;

    if (avaliacoes[idx].cpf != cpf)
        return AVAL_ERRO_PERMISSAO;

    // Atribui a nova nota com segurança
    avaliacoes[idx].nota = novaNota;

    if (novaTxt != NULL) {
        strncpy(avaliacoes[idx].comentario, novaTxt, TAM_COMENTARIO - 1);
        avaliacoes[idx].comentario[TAM_COMENTARIO - 1] = '\0';
    }

    return AVAL_OK;
}

/* ---------------------------------------------------------------
 * imprimirAvaliacoesUsuario
 * Varre o repositorio de avaliacoes e imprime no terminal todos
 * os registros vinculados ao CPF fornecido.
 * Retorna o total de avaliacoes impressas.
 * --------------------------------------------------------------- */
int imprimirAvaliacoesUsuario(long long int cpf) {
    /* Assertiva de entrada */
    assert(cpf > 0);

    int contagem = 0;
    for (int i = 0; i < nAvaliacoes; i++) {
        if (avaliacoes[i].cpf != cpf) continue;
        printf("- [ID %d] Prato ID %d: Nota %.1f | %s\n",
               avaliacoes[i].idAval,
               avaliacoes[i].idPrato,
               avaliacoes[i].nota,
               avaliacoes[i].comentario);
        contagem++;
    }

    return contagem;
}