/*
 * feed.h
 * Modulo Feed - Foodies
 *
 * Responsavel por: validar o acesso do usuario logado a tela de feed.
 * As recomendacoes aleatorias sao delegadas aos modulos Pratos e
 * Restaurante, que sao os servidores responsaveis por esses dados.
 *
 * Relacoes cliente-servidor (PDF 3.2):
 *   FEED (cliente) -> PRATOS       (servidor): getFeedPratos
 *   FEED (cliente) -> RESTAURANTES (servidor): getFeedRest
 *   FEED (cliente) -> POSTAR       (servidor): verificaLogin
 *
 * REGRA DE I/O: este modulo nao abre nem fecha arquivos.
 * Todas as operacoes sao feitas sobre AppDados* (banco em memoria).
 *
 * Funcao de acesso (PDF 3.4.4):
 *   enterFeed - valida login e libera acesso a tela de feed
 */

#ifndef FEED_H
#define FEED_H

#include "../dados/dados.h"

/* --- Codigos de retorno - enterFeed --- */
#define FEED_OK             0   /* acesso liberado                    */
#define FEED_CPF_INVALIDO   1   /* usuario nao esta logado            */
#define FEED_PARAM_INVALIDO 2   /* db==NULL ou cpf<=0                 */

/*
 * enterFeed
 *   Verifica se o usuario esta logado (via verificaLogin do modulo Postar)
 *   e libera o fluxo do feed de recomendacoes. As recomendacoes
 *   propriamente ditas (getFeedPratos, getFeedRest) sao chamadas
 *   diretamente pelos modulos Pratos e Restaurante.
 *
 *   Parametros:
 *     db  - banco em memoria (nao pode ser NULL)
 *     cpf - CPF do usuario solicitante (long long int, deve ser > 0)
 *
 *   Retorno (PDF 5.3):
 *     FEED_OK             (0) - login valido, acesso liberado
 *     FEED_CPF_INVALIDO   (1) - CPF nao esta logado na sessao
 *     FEED_PARAM_INVALIDO (2) - db==NULL ou cpf<=0
 */
int enterFeed(AppDados *db, long long int cpf);

#endif /* FEED_H */