/*
 * pratos.h
 * Modulo Pratos - Foodies
 *
 * Responsavel por: carregar e consultar pratos.
 *
 * DADOS FIXOS: pratos.json e somente leitura.
 * Usuarios nao podem adicionar ou remover pratos (PDF 2.1).
 * O JSON e carregado no inicio pelo Principal e NAO e regravado
 * ao encerrar (os dados nunca mudam durante a execucao).
 *
 * Relacoes cliente-servidor (PDF 3.2):
 *   BUSCAR      (cliente) -> PRATOS (servidor): getListaPratos
 *   FEED        (cliente) -> PRATOS (servidor): getFeedPratos
 *   AVALIACAO   (cliente) -> PRATOS (servidor): getPratos
 *   RESTAURANTE (cliente) -> PRATOS (servidor): getMenu
 *
 * Dados encapsulados (PDF 3.3):
 *   prato(int idPrato, char nome[], char descricao[],
 *         long long int cnpjRestaurante, char enderecoRestaurante[])
 *   Conhecido por: prato e buscar
 *
 * Funcao de carga do JSON (chamada pelo Principal no inicio):
 *   carregarPratos - le pratos.json -> db->pratos[]
 *
 * Funcoes de acesso (PDF 3.4.7):
 *   getPratos      - retorna dados de um prato pelo ID
 *   getListaPratos - retorna pratos cujo nome contenha a substring
 *   getMenu        - retorna todos os pratos de um restaurante
 *   getFeedPratos  - retorna 20 pratos aleatorios nao avaliados pelo usuario
 */

#ifndef PRATOS_H
#define PRATOS_H

#include "../dados/dados.h"

/* --- Nome do arquivo JSON deste modulo (somente leitura) --- */
#define PRATOS_JSON "pratos.json"

/* --- Quantidade do feed de pratos --- */
#define PRATOS_FEED_QTD 20

/* --- Codigos de retorno --- */
#define PRATOS_NOME_INVALIDO  -1   /* nome==NULL ou resultado==NULL       */
#define PRATOS_PARAM_INVALIDO -2   /* db==NULL ou parametro invalido      */
#define PRATOS_NAO_ENCONTRADO -3   /* idPrato nao existe no banco         */
#define PRATOS_INSUFICIENTE   -4   /* menos de 20 pratos elegiveis        */
/* Retorno >= 0 indica quantidade de itens copiados                       */

/*
 * carregarPratos
 *   Le pratos.json e popula db->pratos[].
 *   Chamada UMA UNICA VEZ pelo Principal no inicio da execucao.
 *   O arquivo nao e alterado durante nem ao final da execucao.
 *   Se o arquivo nao existir, retorna 0 (banco vazio, sem erro).
 *
 *   Retorno:
 *     0  - carregado com sucesso (ou arquivo inexistente)
 *    -1  - erro de leitura
 */
int carregarPratos(AppDados *db);

/*
 * getPratos
 *   Retorna ponteiro para o prato com o idPrato dado.
 *   Operacao de somente leitura - nao modifica db.
 *
 *   Parametros:
 *     db      - banco em memoria
 *     idPrato - ID do prato a consultar (deve ser > 0)
 *
 *   Retorno (PDF 5.6 getPratos):
 *     Prato*  - ponteiro para o registro encontrado (dici_prato)
 *     NULL    - idPrato nao encontrado ou parametro invalido
 */
Prato *getPratos(AppDados *db, int idPrato);

/*
 * getListaPratos
 *   Retorna pratos cujo nome contenha nome_prato como substring.
 *   Busca case-insensitive. Nome vazio retorna 0 (PDF 2.4).
 *
 *   Parametros:
 *     db            - banco em memoria
 *     nome_prato    - substring a buscar (nao pode ser NULL)
 *     resultado     - array de Prato alocado pelo chamador
 *     maxResultados - capacidade do array
 *
 *   Retorno (PDF 5.6 getListaPratos):
 *     >= 0                 - quantidade encontrada
 *     PRATOS_NOME_INVALIDO - nome_prato==NULL ou resultado==NULL
 */
int getListaPratos(AppDados *db, const char *nome_prato,
                   Prato *resultado, int maxResultados);

/*
 * getMenu
 *   Retorna todos os pratos pertencentes ao restaurante de dado CNPJ.
 *
 *   Parametros:
 *     db            - banco em memoria
 *     cnpj          - CNPJ do restaurante (deve ser > 0)
 *     resultado     - array de Prato alocado pelo chamador
 *     maxResultados - capacidade do array
 *
 *   Retorno (PDF 5.6 getMenu):
 *     >= 0                  - quantidade de pratos encontrados
 *     PRATOS_PARAM_INVALIDO - db==NULL, cnpj<=0 ou resultado==NULL
 */
int getMenu(AppDados *db, long long int cnpj,
            Prato *resultado, int maxResultados);

/*
 * getFeedPratos
 *   Seleciona PRATOS_FEED_QTD (20) pratos aleatorios nao avaliados
 *   pelo usuario logado (PDF 2.7). Usa Fisher-Yates shuffle.
 *
 *   Parametros:
 *     db            - banco em memoria
 *     cpf           - CPF do usuario logado (para filtrar avaliados)
 *     resultado     - array de Prato (minimo 20 slots)
 *     maxResultados - capacidade do array
 *
 *   Retorno (PDF 5.6 getFeedPratos):
 *     PRATOS_FEED_QTD (20)  - sucesso
 *     PRATOS_INSUFICIENTE   - menos de 20 pratos elegiveis
 *     PRATOS_PARAM_INVALIDO - db==NULL, resultado==NULL ou cpf<=0
 */
int getFeedPratos(AppDados *db, long long int cpf,
                  Prato *resultado, int maxResultados);

#endif /* PRATOS_H */