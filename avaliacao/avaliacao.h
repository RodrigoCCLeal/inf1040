/*
 * avaliacao.h
 * Modulo de Avaliacao – Foodies
 *
 * Responsavel por: criar, editar, consultar e persistir avaliacoes de pratos.
 *
 * Relacoes cliente-servidor:
 * UI / PRINCIPAL -> AVALIACAO: postAval, editAval, carregarAvaliacoes, salvarAvaliacoes
 * PRATOS (cliente) -> AVALIACAO (servidor): verificarSeAvaliado
 */

#ifndef AVALIACAO_H
#define AVALIACAO_H

/* --- Limites comerciais e tamanhos realocados --- */
#define MAX_AVALIACOES    5000
#define TAM_COMENTARIO     401

/* Avaliacao - Estrutura publica para trafego entre modulos */
typedef struct {
    int           idAval;       /* identificador unico */
    float         nota;         /* numero inteiro de 0 a 5 */
    long long int cpf;          /* autor da avaliacao (FK -> Usuario) */
    int           idPrato;      /* prato avaliado (FK -> Prato) */
    char          comentario[TAM_COMENTARIO];
} Avaliacao;

/* --- Nome do arquivo JSON deste modulo --- */
#define AVALIACAO_JSON "avaliacoes.json"

/* --- Codigos de retorno --- */
#define AVAL_OK                0   /* sucesso (com comentario)            */
#define AVAL_OK_SEM_TXT        1   /* sucesso (sem comentario)            */
#define AVAL_ERRO_ID_PRATO     2   /* idPrato nao existe no banco         */
#define AVAL_ERRO_NOTA         4   /* nota fora de [0,5] ou nao-inteira   */
#define AVAL_PARAM_INVALIDO   -1   /* ponteiro nulo ou valor ilegal       */
#define AVAL_ERRO_EDICAO      -2   /* avaliacao nao encontrada            */
#define AVAL_ERRO_PERMISSAO   -3   /* CPF nao e o autor da avaliacao      */
#define AVAL_BANCO_CHEIO      -4   /* MAX_AVALIACOES atingido             */

/* =================================================================
 * FUNCOES DE INTERFACE ADICIONAIS (API ENTRE MODULOS)
 * ================================================================= */

/*
 * verificarSeAvaliado
 * Atende diretamente o modulo Pratos para filtrar o feed de recomendacoes.
 * Retorna 1 se o CPF ja avaliou o idPrato, ou 0 caso contrario.
 */
int verificarSeAvaliado(long long int cpf, int idPrato);

/* =================================================================
 * FUNCOES DE I/O E GERENCIAMENTO DO BANCO DE DADOS OCULTO
 * ================================================================= */

/*
 * carregarAvaliacoes
 * Le avaliacoes.json e popula o vetor estatico interno.
 */
int carregarAvaliacoes(void);

/*
 * salvarAvaliacoes
 * Serializa o vetor estatico interno de avaliacoes para avaliacoes.json.
 */
int salvarAvaliacoes(void);

/*
 * postAval
 * Cria uma avaliacao e a insere no banco em memoria local.
 */
int postAval(long long int cpf, int idPrato, const char *txt, float nota);

/*
 * verAval
 * Copia para resultado[] todas as avaliacoes do prato indicado.
 */
int verAval(int idPrato, Avaliacao *resultado, int maxResultados);

/*
 * editAval
 * Edita nota e/ou comentario de uma avaliacao existente.
 */
int editAval(int idAval, long long int cpf, const char *novaTxt, float novaNota);

/* ---------------------------------------------------------------
 * imprimirAvaliacoesUsuario
 * Varre o repositorio de avaliacoes e imprime no terminal todos
 * os registros vinculados ao CPF fornecido.
 *
 * Pre-condicoes:  cpf > 0
 * Pos-condicoes:  Retorna o total de avaliacoes impressas (>= 0)
 * --------------------------------------------------------------- */
int imprimirAvaliacoesUsuario(long long int cpf);

#endif /* AVALIACAO_H */