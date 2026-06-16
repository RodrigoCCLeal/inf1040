/*
 * buscar.c
 * Modulo Buscar - Foodies
 *
 * Implementacao de enterBuscar declarada em buscar.h.
 *
 * ZERO I/O de arquivo neste modulo.
 * ZERO armazenamento de estado global ou estatico.
 *
 * Dependencia: verificaLogin() do modulo que gerencia sessões (ex: postar ou perfil).
 */

#include "buscar.h"
#include "../postar/postar.h" /* Necessario para chamar verificaLogin */
#include <stddef.h>

/*
 * enterBuscar
 * Valida o login via verificaLogin e libera o fluxo de busca.
 * Ver contrato completo em buscar.h.
 */
int enterBuscar(long long int cpf) {

    /* Parametros invalidos */
    if (cpf <= 0)
        return BUSCAR_PARAM_INVALIDO;

    /* Delega verificacao de sessao de forma limpa, sem expor structs internas */
    if (verificaLogin(cpf) != LOGIN_OK)
        return BUSCAR_CPF_INVALIDO;

    return BUSCAR_OK;
}