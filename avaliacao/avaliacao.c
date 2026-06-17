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

/*
 * Objetivo: verificar se um usuario ja avaliou um determinado prato.
 * Descricao:
 *     O sistema deve percorrer o vetor interno de avaliacoes buscando um
 *     registro que combine o CPF e o idPrato fornecidos. Usada pelo modulo
 *     Pratos para filtrar o feed de recomendacoes e evitar repeticoes.
 * Acoplamento:
 *     Parametros:
 *       • long long int cpf    - CPF do usuario a verificar.
 *       • int           idPrato - identificador do prato a verificar.
 *     Retornos:
 *       • 1 - se ja existe avaliacao do CPF para o idPrato.
 *       • 0 - se o prato ainda nao foi avaliado pelo usuario.
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • carregarAvaliacoes() foi chamada anteriormente.
 * Assertivas de Saida:
 *   • Retorna 1 ou 0 sem alterar o estado interno.
 */
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

/*
 * Objetivo: carregar os dados de avaliacoes a partir do arquivo JSON.
 * Descricao:
 *     O sistema deve persistir os dados contidos no modulo Avaliacao.
 *     O sistema deve carregar os dados do modulo Avaliacao a partir do arquivo
 *     "avaliacoes.json", populando o vetor estatico interno linha a linha e
 *     ajustando proximoIdAval. Se o arquivo nao existir, retorna 0 sem erro.
 * Acoplamento:
 *     Parametros:
 *       • (nenhum)
 *     Retornos:
 *       • int - 0 em caso de sucesso ou arquivo inexistente.
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • A estrutura de dados interna de avaliacoes esta disponivel.
 *       • O arquivo "avaliacoes.json" existe ou e a primeira execucao do sistema.
 * Assertivas de Saida:
 *   • O vetor avaliacoes[] esta populado corretamente e nAvaliacoes reflete
 *     a quantidade carregada; proximoIdAval e maior que o maior idAval lido.
 *   • Se falha na abertura, nAvaliacoes == 0 e proximoIdAval == 1.
 */
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

/*
 * Objetivo: salvar os dados de avaliacoes no arquivo JSON.
 * Descricao:
 *     O sistema deve persistir os dados contidos no modulo Avaliacao.
 *     O sistema deve serializar o vetor estatico interno de avaliacoes para o
 *     arquivo "avaliacoes.json", gravando um registro por linha em formato JSON.
 *     Chamada pelo Principal ao encerrar a execucao.
 * Acoplamento:
 *     Parametros:
 *       • (nenhum)
 *     Retornos:
 *       • int - 0 em caso de sucesso; -1 se nao for possivel abrir o arquivo.
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • O vetor avaliacoes[] e nAvaliacoes estao em estado valido em memoria.
 * Assertivas de Saida:
 *   • O arquivo "avaliacoes.json" contem os dados atualizados de todas as
 *     avaliacoes em memoria, ou o arquivo permanece inalterado em caso de erro.
 */
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

/*
 * Objetivo: criar e registrar uma nova avaliacao de prato no sistema.
 * Descricao:
 *     O sistema deve validar os parametros, verificar se o prato existe via
 *     API do modulo Pratos, criar a avaliacao no vetor interno com ID unico
 *     autoincremental e comentario opcional.
 * Acoplamento:
 *     Parametros:
 *       • long long int cpf    - CPF do avaliador (deve ser > 0).
 *       • int           idPrato - ID do prato a avaliar (deve ser > 0 e existir).
 *       • const char   *txt     - comentario textual opcional (NULL = sem comentario).
 *       • float         nota    - nota inteira de 0 a 5.
 *     Retornos:
 *       • AVAL_OK (0)              - avaliacao criada com comentario.
 *       • AVAL_OK_SEM_TXT (1)      - avaliacao criada sem comentario.
 *       • AVAL_ERRO_ID_PRATO (2)   - idPrato nao existe no banco de pratos.
 *       • AVAL_ERRO_NOTA (4)       - nota fora de [0,5] ou nao-inteira.
 *       • AVAL_PARAM_INVALIDO (-1) - cpf <= 0 ou idPrato <= 0.
 *       • AVAL_BANCO_CHEIO (-4)    - MAX_AVALIACOES atingido.
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • cpf > 0, idPrato > 0, nota em [0,5] inteiro.
 *       • carregarAvaliacoes() e carregarPratos() foram chamadas anteriormente.
 * Assertivas de Saida:
 *   • Se AVAL_OK ou AVAL_OK_SEM_TXT, nova avaliacao esta no vetor interno,
 *     nAvaliacoes foi incrementado e proximoIdAval foi atualizado.
 *   • Se falha, o vetor interno permanece inalterado.
 */
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

/*
 * Objetivo: recuperar todas as avaliacoes de um determinado prato.
 * Descricao:
 *     O sistema deve percorrer o vetor interno e copiar para resultado[] todas
 *     as avaliacoes cujo idPrato corresponda ao informado, respeitando o limite
 *     maxResultados. Usada por obterMediaPrato() no modulo Pratos.
 * Acoplamento:
 *     Parametros:
 *       • int        idPrato       - ID do prato a consultar (deve ser > 0).
 *       • Avaliacao *resultado     - buffer de destino para as avaliacoes encontradas.
 *       • int        maxResultados - capacidade maxima do buffer (deve ser > 0).
 *     Retornos:
 *       • int >= 0             - quantidade de avaliacoes copiadas para resultado[].
 *       • AVAL_PARAM_INVALIDO (-1) - parametro invalido (NULL, idPrato <= 0 ou max <= 0).
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • idPrato > 0, resultado != NULL, maxResultados > 0.
 *       • carregarAvaliacoes() foi chamada anteriormente.
 * Assertivas de Saida:
 *   • resultado[] contem ate maxResultados avaliacoes do prato indicado.
 *   • O retorno indica a quantidade efetivamente preenchida em resultado[].
 *   • O vetor interno nao e alterado.
 */
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

/*
 * Objetivo: editar a nota e/ou o comentario de uma avaliacao existente.
 * Descricao:
 *     O sistema deve localizar a avaliacao pelo idAval, validar que o CPF
 *     fornecido e o autor da avaliacao (controle de permissao), e aplicar
 *     a nova nota e/ou comentario se fornecidos.
 * Acoplamento:
 *     Parametros:
 *       • int           idAval   - ID da avaliacao a editar (deve ser > 0).
 *       • long long int cpf      - CPF do solicitante (deve ser o autor da avaliacao).
 *       • const char   *novaTxt  - novo comentario textual (NULL = manter o atual).
 *       • float         novaNota - nova nota inteira em [0,5].
 *     Retornos:
 *       • AVAL_OK (0)               - avaliacao editada com sucesso.
 *       • AVAL_ERRO_NOTA (4)        - novaNota fora de [0,5] ou nao-inteira.
 *       • AVAL_PARAM_INVALIDO (-1)  - idAval <= 0 ou cpf <= 0.
 *       • AVAL_ERRO_EDICAO (-2)     - avaliacao com idAval nao encontrada.
 *       • AVAL_ERRO_PERMISSAO (-3)  - CPF nao e o autor da avaliacao.
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • idAval > 0, cpf > 0, novaNota em [0,5] inteiro.
 *       • carregarAvaliacoes() foi chamada anteriormente.
 * Assertivas de Saida:
 *   • Se AVAL_OK, a avaliacao idAval no vetor interno esta com os novos
 *     valores aplicados.
 *   • Se falha, o vetor interno permanece inalterado.
 */
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

/*
 * Objetivo: imprimir no terminal todas as avaliacoes feitas por um usuario.
 * Descricao:
 *     O sistema deve percorrer o vetor interno de avaliacoes e imprimir no
 *     stdout cada registro cujo CPF corresponda ao fornecido, exibindo ID,
 *     prato, nota e comentario. Retorna o total de avaliacoes encontradas.
 * Acoplamento:
 *     Parametros:
 *       • long long int cpf - CPF do usuario (deve ser > 0).
 *     Retornos:
 *       • int >= 0 - quantidade de avaliacoes impressas no terminal.
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • cpf > 0.
 *       • carregarAvaliacoes() foi chamada anteriormente.
 * Assertivas de Saida:
 *   • Todos os registros do CPF foram impressos no stdout.
 *   • Retorno >= 0 indica a contagem total de avaliacoes impressas.
 *   • O vetor interno nao e alterado.
 */
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