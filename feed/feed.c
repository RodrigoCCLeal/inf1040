/*
 * feed.c
 * Modulo Feed - Foodies
 *
 * Implementacao de enterFeed declarada em feed.h.
 *
 * ZERO I/O de arquivo neste modulo.
 * ZERO armazenamento de estado global ou estatico local.
 *
 * Dependencia: verificaLogin() do modulo que gerencia sessoes (ex: postar ou perfil).
 */

#include "feed.h"
#include "../postar/postar.h" /* Necessario para chamar verificaLogin */
#include <stddef.h>

/*
 * enterFeed
 * Valida o login via verificaLogin e libera o fluxo do feed.
 * Ver contrato completo em feed.h.
 */
int enterFeed(long long int cpf) {

    /* Parametros invalidos */
    if (cpf <= 0)
        return FEED_PARAM_INVALIDO;

    /* Delega verificacao de sessao sem expor acoplamento de dados */
    if (verificaLogin(cpf) != LOGIN_OK)
        return FEED_CPF_INVALIDO;

    return FEED_OK;
}