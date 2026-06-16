/*
 * buscar.c
 * Módulo Buscar – Foodies
 *
 * Implementação das funções de acesso declaradas em buscar.h.
 *
 * REGRA FUNDAMENTAL: ZERO I/O de arquivo neste módulo.
 * Todas as operações leem de db->pratos[] e db->restaurantes[] (memória).
 * O JSON é de responsabilidade exclusiva da main.
 *
 * Dependência externa: verificaLogin() do módulo Postar (PDF §3.4.8),
 * usada por enterBuscar para validar a sessão ativa.
 */

#include "buscar.h"
#include "../postar/postar.h"   /* verificaLogin */

#include <string.h>
#include <ctype.h>
#include <stddef.h>


/* ═══════════════════════════════════════════════════════════════
 * FUNÇÕES DE ACESSO PÚBLICAS (PDF §3.4.3)
 * ═══════════════════════════════════════════════════════════════ */

/*
 * enterBuscar
 *   Valida o login via verificaLogin e libera o fluxo de busca.
 *   Ver contrato completo em buscar.h.
 */
int enterBuscar(AppDados *db, long long int cpf) {

    /* Parâmetros inválidos */
    if (db == NULL || cpf <= 0)
        return BUSCAR_PARAM_INVALIDO;

    /* Delega verificação de sessão ao módulo Postar */
    if (verificaLogin(db, cpf) != LOGIN_OK)
        return BUSCAR_CPF_INVALIDO;

    /*
     * Em produção, aqui a main navegaria para a tela de busca.
     * Este módulo é lógica pura: sinaliza sucesso para o Principal.
     */
    return BUSCAR_OK;
}

