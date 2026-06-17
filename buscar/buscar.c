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

/*
 * Objetivo: validar o acesso do usuario logado a tela de busca de pratos e restaurantes.
 * Descricao:
 *     O sistema deve verificar se o CPF fornecido esta autenticado na sessao
 *     ativa, delegando a verificacao para verificaLogin() do modulo Postar.
 *     Nao acessa arquivos nem altera qualquer estado interno.
 * Acoplamento:
 *     Parametros:
 *       • long long int cpf - CPF do usuario solicitante (deve ser > 0).
 *     Retornos:
 *       • BUSCAR_OK (0)             - login valido, acesso liberado.
 *       • BUSCAR_CPF_INVALIDO (1)   - CPF nao esta logado na sessao ativa.
 *       • BUSCAR_PARAM_INVALIDO (2) - cpf <= 0.
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • cpf > 0.
 *       • Uma sessao pode ou nao estar ativa no modulo Postar.
 * Assertivas de Saida:
 *   • BUSCAR_OK indica que o fluxo de busca pode prosseguir.
 *   • Nenhum estado interno e alterado por esta funcao.
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