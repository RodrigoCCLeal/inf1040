/*
 * feed.h
 * Modulo Feed - Foodies
 *
 * Responsavel por: validar o acesso do usuario logado a tela de feed.
 * As recomendacoes aleatorias sao delegadas aos modulos Pratos e
 * Restaurante, que sao os servidores responsaveis por esses dados.
 *
 * Relacoes cliente-servidor:
 * FEED (cliente) -> PRATOS       (servidor): getFeedPratos
 * FEED (cliente) -> RESTAURANTES (servidor): getFeedRest
 * FEED (cliente) -> POSTAR/PERFIL(servidor): verificaLogin
 *
 * REGRA DE I/O: este modulo nao abre nem fecha arquivos.
 */

#ifndef FEED_H
#define FEED_H

/* --- Codigos de retorno - enterFeed --- */
#define FEED_OK             0   /* acesso liberado                    */
#define FEED_CPF_INVALIDO   1   /* usuario nao esta logado            */
#define FEED_PARAM_INVALIDO 2   /* parametro invalido (cpf<=0)        */

/*
 * enterFeed
 * Verifica se o usuario esta logado (via verificaLogin)
 * e libera o fluxo do feed de recomendacoes.
 *
 * Parametros:
 * cpf - CPF do usuario solicitante (long long int, deve ser > 0)
 *
 * Retorno:
 * FEED_OK             (0) - login valido, acesso liberado
 * FEED_CPF_INVALIDO   (1) - CPF nao esta logado na sessao
 * FEED_PARAM_INVALIDO (2) - cpf<=0
 */
int enterFeed(long long int cpf);

#endif /* FEED_H */