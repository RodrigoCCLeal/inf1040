/*
 * dados.h
 * Foodies – Estruturas de dados compartilhadas em memória
 *
 * RESPONSABILIDADE DO MÓDULO PRINCIPAL (main):
 *   - Abrir o JSON no início da execução → popular estas structs
 *   - Fechar/salvar o JSON ao encerrar  → serializar estas structs
 *
 * Todos os outros módulos recebem ponteiro para AppDados e
 * operam exclusivamente em memória. NENHUM outro módulo abre
 * ou fecha arquivos JSON.
 *
 * OBS (PDF §3.3): structs de Prato, Avaliação e Restaurante
 * devem estar declaradas no .h para acoplamento entre módulos.
 */

#ifndef DADOS_H
#define DADOS_H

/* ─── Limites de capacidade ──────────────────────────────────── */
#define MAX_USUARIOS      500
#define MAX_RESTAURANTES  500
#define MAX_PRATOS       2000
#define MAX_AVALIACOES   5000

#define TAM_NOME          101
#define TAM_SENHA          51
#define TAM_ENDERECO      201
#define TAM_COMENTARIO    401   /* 400 chars úteis + '\0' */
#define TAM_CNPJ           15   /* "XX.XXX.XXX/XXXX-XX" + '\0' */

/* ═══════════════════════════════════════════════════════════════
 * STRUCTS DAS ENTIDADES
 * Declaradas aqui para que Prato, Avaliacao e Restaurante sejam
 * acessíveis nos .h dos módulos que as utilizam (OBS4).
 * ═══════════════════════════════════════════════════════════════ */

/* Perfil de usuário */
typedef struct {
    long long int cpf;          /* identificador – 11 dígitos          */
    char nome [TAM_NOME];
    char senha[TAM_SENHA];
} Usuario;

/* Restaurante */
typedef struct {
    long long int cnpj;         /* identificador principal             */
    char endereco[TAM_ENDERECO];
    char nome    [TAM_NOME];
} Restaurante;

/* Prato */
typedef struct {
    int           idPrato;      /* identificador principal             */
    char          nome     [TAM_NOME];
    char          descricao[TAM_COMENTARIO];
    long long int cnpjRestaurante;          /* FK → Restaurante.cnpj  */
    char          enderecoRestaurante[TAM_ENDERECO];
} Prato;

/* Avaliação */
typedef struct {
    int           idAval;       /* identificador principal             */
    float         nota;         /* inteiro 0-5, armazenado como float  */
    char          comentario[TAM_COMENTARIO];
    long long int cpf;          /* FK → Usuario.cpf                   */
    int           idPrato;      /* FK → Prato.idPrato                 */
} Avaliacao;

/* ═══════════════════════════════════════════════════════════════
 * BANCO COMPLETO EM MEMÓRIA
 * Alocado pelo Principal; passado por ponteiro a todos os módulos.
 * ═══════════════════════════════════════════════════════════════ */
typedef struct {
    Usuario     usuarios    [MAX_USUARIOS];
    Restaurante restaurantes[MAX_RESTAURANTES];
    Prato       pratos      [MAX_PRATOS];
    Avaliacao   avaliacoes  [MAX_AVALIACOES];

    /* Contadores de registros válidos */
    int nUsuarios;
    int nRestaurantes;
    int nPratos;
    int nAvaliacoes;

    /* Geradores de ID auto-increment */
    int proximoIdPrato;
    int proximoIdAval;

    /* CPF do usuário atualmente logado (0 = nenhum) */
    long long int cpfLogado;
} AppDados;

#endif /* DADOS_H */
