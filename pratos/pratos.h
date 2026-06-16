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
 * Relacoes cliente-servidor:
 * BUSCAR      (cliente) -> PRATOS (servidor): getListaPratos
 * FEED        (cliente) -> PRATOS (servidor): getFeedPratos
 * AVALIACAO   (cliente) -> PRATOS (servidor): getPratos
 * RESTAURANTE (cliente) -> PRATOS (servidor): getPratosPorCnpj
 */

#ifndef PRATOS_H
#define PRATOS_H

/* --- Limites comerciais e tamanhos realocados --- */
#define MAX_PRATOS       2000
#define TAM_NOME          101
#define TAM_COMENTARIO    401
#define TAM_ENDERECO      201

/* Prato - Estrutura publica para trafego entre modulos */
typedef struct {
    int           idPrato;      /* identificador principal             */
    char          nome     [TAM_NOME];
    char          descricao[TAM_COMENTARIO];
    long long int cnpjRestaurante;          /* FK → Restaurante.cnpj  */
    char          enderecoRestaurante[TAM_ENDERECO];
} Prato;

/* --- Nome do arquivo JSON deste modulo (somente leitura) --- */
#define PRATOS_JSON "pratos.json"

/* --- Quantidade do feed de pratos --- */
#define PRATOS_FEED_QTD 20

/* --- Codigos de retorno --- */
#define PRATOS_NOME_INVALIDO  -1   /* nome==NULL ou resultado==NULL       */
#define PRATOS_PARAM_INVALIDO -2   /* parametro invalido                  */
#define PRATOS_NAO_ENCONTRADO -3   /* idPrato nao existe no banco         */
#define PRATOS_INSUFICIENTE   -4   /* menos de 20 pratos elegiveis        */
/* Retorno >= 0 indica quantidade de itens copiados                       */

/* =================================================================
 * FUNCOES DE INTERFACE ADICIONAIS (API ENTRE MODULOS)
 * ================================================================= */

/*
 * getPratosPorCnpj
 * Atende diretamente o modulo Restaurante para montar seu Menu.
 */
int getPratosPorCnpj(long long int cnpj, Prato *resultado, int maxResultados);

/* =================================================================
 * FUNCOES DE I/O E GERENCIAMENTO DO BANCO DE DADOS OCULTO
 * ================================================================= */

/*
 * carregarPratos
 * Le pratos.json e popula o vetor estatico interno deste arquivo .c.
 * Chamada UMA UNICA VEZ pelo Principal no inicio da execucao.
 */
int carregarPratos(void);

/*
 * getPratos
 * Retorna um ponteiro para o prato correspondente ao idPrato informado.
 */
Prato *getPratos(int idPrato);

/*
 * getListaPratos
 * Retorna pratos cujo nome contenha nome_prato como substring.
 */
int getListaPratos(const char *nome_prato, Prato *resultado, int maxResultados);

/*
 * getMenu
 * Mantido por compatibilidade. Retorna todos os pratos de um restaurante.
 */
int getMenu(long long int cnpj, Prato *resultado, int maxResultados);

/*
 * getFeedPratos
 * Seleciona 20 pratos aleatorios nao avaliados pelo usuario logado.
 */
int getFeedPratos(long long int cpf, Prato *resultado, int maxResultados);

/* ---------------------------------------------------------------
 * listarNomesPratos
 * Exibe na saida padrao o ID e o Nome de todos os pratos cadastrados.
 *
 * Pre-condicoes:  Nenhuma
 * Pos-condicoes:  Garante o fluxo de execucao completo sem efeitos colaterais
 * --------------------------------------------------------------- */
void listarNomesPratos(void);

/* ---------------------------------------------------------------
 * obterMediaPrato
 * Calcula a media aritmetica das notas de um prato pelo seu ID.
 * Modifica o float apontado por mediaDestino e retorna a quantidade
 * total de avaliacoes computadas para o calculo.
 *
 * Pre-condicoes:  idPrato > 0, mediaDestino != NULL
 * Pos-condicoes:  Retorna a qtd de avaliacoes (>= 0) e mediaDestino entre [0.0, 5.0]
 * --------------------------------------------------------------- */
int obterMediaPrato(int idPrato, float *mediaDestino);

#endif /* PRATOS_H */