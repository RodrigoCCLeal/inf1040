/*
 * buscar.h
 * Modulo Buscar - Foodies
 *
 * Responsavel por: validar o acesso do usuario logado a tela de busca.
 * As buscas em si sao delegadas aos modulos Pratos e Restaurante,
 * que sao os servidores responsaveis por esses dados.
 *
 * Relacoes cliente-servidor (PDF 3.2):
 *   BUSCAR (cliente) -> PRATOS       (servidor): getListaPratos
 *   BUSCAR (cliente) -> RESTAURANTES (servidor): getListaRest
 *   BUSCAR (cliente) -> POSTAR       (servidor): verificaLogin
 *
 * REGRA DE I/O: este modulo nao abre nem fecha arquivos.
 * Todas as operacoes sao feitas sobre AppDados* (banco em memoria).
 *
 * Funcao de acesso (PDF 3.4.3):
 *   enterBuscar - valida login e libera acesso a tela de busca
 */

#ifndef BUSCAR_H
#define BUSCAR_H

#include "../dados/dados.h"

/* --- Codigos de retorno - enterBuscar --- */
#define BUSCAR_OK             0   /* acesso liberado                    */
#define BUSCAR_CPF_INVALIDO   1   /* usuario nao esta logado            */
#define BUSCAR_PARAM_INVALIDO 2   /* db==NULL ou cpf<=0                 */

/*
 * enterBuscar
 *   Verifica se o usuario esta logado (via verificaLogin do modulo Postar)
 *   e libera o fluxo de busca. As buscas propriamente ditas
 *   (getListaPratos, getListaRest) sao chamadas diretamente
 *   pelos modulos Pratos e Restaurante.
 *
 *   Parametros:
 *     db  - banco em memoria (nao pode ser NULL)
 *     cpf - CPF do usuario solicitante (long long int, deve ser > 0)
 *
 *   Retorno (PDF 5.2):
 *     BUSCAR_OK             (0) - login valido, acesso liberado
 *     BUSCAR_CPF_INVALIDO   (1) - CPF nao esta logado na sessao
 *     BUSCAR_PARAM_INVALIDO (2) - db==NULL ou cpf<=0
 */
int enterBuscar(AppDados *db, long long int cpf);

#endif /* BUSCAR_H */