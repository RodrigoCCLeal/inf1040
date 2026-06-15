/*
 * postar.c
 * Módulo Postar – Foodies
 *
 * Implementação do controle de acesso à postagem e verificação de sessão.
 *
 * REGRA FUNDAMENTAL: ZERO I/O de arquivo neste módulo.
 * A sessão ativa está em db->cpfLogado, carregado pela main.
 * Não há cache estático: cada chamada lê diretamente do banco passado.
 */

#include "postar.h"
#include <stddef.h>

/* ═══════════════════════════════════════════════════════════════
 * FUNÇÕES DE ACESSO PÚBLICAS (PDF §3.4.2 e §3.4.8)
 * ═══════════════════════════════════════════════════════════════ */

/*
 * verificaLogin
 *   Verifica se o CPF fornecido corresponde à sessão ativa em memória.
 *   A sessão está simplesmente em db->cpfLogado; nenhum arquivo é lido.
 *   Ver contrato completo em postar.h.
 */
int verificaLogin(AppDados *db, long long int cpf) {

    /* Parâmetros inválidos → usuário não está logado */
    if (db == NULL || cpf <= 0)
        return LOGIN_OUTRO_CPF;

    /* Compara CPF com a sessão ativa no banco em memória */
    if (db->cpfLogado != 0 && db->cpfLogado == cpf)
        return LOGIN_OK;

    return LOGIN_OUTRO_CPF;
}

/*
 * enterPostar
 *   Valida o login e libera acesso à funcionalidade de postagem.
 *   Delega verificação de sessão a verificaLogin.
 *   Ver contrato completo em postar.h.
 */
int enterPostar(AppDados *db, long long int cpf) {

    /* Parâmetros inválidos */
    if (db == NULL || cpf <= 0)
        return POSTAR_PARAM_INVALIDO;

    /* Verificar autenticação */
    if (verificaLogin(db, cpf) != LOGIN_OK)
        return POSTAR_CPF_INVALIDO;

    /*
     * Em produção, aqui a main navegaria para a tela de postagem.
     * Este módulo é lógica pura: sinaliza sucesso para o Principal.
     */
    return POSTAR_OK;
}
