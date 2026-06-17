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

/*
 * Objetivo: retornar a quantidade de restaurantes atualmente carregados em memoria.
 * Descricao:
 *     O sistema deve expor o valor da variavel estatica privada nRestaurantes
 *     sem conceder acesso direto a ela. Util para integracoes e iteracoes externas.
 * Acoplamento:
 *     Parametros:
 *       • (nenhum)
 *     Retornos:
 *       • int - numero de restaurantes carregados (>= 0).
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • carregarRestaurantes() foi chamada anteriormente.
 * Assertivas de Saida:
 *   • Retorna valor >= 0 sem alterar o estado interno.
 */
int obterTotalRestaurantes(void) {
    return nRestaurantes;
}

/*
 * Objetivo: retornar uma copia do restaurante em um determinado indice do banco interno.
 * Descricao:
 *     O sistema deve acessar o vetor estatico interno pelo indice fornecido e
 *     retornar uma copia por valor. Se o indice for invalido, retorna uma struct
 *     zerada. Util para iteracoes externas seguras sem expor o vetor interno.
 * Acoplamento:
 *     Parametros:
 *       • int indice - posicao no vetor interno (0 <= indice < nRestaurantes).
 *     Retornos:
 *       • Restaurante - copia do registro na posicao informada.
 *       • Restaurante zerada (cnpj=0) se indice fora dos limites.
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • indice >= 0 e < nRestaurantes.
 *       • carregarRestaurantes() foi chamada anteriormente.
 * Assertivas de Saida:
 *   • Retorna copia valida do restaurante ou struct zerada se fora dos limites.
 *   • O vetor interno permanece inalterado.
 */
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

/*
 * Objetivo: carregar os dados de restaurantes a partir do arquivo JSON.
 * Descricao:
 *     O sistema deve persistir os dados contidos no modulo Restaurante.
 *     O sistema deve carregar os dados do modulo Restaurante a partir do arquivo
 *     "restaurantes.json", populando o vetor estatico interno linha a linha.
 *     Chamada uma unica vez pelo Principal no inicio da execucao.
 *     O JSON e somente leitura; nao e regravado ao encerrar.
 * Acoplamento:
 *     Parametros:
 *       • (nenhum)
 *     Retornos:
 *       • int - 0 em caso de sucesso ou arquivo inexistente.
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • A estrutura de dados interna de restaurantes esta disponivel.
 *       • O arquivo "restaurantes.json" existe.
 * Assertivas de Saida:
 *   • O vetor restaurantes[] esta populado com os registros do JSON e
 *     nRestaurantes reflete a quantidade carregada, ou nRestaurantes == 0
 *     em caso de falha.
 */
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

/*
 * Objetivo: buscar restaurantes cujo nome contenha a substring fornecida.
 * Descricao:
 *     O sistema deve percorrer o vetor interno realizando busca case-insensitive
 *     e copiar os restaurantes cujo nome contenha nome_rest como substring para
 *     resultado[], respeitando maxResultados. Nome vazio retorna 0.
 * Acoplamento:
 *     Parametros:
 *       • const char    *nome_rest    - substring a buscar no nome (nao NULL).
 *       • Restaurante   *resultado    - buffer de destino para os restaurantes encontrados.
 *       • int            maxResultados - capacidade maxima do buffer (deve ser > 0).
 *     Retornos:
 *       • int >= 0             - quantidade de restaurantes copiados para resultado[].
 *       • REST_NOME_INVALIDO (-1) - nome_rest == NULL, resultado == NULL ou max <= 0.
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • nome_rest != NULL, resultado != NULL, maxResultados > 0.
 *       • carregarRestaurantes() foi chamada anteriormente.
 * Assertivas de Saida:
 *   • resultado[] contem os restaurantes cujo nome contem nome_rest.
 *   • O retorno indica a quantidade efetivamente preenchida em resultado[].
 *   • O vetor interno nao e alterado.
 */
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

/*
 * Objetivo: selecionar aleatoriamente restaurantes para o feed de sugestoes.
 * Descricao:
 *     O sistema deve embaralhar os indices do vetor interno com o algoritmo
 *     Fisher-Yates e copiar os primeiros REST_FEED_QTD (6) restaurantes para
 *     resultado[], garantindo ausencia de repeticoes. Requer ao menos 6 registros.
 * Acoplamento:
 *     Parametros:
 *       • Restaurante *resultado     - buffer de destino para os restaurantes selecionados.
 *       • int          maxResultados - capacidade maxima do buffer (deve ser > 0).
 *     Retornos:
 *       • REST_FEED_QTD (6)       - quantidade de restaurantes copiados com sucesso.
 *       • REST_INSUFICIENTE (-3)  - menos de 6 restaurantes no banco.
 *       • REST_PARAM_INVALIDO (-2) - resultado == NULL ou maxResultados <= 0.
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • resultado != NULL, maxResultados > 0.
 *       • nRestaurantes >= REST_FEED_QTD.
 *       • carregarRestaurantes() foi chamada anteriormente.
 * Assertivas de Saida:
 *   • resultado[] contem REST_FEED_QTD restaurantes aleatorios sem repeticao.
 *   • A semente aleatoria foi inicializada.
 *   • O vetor interno de restaurantes nao e alterado.
 */
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
 * Objetivo: retornar todos os pratos de um restaurante pelo CNPJ.
 * Descricao:
 *     O sistema deve delegar a consulta para getPratosPorCnpj() do modulo
 *     Pratos, mantendo o encapsulamento e evitando acesso direto ao vetor
 *     interno de pratos a partir deste modulo.
 * Acoplamento:
 *     Parametros:
 *       • long long int cnpj          - CNPJ do restaurante (deve ser > 0).
 *       • Prato        *resultado     - buffer de destino para os pratos encontrados.
 *       • int           maxResultados - capacidade maxima do buffer (deve ser > 0).
 *     Retornos:
 *       • int >= 0                - quantidade de pratos copiados para resultado[].
 *       • REST_PARAM_INVALIDO (-2) - cnpj <= 0, resultado == NULL ou max <= 0.
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • cnpj > 0, resultado != NULL, maxResultados > 0.
 *       • carregarPratos() e carregarRestaurantes() foram chamadas anteriormente.
 * Assertivas de Saida:
 *   • resultado[] contem os pratos pertencentes ao restaurante com o CNPJ fornecido.
 *   • O retorno indica a quantidade efetivamente preenchida em resultado[].
 */
int getMenuRestaurante(long long int cnpj, Prato *resultado, int maxResultados) {
    if (cnpj <= 0 || resultado == NULL || maxResultados <= 0)
        return REST_PARAM_INVALIDO;

    /* * Encaminha o pedido diretamente para a nova funcao de interface de pratos, 
     * a qual varrera os pratos dela isoladamente.
     */
    return getPratosPorCnpj(cnpj, resultado, maxResultados);
}