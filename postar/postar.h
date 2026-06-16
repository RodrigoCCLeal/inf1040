/*
 * postar.h
 * Módulo Postar – Foodies
 *
 * Responsável por: controlar acesso à tela de postagem e gerenciar/verificar
 * a autenticação do usuário na sessão ativa do programa.
 *
 * REGRA DE I/O: nenhuma função deste módulo abre ou fecha arquivos.
 * A sessão ativa é controlada e encapsulada estaticamente dentro deste módulo.
 *
 * Funções de acesso:
 * enterPostar   – valida login e libera acesso à tela de postagem
 *
 * Função compartilhada (usada por Buscar, Feed, Avaliacao):
 * verificaLogin – verifica se o CPF fornecido está logado na sessão ativa
 */

#ifndef POSTAR_H
#define POSTAR_H

/* ─── Códigos de retorno – enterPostar ───────────────────────── */
#define POSTAR_OK             0   /* acesso liberado                    */
#define POSTAR_CPF_INVALIDO   1   /* usuário não está logado            */
#define POSTAR_PARAM_INVALIDO 2   /* parâmetro inválido (cpf<=0)        */

/* ─── Códigos de retorno – verificaLogin ─────────────────────── */
#define LOGIN_OK              0   /* CPF confere com a sessão ativa     */
#define LOGIN_OUTRO_CPF       1   /* sem sessão ou CPF diferente        */

/* =================================================================
 * FUNÇÕES DE INTERFACE ADICIONAIS (API ENTRE MÓDULOS)
 * ================================================================= */

/*
 * definirSessaoAtual
 * Define qual usuário está logado no sistema após uma autenticação bem-sucedida.
 * Chamada pelo módulo de Perfil/Login.
 */
void definirSessaoAtual(long long int cpf);

/*
 * encerrarSessaoAtual
 * Limpa a sessão ativa (Logout). Chamada pelo módulo de Perfil.
 */
void encerrarSessaoAtual(void);

/* =================================================================
 * FUNÇÕES DE ACESSO PÚBLICAS
 * ================================================================= */

/*
 * verificaLogin
 * Compara `cpf` com a sessão ativa mantida internamente.
 * Usada pelos módulos Buscar, Postar, Feed e Avaliacao.
 */
int verificaLogin(long long int cpf);

/*
 * enterPostar
 * Valida o login via verificaLogin e libera o fluxo de postagem.
 *
 * Parâmetros:
 * cpf – CPF do usuário que quer postar (long long int)
 *
 * Retorno:
 * POSTAR_OK             (0) – login válido, acesso liberado
 * POSTAR_CPF_INVALIDO   (1) – usuário não está logado
 * POSTAR_PARAM_INVALIDO (2) – cpf<=0
 */
int enterPostar(long long int cpf);

#endif /* POSTAR_H */