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
#include "../restaurante/restaurante.h"

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

/*
 * Objetivo: carregar os dados de pratos a partir do arquivo JSON.
 * Descricao:
 *     O sistema deve persistir os dados contidos no modulo Pratos.
 *     O sistema deve carregar os dados do modulo Pratos a partir do arquivo
 *     "pratos.json", populando o vetor estatico interno linha a linha.
 *     Chamada uma unica vez pelo Principal no inicio da execucao.
 *     O JSON e somente leitura; nao e regravado ao encerrar.
 * Acoplamento:
 *     Parametros:
 *       • (nenhum)
 *     Retornos:
 *       • int - 0 em caso de sucesso ou arquivo inexistente.
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • A estrutura de dados interna de pratos esta disponivel.
 *       • O arquivo "pratos.json" existe.
 * Assertivas de Saida:
 *   • O vetor pratos[] esta populado com os registros do JSON e nPratos
 *     reflete a quantidade carregada, ou nPratos == 0 em caso de falha.
 */
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

/*
 * Objetivo: obter um ponteiro para o prato correspondente ao ID informado.
 * Descricao:
 *     O sistema deve percorrer o vetor estatico interno buscando o prato
 *     pelo idPrato e retornar um ponteiro direto para ele. Usada pelo modulo
 *     Avaliacao para validar a existencia do prato antes de postAval().
 * Acoplamento:
 *     Parametros:
 *       • int idPrato - identificador do prato a buscar (deve ser > 0).
 *     Retornos:
 *       • Prato* - ponteiro para o prato encontrado no vetor interno.
 *       • NULL   - se idPrato <= 0 ou prato nao encontrado.
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • idPrato > 0.
 *       • carregarPratos() foi chamada anteriormente.
 * Assertivas de Saida:
 *   • Retorna ponteiro valido se o idPrato existe, NULL caso contrario.
 *   • Nenhum estado interno e alterado por esta funcao.
 */
Prato *getPratos(int idPrato) {
    if (idPrato <= 0) return NULL;

    for (int i = 0; i < nPratos; i++) {
        if (pratos[i].idPrato == idPrato)
            return &pratos[i];
    }
    return NULL;
}

/*
 * Objetivo: buscar pratos cujo nome contenha a substring fornecida.
 * Descricao:
 *     O sistema deve percorrer o vetor interno realizando busca case-insensitive
 *     e copiar os pratos cujo nome contenha nome_prato como substring para
 *     resultado[], respeitando maxResultados. Nome vazio retorna 0.
 * Acoplamento:
 *     Parametros:
 *       • const char *nome_prato   - substring a buscar no nome (nao NULL).
 *       • Prato      *resultado    - buffer de destino para os pratos encontrados.
 *       • int         maxResultados - capacidade maxima do buffer (deve ser > 0).
 *     Retornos:
 *       • int >= 0              - quantidade de pratos copiados para resultado[].
 *       • PRATOS_NOME_INVALIDO (-1) - nome_prato == NULL, resultado == NULL ou max <= 0.
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • nome_prato != NULL, resultado != NULL, maxResultados > 0.
 *       • carregarPratos() foi chamada anteriormente.
 * Assertivas de Saida:
 *   • resultado[] contem os pratos cujo nome contem nome_prato como substring.
 *   • O retorno indica a quantidade efetivamente preenchida em resultado[].
 *   • O vetor interno nao e alterado.
 */
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
 * Objetivo: recuperar todos os pratos pertencentes a um restaurante pelo CNPJ.
 * Descricao:
 *     O sistema deve percorrer o vetor interno e copiar para resultado[] todos
 *     os pratos cujo cnpjRestaurante corresponda ao CNPJ fornecido. Atende
 *     diretamente o modulo Restaurante para montar seu menu.
 * Acoplamento:
 *     Parametros:
 *       • long long int cnpj          - CNPJ do restaurante (deve ser > 0).
 *       • Prato        *resultado     - buffer de destino para os pratos encontrados.
 *       • int           maxResultados - capacidade maxima do buffer (deve ser > 0).
 *     Retornos:
 *       • int >= 0                - quantidade de pratos copiados para resultado[].
 *       • PRATOS_PARAM_INVALIDO (-2) - cnpj <= 0, resultado == NULL ou max <= 0.
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • cnpj > 0, resultado != NULL, maxResultados > 0.
 *       • carregarPratos() foi chamada anteriormente.
 * Assertivas de Saida:
 *   • resultado[] contem os pratos do restaurante com o CNPJ fornecido.
 *   • O retorno indica a quantidade efetivamente preenchida em resultado[].
 *   • O vetor interno nao e alterado.
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

/*
 * Objetivo: retornar o menu completo de um restaurante pelo CNPJ.
 * Descricao:
 *     O sistema deve delegar integralmente a chamada para getPratosPorCnpj(),
 *     mantendo compatibilidade com o contrato de interface original.
 * Acoplamento:
 *     Parametros:
 *       • long long int cnpj          - CNPJ do restaurante (deve ser > 0).
 *       • Prato        *resultado     - buffer de destino para os pratos encontrados.
 *       • int           maxResultados - capacidade maxima do buffer (deve ser > 0).
 *     Retornos:
 *       • Mesmo retorno de getPratosPorCnpj().
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • cnpj > 0, resultado != NULL, maxResultados > 0.
 *       • carregarPratos() foi chamada anteriormente.
 * Assertivas de Saida:
 *   • Mesmo comportamento de getPratosPorCnpj().
 */
int getMenu(long long int cnpj, Prato *resultado, int maxResultados) {
    return getPratosPorCnpj(cnpj, resultado, maxResultados);
}

/*
 * Objetivo: selecionar aleatoriamente pratos nao avaliados pelo usuario para o feed.
 * Descricao:
 *     O sistema deve filtrar os pratos ainda nao avaliados pelo CPF fornecido
 *     (consultando verificarSeAvaliado() do modulo Avaliacao), embaralhar os
 *     elegiveis com Fisher-Yates e retornar exatamente PRATOS_FEED_QTD (20) pratos.
 *     Inicializa a semente aleatoria na primeira chamada.
 * Acoplamento:
 *     Parametros:
 *       • long long int cpf           - CPF do usuario logado (deve ser > 0).
 *       • Prato        *resultado     - buffer de destino para os pratos selecionados.
 *       • int           maxResultados - capacidade maxima do buffer (deve ser > 0).
 *     Retornos:
 *       • PRATOS_FEED_QTD (20)     - quantidade de pratos copiados com sucesso.
 *       • PRATOS_INSUFICIENTE (-4) - menos de 20 pratos elegiveis (nao avaliados).
 *       • PRATOS_PARAM_INVALIDO (-2) - resultado == NULL, cpf <= 0 ou max <= 0.
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • cpf > 0, resultado != NULL, maxResultados > 0.
 *       • carregarPratos() e carregarAvaliacoes() foram chamadas anteriormente.
 * Assertivas de Saida:
 *   • resultado[] contem PRATOS_FEED_QTD pratos aleatorios nao avaliados pelo CPF.
 *   • A semente aleatoria foi inicializada.
 *   • O vetor interno de pratos nao e alterado.
 */
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

/*
 * Objetivo: exibir no terminal o ID e o nome de todos os pratos cadastrados.
 * Descricao:
 *     O sistema deve percorrer o vetor interno e imprimir no stdout cada prato
 *     no formato "ID: X | Nome", facilitando a escolha do usuario na tela de
 *     postagem de avaliacao.
 * Acoplamento:
 *     Parametros:
 *       • (nenhum)
 *     Retornos:
 *       • (nenhum)
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • carregarPratos() foi chamada anteriormente.
 * Assertivas de Saida:
 *   • Todos os pratos do vetor interno foram impressos no stdout.
 *   • Nenhum estado interno e alterado por esta funcao.
 */
void listarNomesPratos(void) {
    printf("\n--- Pratos Disponiveis ---\n");
    for (int i = 0; i < nPratos; i++) {
        Restaurante rest = obterRestaurantePorCnpj(pratos[i].cnpjRestaurante);
        printf("ID: %d | %s - %s\n",
               pratos[i].idPrato,
               pratos[i].nome,
               rest.cnpj != 0 ? rest.nome : "Restaurante desconhecido");
    }
}
/*
 * Objetivo: calcular a media aritmetica das notas de um prato e armazena-la.
 * Descricao:
 *     O sistema deve recuperar todas as avaliacoes do prato via verAval() do
 *     modulo Avaliacao, calcular a media aritmetica das notas e armazenar o
 *     resultado no ponteiro mediaDestino. Retorna a quantidade de avaliacoes.
 * Acoplamento:
 *     Parametros:
 *       • int    idPrato       - ID do prato a calcular (deve ser > 0).
 *       • float *mediaDestino  - ponteiro onde a media sera armazenada (nao NULL).
 *     Retornos:
 *       • int >= 0 - quantidade de avaliacoes usadas no calculo.
 *       • 0        - se idPrato <= 0 ou sem avaliacoes (*mediaDestino == 0.0f).
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • idPrato > 0, mediaDestino != NULL.
 *       • carregarAvaliacoes() e carregarPratos() foram chamadas anteriormente.
 * Assertivas de Saida:
 *   • *mediaDestino contem a media em [0.0, 5.0].
 *   • O retorno indica a quantidade de avaliacoes computadas.
 *   • O vetor interno nao e alterado.
 */
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