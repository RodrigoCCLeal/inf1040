/*
 * feed.h
 * Módulo Feed – Foodies
 *
 * Responsável por: fornecer ao usuário logado recomendações aleatórias
 * de pratos (20) e restaurantes (6), excluindo pratos já avaliados
 * pelo usuário (PDF §2.7).
 *
 * Relações cliente-servidor (PDF §3.2):
 *   FEED (cliente) → PRATOS       (servidor): getFeedPratos
 *   FEED (cliente) → RESTAURANTES (servidor): getFeedRest
 *   FEED (cliente) → PERFIL/POSTAR(servidor): verificaLogin
 *
 * REGRA DE I/O: este módulo NÃO abre nem fecha arquivos JSON.
 * Todas as operações são feitas sobre AppDados* (banco em memória).
 * O JSON é aberto/fechado exclusivamente pela main.
 *
 * Funções de acesso definidas no PDF §3.4.4:
 *   enterFeed    – valida login e libera acesso à tela de feed
 *
 * Funções de recomendação aleatória (PDF §3.4.6 e §3.4.7):
 *   getFeedPratos – retorna até 20 pratos aleatórios não avaliados pelo usuário
 *   getFeedRest   – retorna até 6 restaurantes aleatórios
 */

#ifndef FEED_H
#define FEED_H

#include "../dados/dados.h"

/* ─── Constantes de quantidade do feed (PDF §2.7) ───────────── */
#define FEED_QTD_PRATOS       20   /* pratos aleatórios por chamada      */
#define FEED_QTD_RESTAURANTES  6   /* restaurantes aleatórios por chamada */

/* ─── Códigos de retorno – enterFeed ────────────────────────── */
#define FEED_OK               0   /* acesso liberado à tela de feed     */
#define FEED_CPF_INVALIDO     1   /* usuário não está logado            */
#define FEED_PARAM_INVALIDO   2   /* db==NULL ou cpf<=0                 */

/* ─── Códigos de retorno – getFeedPratos / getFeedRest ──────── */
#define FEED_INSUFICIENTE    -1   /* banco tem menos itens que o mínimo */
#define FEED_ERRO_PARAM      -2   /* db==NULL ou resultado==NULL         */
/* Retorno >= 0 indica a quantidade de itens copiados para o array      */

/*
 * enterFeed
 *   Verifica se o usuário está logado (via verificaLogin do módulo Postar)
 *   e libera o fluxo do feed de recomendações.
 *
 *   Parâmetros:
 *     db  – banco em memória (não pode ser NULL)
 *     cpf – CPF do usuário solicitante (long long int, deve ser > 0)
 *
 *   Retorno (PDF §5.3):
 *     FEED_OK             (0) – login válido, acesso liberado
 *     FEED_CPF_INVALIDO   (1) – CPF não está logado na sessão
 *     FEED_PARAM_INVALIDO (2) – db==NULL ou cpf<=0
 */
int enterFeed(AppDados *db, long long int cpf);

/*
 * getFeedPratos
 *   Seleciona aleatoriamente até FEED_QTD_PRATOS (20) pratos do banco,
 *   excluindo os pratos já avaliados pelo usuário logado (PDF §2.7).
 *   Usa Fisher-Yates shuffle sobre os índices elegíveis.
 *
 *   Parâmetros:
 *     db           – banco em memória
 *     cpf          – CPF do usuário logado (para filtrar avaliados)
 *     resultado    – array de Prato alocado pelo chamador (mín. 20 slots)
 *     maxResultados– capacidade do array (deve ser >= FEED_QTD_PRATOS)
 *
 *   Retorno (PDF §5.6 getFeedPratos):
 *     >= 0              – quantidade de pratos copiados (pode ser < 20
 *                         se o banco tiver menos elegíveis)
 *     FEED_INSUFICIENTE(-1) – banco tem menos de 20 pratos elegíveis
 *                             (nulo segundo PDF)
 *     FEED_ERRO_PARAM  (-2) – db==NULL, resultado==NULL ou cpf<=0
 */
int getFeedPratos(AppDados *db, long long int cpf,
                  Prato *resultado, int maxResultados);

/*
 * getFeedRest
 *   Seleciona aleatoriamente FEED_QTD_RESTAURANTES (6) restaurantes do banco.
 *   Usa Fisher-Yates shuffle sobre todos os restaurantes disponíveis.
 *
 *   Parâmetros:
 *     db           – banco em memória
 *     resultado    – array de Restaurante alocado pelo chamador (mín. 6 slots)
 *     maxResultados– capacidade do array (deve ser >= FEED_QTD_RESTAURANTES)
 *
 *   Retorno (PDF §5.5 getFeedRest):
 *     >= 0              – quantidade de restaurantes copiados (= 6 em caso normal)
 *     FEED_INSUFICIENTE(-1) – banco tem menos de 6 restaurantes (nulo segundo PDF)
 *     FEED_ERRO_PARAM  (-2) – db==NULL ou resultado==NULL
 */
int getFeedRest(AppDados *db, Restaurante *resultado, int maxResultados);

#endif /* FEED_H */
