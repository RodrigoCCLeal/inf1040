/*
 * feed.c
 * Módulo Feed – Foodies
 *
 * Implementação das funções de acesso declaradas em feed.h.
 *
 * REGRA FUNDAMENTAL: ZERO I/O de arquivo neste módulo.
 * Todas as operações leem de db->pratos[] e db->restaurantes[] (memória).
 * O JSON é de responsabilidade exclusiva da main.
 *
 * Aleatoriedade: usa Fisher-Yates shuffle sobre array de índices elegíveis,
 * garantindo seleção sem repetição e distribuição uniforme.
 *
 * Dependência externa: verificaLogin() do módulo Postar (PDF §3.4.8).
 */

#include "feed.h"
#include "../postar/postar.h"   /* verificaLogin */

#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <time.h>


/* ═══════════════════════════════════════════════════════════════
 * FUNÇÕES DE ACESSO PÚBLICAS (PDF §3.4.4)
 * ═══════════════════════════════════════════════════════════════ */

/*
 * enterFeed
 *   Valida o login via verificaLogin e libera o fluxo do feed.
 *   Ver contrato completo em feed.h.
 */
int enterFeed(AppDados *db, long long int cpf) {

    /* Parâmetros inválidos */
    if (db == NULL || cpf <= 0)
        return FEED_PARAM_INVALIDO;

    /* Delega verificação de sessão ao módulo Postar */
    if (verificaLogin(db, cpf) != LOGIN_OK)
        return FEED_CPF_INVALIDO;

    /*
     * Em produção, aqui a main navegaria para a tela de feed.
     * Este módulo é lógica pura: sinaliza sucesso para o Principal.
     */
    return FEED_OK;
}
