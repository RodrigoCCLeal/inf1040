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

/*
 * Objetivo: validar o acesso do usuario logado a tela de feed de recomendacoes.
 * Descricao:
 *     O sistema deve verificar se o CPF fornecido esta autenticado na sessao
 *     ativa, delegando a verificacao para verificaLogin() do modulo Postar.
 *     Nao acessa arquivos nem altera qualquer estado interno.
 * Acoplamento:
 *     Parametros:
 *       • long long int cpf - CPF do usuario solicitante (deve ser > 0).
 *     Retornos:
 *       • FEED_OK (0)             - login valido, acesso liberado.
 *       • FEED_CPF_INVALIDO (1)   - CPF nao esta logado na sessao ativa.
 *       • FEED_PARAM_INVALIDO (2) - cpf <= 0.
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • cpf > 0.
 *       • Uma sessao pode ou nao estar ativa no modulo Postar.
 * Assertivas de Saida:
 *   • FEED_OK indica que o fluxo de feed pode prosseguir.
 *   • Nenhum estado interno e alterado por esta funcao.
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