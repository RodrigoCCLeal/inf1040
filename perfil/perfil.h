/*
 * perfil.h
 * Modulo Perfil - Foodies
 *
 * Responsavel por: autenticar usuarios, criar perfis e consultar dados
 * de usuario. Tambem e responsavel por carregar e salvar o JSON de
 * usuarios (perfis.json), pois e o unico modulo que encapsula esses dados.
 *
 * Relacoes cliente-servidor (PDF 3.2):
 *   PRINCIPAL (cliente) -> PERFIL (servidor): enterPerfil, criarPerfil
 *   POSTAR, BUSCAR, FEED (clientes) -> PERFIL (servidor): getUsuario
 *
 * Dados encapsulados:
 *   perfil(long long int cpf, char nome[], char senha[])
 *   Conhecido por: perfil
 *
 * Funcoes de abertura/fechamento do JSON (chamadas pelo Principal):
 *   carregarPerfis  - le perfis.json e popula db->usuarios[]
 *   salvarPerfis    - serializa db->usuarios[] para perfis.json
 *
 * Funcoes de acesso (PDF 3.4.8):
 *   enterPerfil  - autentica usuario (login)
 *   criarPerfil  - cria novo usuario
 *   getUsuario   - retorna dados de um usuario pelo CPF
 */

#ifndef PERFIL_H
#define PERFIL_H

#include "../dados/dados.h"

/* --- Nome do arquivo JSON deste modulo --- */
#define PERFIL_JSON "perfis.json"

/* --- Codigos de retorno - enterPerfil --- */
#define PERFIL_OK              0   /* login realizado com sucesso        */
#define PERFIL_DADOS_INVALIDOS 1   /* CPF nao encontrado ou senha errada */
#define PERFIL_PARAM_INVALIDO  2   /* db==NULL, cpf<=0 ou senha==NULL    */

/* --- Codigos de retorno - criarPerfil --- */
#define PERFIL_CRIADO          0   /* perfil criado com sucesso          */
#define PERFIL_VALORES_INV     1   /* CPF invalido, nome ou senha vazios */
/* PERFIL_PARAM_INVALIDO (2) tambem se aplica                           */
#define PERFIL_JA_EXISTE       3   /* CPF ja cadastrado no banco         */

/*
 * carregarPerfis
 *   Le perfis.json e popula db->usuarios[].
 *   Chamada pelo Principal no inicio da execucao.
 *   Se o arquivo nao existir, o banco comeca vazio (sem erro).
 *
 *   Retorno:
 *     0  - carregado com sucesso (ou arquivo inexistente)
 *    -1  - erro de leitura no arquivo
 */
int carregarPerfis(AppDados *db);

/*
 * salvarPerfis
 *   Serializa db->usuarios[] para perfis.json.
 *   Chamada pelo Principal ao encerrar a execucao.
 *
 *   Retorno:
 *     0  - salvo com sucesso
 *    -1  - erro de escrita
 */
int salvarPerfis(AppDados *db);

/*
 * enterPerfil
 *   Autentica o usuario: verifica se o CPF existe e a senha confere.
 *   Se autenticado, define db->cpfLogado = cpf.
 *
 *   Retorno (PDF 5.7 enterPerfil):
 *     PERFIL_OK              (0) - autenticado, cpfLogado atualizado
 *     PERFIL_DADOS_INVALIDOS (1) - CPF nao encontrado ou senha errada
 *     PERFIL_PARAM_INVALIDO  (2) - db==NULL, cpf<=0 ou senha==NULL
 */
int enterPerfil(AppDados *db, long long int cpf, const char *senha);

/*
 * criarPerfil
 *   Cria um novo usuario e o adiciona em db->usuarios[].
 *   Valida CPF (11 digitos), nome e senha nao vazios.
 *
 *   Retorno (PDF 5.7 criarPerfil):
 *     PERFIL_CRIADO         (0) - criado com sucesso
 *     PERFIL_VALORES_INV    (1) - CPF invalido (< 11 digitos),
 *                                 nome ou senha vazios
 *     PERFIL_PARAM_INVALIDO (2) - db==NULL, cpf<=0, nome==NULL
 *                                 ou senha==NULL
 *     PERFIL_JA_EXISTE      (3) - CPF ja cadastrado
 */
int criarPerfil(AppDados *db, long long int cpf,
                const char *nome, const char *senha);

/*
 * getUsuario
 *   Retorna ponteiro para o Usuario com o CPF dado.
 *   Operacao de somente leitura - nao modifica db.
 *
 *   Retorno (PDF 5.7 getUsuario):
 *     Usuario*  - ponteiro para o registro encontrado
 *     NULL      - CPF nao encontrado ou parametro invalido
 */
Usuario *getUsuario(AppDados *db, long long int cpf);

#endif /* PERFIL_H */