/*
 * buscar.h
 * Modulo Buscar - Foodies
 *
 * Responsavel por: validar o acesso do usuario logado a tela de busca.
 * As buscas em si sao delegadas aos modulos Pratos e Restaurante,
 * que sao os servidores responsaveis por esses dados.
 *
 * Relacoes cliente-servidor:
 * BUSCAR (cliente) -> PRATOS       (servidor): getListaPratos
 * BUSCAR (cliente) -> RESTAURANTES (servidor): getListaRest
 * BUSCAR (cliente) -> POSTAR/PERFIL (servidor): verificaLogin
 *
 * REGRA DE I/O: este modulo nao abre nem fecha arquivos.
 */

#ifndef BUSCAR_H
#define BUSCAR_H

/* --- Codigos de retorno - enterBuscar --- */
#define BUSCAR_OK             0   /* acesso liberado                    */
#define BUSCAR_CPF_INVALIDO   1   /* usuario nao esta logado            */
#define BUSCAR_PARAM_INVALIDO 2   /* parametro invalido (cpf<=0)        */

/*
 * enterBuscar
 * Verifica se o usuario esta logado (via verificaLogin do modulo responsável)
 * e libera o fluxo de busca.
 *
 * Parametros:
 * cpf - CPF do usuario solicitante (long long int, deve ser > 0)
 *
 * Retorno:
 * BUSCAR_OK             (0) - login valido, acesso liberado
 * BUSCAR_CPF_INVALIDO   (1) - CPF nao esta logado na sessao
 * BUSCAR_PARAM_INVALIDO (2) - cpf<=0
 */
int enterBuscar(long long int cpf);

#endif /* BUSCAR_H */