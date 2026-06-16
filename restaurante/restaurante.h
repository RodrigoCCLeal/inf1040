/*
 * restaurante.h
 * Modulo Restaurante - Foodies
 *
 * Responsavel por: carregar e consultar restaurantes.
 *
 * DADOS FIXOS: restaurantes.json e somente leitura.
 * Usuarios nao podem adicionar ou remover restaurantes (PDF 2.1).
 * O JSON e carregado no inicio pelo Principal e NAO e regravado
 * ao encerrar (os dados nunca mudam durante a execucao).
 *
 * Relacoes cliente-servidor (PDF 3.2):
 *   BUSCAR (cliente) -> RESTAURANTES (servidor): getListaRest
 *   FEED   (cliente) -> RESTAURANTES (servidor): getFeedRest
 *   PRATOS (cliente) -> RESTAURANTES (servidor): getMenuRestaurante
 *
 * Dados encapsulados (PDF 3.3):
 *   restaurante(long long int cnpj, char endereco[], char nome[])
 *   Conhecido por: prato e buscar
 *
 * Funcao de carga do JSON (chamada pelo Principal no inicio):
 *   carregarRestaurantes - le restaurantes.json -> db->restaurantes[]
 *
 * Funcoes de acesso (PDF 3.4.6):
 *   getListaRest       - retorna restaurantes cujo nome contenha a substring
 *   getFeedRest        - retorna 6 restaurantes aleatorios
 *   getMenuRestaurante - retorna todos os pratos de um restaurante (via CNPJ)
 */

#ifndef RESTAURANTE_H
#define RESTAURANTE_H

#include "../dados/dados.h"

/* --- Nome do arquivo JSON deste modulo (somente leitura) --- */
#define RESTAURANTE_JSON "restaurantes.json"

/* --- Quantidade do feed de restaurantes --- */
#define REST_FEED_QTD 6

/* --- Codigos de retorno --- */
#define REST_NOME_INVALIDO  -1   /* nome==NULL ou resultado==NULL      */
#define REST_PARAM_INVALIDO -2   /* db==NULL ou parametro invalido     */
#define REST_INSUFICIENTE   -3   /* menos de 6 restaurantes no banco   */
/* Retorno >= 0 indica quantidade de itens copiados                    */

/*
 * carregarRestaurantes
 *   Le restaurantes.json e popula db->restaurantes[].
 *   Chamada UMA UNICA VEZ pelo Principal no inicio da execucao.
 *   O arquivo nao e alterado durante nem ao final da execucao.
 *   Se o arquivo nao existir, retorna 0 (banco vazio, sem erro).
 *
 *   Retorno:
 *     0  - carregado com sucesso (ou arquivo inexistente)
 *    -1  - erro de leitura
 */
int carregarRestaurantes(AppDados *db);

/*
 * getListaRest
 *   Retorna restaurantes cujo nome contenha nome_rest como substring.
 *   Busca case-insensitive. Nome vazio retorna 0 (PDF 2.4).
 *
 *   Parametros:
 *     db            - banco em memoria
 *     nome_rest     - substring a buscar (nao pode ser NULL)
 *     resultado     - array de Restaurante alocado pelo chamador
 *     maxResultados - capacidade do array
 *
 *   Retorno (PDF 5.5 getListaRest):
 *     >= 0               - quantidade encontrada
 *     REST_NOME_INVALIDO - nome_rest==NULL ou resultado==NULL
 */
int getListaRest(AppDados *db, const char *nome_rest,
                 Restaurante *resultado, int maxResultados);

/*
 * getFeedRest
 *   Seleciona REST_FEED_QTD (6) restaurantes aleatorios do banco.
 *   Usa Fisher-Yates shuffle. Sem repeticoes.
 *
 *   Parametros:
 *     db            - banco em memoria
 *     resultado     - array de Restaurante (minimo 6 slots)
 *     maxResultados - capacidade do array
 *
 *   Retorno (PDF 5.5 getFeedRest):
 *     REST_FEED_QTD (6)   - sucesso
 *     REST_INSUFICIENTE   - menos de 6 restaurantes no banco
 *     REST_PARAM_INVALIDO - db==NULL ou resultado==NULL
 */
int getFeedRest(AppDados *db, Restaurante *resultado, int maxResultados);

/*
 * getMenuRestaurante
 *   Retorna todos os pratos pertencentes ao restaurante de dado CNPJ.
 *
 *   Parametros:
 *     db            - banco em memoria
 *     cnpj          - CNPJ do restaurante (deve ser > 0)
 *     resultado     - array de Prato alocado pelo chamador
 *     maxResultados - capacidade do array
 *
 *   Retorno (PDF 5.6 getMenu):
 *     >= 0                - quantidade de pratos encontrados
 *     REST_PARAM_INVALIDO - db==NULL, cnpj<=0 ou resultado==NULL
 */
int getMenuRestaurante(AppDados *db, long long int cnpj,
                       Prato *resultado, int maxResultados);

#endif /* RESTAURANTE_H */