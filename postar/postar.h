/*
 * postar.h
 * Módulo Postar – Foodies
 *
 * Responsável por: controlar acesso à tela de postagem e verificar
 * autenticação do usuário na sessão atual.
 *
 * REGRA DE I/O: nenhuma função deste módulo abre ou fecha arquivos.
 * A sessão ativa é lida de db->cpfLogado (campo do banco em memória).
 *
 * Funções de acesso definidas no PDF §3.4.2:
 *   enterPostar   – valida login e libera acesso à tela de postagem
 *
 * Função compartilhada definida no PDF §3.4.8 (usada por Buscar, Feed):
 *   verificaLogin – verifica se o CPF fornecido está logado
 */

#ifndef POSTAR_H
#define POSTAR_H

#include "../dados/dados.h"

/* ─── Códigos de retorno – enterPostar ───────────────────────── */
#define POSTAR_OK             0   /* acesso liberado                    */
#define POSTAR_CPF_INVALIDO   1   /* usuário não está logado            */
#define POSTAR_PARAM_INVALIDO 2   /* db==NULL ou cpf<=0                 */

/* ─── Códigos de retorno – verificaLogin ─────────────────────── */
#define LOGIN_OK              0   /* CPF confere com a sessão ativa     */
#define LOGIN_OUTRO_CPF       1   /* sem sessão ou CPF diferente        */

/*
 * verificaLogin
 *   Compara `cpf` com db->cpfLogado.
 *   Usada pelos módulos Buscar, Postar e Feed (PDF §3.4.8).
 *   Não acessa arquivos.
 *
 *   Parâmetros:
 *     db  – banco em memória (não pode ser NULL)
 *     cpf – CPF a verificar (long long int, deve ser > 0)
 *
 *   Retorno (PDF §5.7 verificaLogin):
 *     LOGIN_OK        (0) – cpf == db->cpfLogado (está logado)
 *     LOGIN_OUTRO_CPF (1) – não está logado ou CPF diferente
 */
int verificaLogin(AppDados *db, long long int cpf);

/*
 * enterPostar
 *   Valida o login via verificaLogin e libera o fluxo de postagem.
 *
 *   Parâmetros:
 *     db  – banco em memória
 *     cpf – CPF do usuário que quer postar (long long int)
 *
 *   Retorno (PDF §5.1):
 *     POSTAR_OK             (0) – login válido, acesso liberado
 *     POSTAR_CPF_INVALIDO   (1) – usuário não está logado
 *     POSTAR_PARAM_INVALIDO (2) – db==NULL ou cpf<=0
 */
int enterPostar(AppDados *db, long long int cpf);

#endif /* POSTAR_H */
