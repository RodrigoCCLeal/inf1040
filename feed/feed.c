/*
 * feed.c
 * Modulo Feed - Foodies
 *
 * Implementacao de enterFeed declarada em feed.h.
 *
 * ZERO I/O de arquivo neste modulo.
 * ZERO funcoes de recomendacao neste modulo — getFeedPratos e getFeedRest
 * pertencem aos modulos Pratos e Restaurante respectivamente,
 * que sao os servidores responsaveis por esses dados encapsulados.
 *
 * Dependencia: verificaLogin() do modulo Postar (PDF 3.4.8).
 */

#include "feed.h"
#include "../postar/postar.h"
#include <stddef.h>

/*
 * enterFeed
 *   Valida o login via verificaLogin e libera o fluxo do feed.
 *   Ver contrato completo em feed.h.
 */
int enterFeed(AppDados *db, long long int cpf) {

    /* Parametros invalidos */
    if (db == NULL || cpf <= 0)
        return FEED_PARAM_INVALIDO;

    /* Delega verificacao de sessao ao modulo Postar */
    if (verificaLogin(db, cpf) != LOGIN_OK)
        return FEED_CPF_INVALIDO;

    return FEED_OK;
}