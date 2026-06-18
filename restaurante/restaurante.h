/*
 * restaurante.h
 * Modulo Restaurante - Foodies
 *
 * Responsavel por: carregar e consultar restaurantes.
 *
 * DADOS FIXOS: restaurantes.json e somente leitura.
 * O JSON e carregado no inicio pelo Principal e NAO e regravado
 * ao encerrar (os dados nunca mudam durante a execucao).
 *
 * Relacoes cliente-servidor:
 * BUSCAR (cliente) -> RESTAURANTES (servidor): getListaRest
 * FEED   (cliente) -> RESTAURANTES (servidor): getFeedRest
 * PRATOS (cliente) -> RESTAURANTES (servidor): getMenuRestaurante
 *
 * Dados encapsulados:
 * Restaurante(long long int cnpj, char endereco[], char nome[])
 */

#ifndef RESTAURANTE_H
#define RESTAURANTE_H

#include "../pratos/pratos.h"

/* --- Limites comerciais e tamanhos realocados --- */
#define MAX_RESTAURANTES  500
#define TAM_NOME          101
#define TAM_ENDERECO      201

/* Restaurante - Estrutura publica para trafego entre modulos */
typedef struct {
    long long int cnpj;         /* identificador principal */
    char endereco[TAM_ENDERECO];
    char nome    [TAM_NOME];
} Restaurante;


/* --- Nome do arquivo JSON deste modulo (somente leitura) --- */
#define RESTAURANTE_JSON "restaurantes.json"

/* --- Quantidade do feed de restaurantes --- */
#define REST_FEED_QTD 6

/* --- Codigos de retorno --- */
#define REST_NOME_INVALIDO  -1   /* nome_rest==NULL ou resultado==NULL  */
#define REST_PARAM_INVALIDO -2   /* Parametro invalido detectado        */
#define REST_INSUFICIENTE   -3   /* menos de 6 restaurantes no banco    */
/* Retorno >= 0 indica quantidade de itens copiados                    */

/* =================================================================
 * FUNCOES DE INTERFACE DE ACESSO AO TOTAL DE REGISTROS (API)
 * ================================================================= */

/*
 * obterTotalRestaurantes
 * Retorna a quantidade de restaurantes atualmente carregados na memoria.
 */
int obterTotalRestaurantes(void);

/*
 * obterRestaurantePorCnpj
 * Retorna uma copia do restaurante identificado pelo CNPJ fornecido.
 * Util para integracoes ou iteracoes externas seguras sem expor o vetor interno.
 */
Restaurante obterRestaurantePorCnpj(long long int cnpj);

/* =================================================================
 * FUNCOES DE I/O E GERENCIAMENTO DO BANCO DE DADOS OCULTO
 * ================================================================= */

/*
 * carregarRestaurantes
 * Le restaurantes.json e popula o vetor estatico interno do arquivo .c.
 * Chamada UMA UNICA VEZ pelo Principal no inicio da execucao.
 */
int carregarRestaurantes(void);

/*
 * getListaRest
 * Retorna restaurantes cujo nome contenha nome_rest como substring.
 * Busca case-insensitive. Nome vazio retorna 0.
 */
int getListaRest(const char *nome_rest, Restaurante *resultado, int maxResultados);

/*
 * getFeedRest
 * Seleciona REST_FEED_QTD (6) restaurantes aleatorios do banco interno.
 * Usa Fisher-Yates shuffle. Sem repeticoes.
 */
int getFeedRest(Restaurante *resultado, int maxResultados);

/*
 * getMenuRestaurante
 * Retorna todos os pratos pertencentes ao restaurante de dado CNPJ.
 * Esta funcao interage diretamente com a API do modulo Pratos.
 */
int getMenuRestaurante(long long int cnpj, Prato *resultado, int maxResultados);

#endif /* RESTAURANTE_H */