/*
 * buscar.c
 * Modulo Buscar - Foodies
 *
 * Implementacao de enterBuscar declarada em buscar.h.
 *
 * ZERO I/O de arquivo neste modulo.
 * ZERO funcoes de busca neste modulo — getListaPratos e getListaRest
 * pertencem aos modulos Pratos e Restaurante respectivamente,
 * que sao os servidores responsaveis por esses dados encapsulados.
 *
 * Dependencia: verificaLogin() do modulo Postar (PDF 3.4.8).
 */

#include "buscar.h"
#include "../postar/postar.h"
#include <stddef.h>

/*
 * enterBuscar
 *   Valida o login via verificaLogin e libera o fluxo de busca.
 *   Ver contrato completo em buscar.h.
 */
int enterBuscar(AppDados *db, long long int cpf) {

    /* Parametros invalidos */
    if (db == NULL || cpf <= 0)
        return BUSCAR_PARAM_INVALIDO;

    /* Delega verificacao de sessao ao modulo Postar */
    if (verificaLogin(db, cpf) != LOGIN_OK)
        return BUSCAR_CPF_INVALIDO;

    return BUSCAR_OK;
}