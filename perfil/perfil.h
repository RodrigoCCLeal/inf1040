/*
 * perfil.h
 * Modulo Perfil - Foodies
 *
 * Responsavel por: autenticar usuarios, criar perfis e consultar dados
 * de usuario. Tambem e responsavel por carregar e salvar o JSON de
 * usuarios (perfis.json), pois e o unico modulo que encapsula esses dados.
 *
 * Relacoes cliente-servidor:
 * PRINCIPAL (cliente) -> PERFIL (servidor): enterPerfil, criarPerfil
 * POSTAR, BUSCAR, FEED (clientes) -> PERFIL (servidor): getUsuario, obterCpfLogado
 *
 * Dados encapsulados:
 * Usuario(long long int cpf, char nome[], char senha[])
 */

#ifndef PERFIL_H
#define PERFIL_H

/* --- Limites comerciais e tamanhos realocados --- */
#define MAX_USUARIOS      500
#define TAM_NOME          101
#define TAM_SENHA          51

/* Perfil de usuario - Mantido publico para tráfego entre módulos */
typedef struct {
    long long int cpf;          /* identificador – 11 dígitos */
    char nome [TAM_NOME];
    char senha[TAM_SENHA];
} Usuario;

/* --- Nome do arquivo JSON deste modulo --- */
#define PERFIL_JSON "perfis.json"

/* --- Codigos de retorno - enterPerfil --- */
#define PERFIL_OK              0   /* login realizado com sucesso        */
#define PERFIL_DADOS_INVALIDOS 1   /* CPF nao encontrado ou senha errada */
#define PERFIL_PARAM_INVALIDO  2   /* cpf<=0 ou senha==NULL              */

/* --- Codigos de retorno - criarPerfil --- */
#define PERFIL_CRIADO          0   /* perfil criado com sucesso          */
#define PERFIL_VALORES_INV     1   /* CPF invalido, nome ou senha vazios */
#define PERFIL_JA_EXISTE       3   /* CPF ja cadastrado no banco         */

/* =================================================================
 * FUNCOES DE INTERFACE DE CONTROLE DE SESSAO (GETTERS / SETTERS)
 * ================================================================= */

/*
 * obterCpfLogado
 * Retorna o CPF do usuario atualmente autenticado no sistema.
 * Substitui o antigo acesso direto a: db->cpfLogado
 */
long long int obterCpfLogado(void);

/*
 * definirCpfLogado
 * Define ou limpa (passando 0) a sessao do usuario logado.
 */
void definirCpfLogado(long long int cpf);


/* =================================================================
 * FUNCOES DE I/O E GERENCIAMENTO DO BANCO DE DADOS OCULTO
 * ================================================================= */

/*
 * carregarPerfis
 * Le perfis.json e popula o vetor estatico interno deste arquivo .c.
 * Chamada pelo Principal no inicio da execucao.
 */
int carregarPerfis(void);

/*
 * salvarPerfis
 * Serializa o vetor estatico interno para perfis.json.
 * Chamada pelo Principal ao encerrar a execucao.
 */
int salvarPerfis(void);

/*
 * enterPerfil
 * Autentica o usuario localmente. Se bem-sucedido, ativa a sessao interna.
 */
int enterPerfil(long long int cpf, const char *senha);

/*
 * criarPerfil
 * Cria um novo usuario e o adiciona no armazenamento estatico interno.
 */
int criarPerfil(long long int cpf, const char *nome, const char *senha);

/*
 * getUsuario
 * Retorna um ponteiro de leitura para o Usuario com o CPF dado.
 */
Usuario *getUsuario(long long int cpf);

/* ---------------------------------------------------------------
 * obterNomeUsuario
 * Busca o nome de um usuario cadastrado no sistema a partir do CPF.
 * Caso o CPF nao seja encontrado, atribui um nome padrao ("Usuario").
 *
 * Pre-condicoes:  cpf > 0, nomeDestino != NULL e com espaco alocado
 * Pos-condicoes:  nomeDestino modificado com o nome recuperado ou padrao
 * --------------------------------------------------------------- */
void obterNomeUsuario(long long int cpf, char *nomeDestino);

#endif /* PERFIL_H */