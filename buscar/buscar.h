/*
 * buscar.h
 * Módulo Buscar – Foodies
 *
 * Responsável por: permitir que o usuário logado pesquise pratos
 * e restaurantes pelo nome dentro do banco em memória.
 *
 * Relações cliente-servidor (PDF §3.2):
 *   BUSCAR (cliente) → PRATOS      (servidor): getListaPratos
 *   BUSCAR (cliente) → RESTAURANTES(servidor): getListaRest
 *   BUSCAR (cliente) → PERFIL/POSTAR(servidor): verificaLogin
 *
 * REGRA DE I/O: este módulo NÃO abre nem fecha arquivos JSON.
 * Todas as operações são feitas sobre AppDados* (banco em memória).
 * O JSON é aberto/fechado exclusivamente pela main.
 *
 * Funções de acesso definidas no PDF §3.4.3:
 *   enterBuscar    – valida login e libera acesso à tela de busca
 *
 * Funções auxiliares de pesquisa (chamadas internamente por enterBuscar
 * e também disponíveis para outros módulos clientes):
 *   getListaPratos – retorna pratos cujo nome contenha a substring buscada
 *   getListaRest   – retorna restaurantes cujo nome contenha a substring buscada
 */

#ifndef BUSCAR_H
#define BUSCAR_H

#include "../dados/dados.h"

/* ─── Tamanho máximo dos arrays de resultado ─────────────────── */
#define BUSCAR_MAX_RESULTADOS 100

/* ─── Códigos de retorno – enterBuscar ──────────────────────── */
#define BUSCAR_OK             0   /* acesso liberado à tela de busca    */
#define BUSCAR_CPF_INVALIDO   1   /* usuário não está logado            */
#define BUSCAR_PARAM_INVALIDO 2   /* db==NULL ou cpf<=0                 */

/* ─── Códigos de retorno – getListaPratos / getListaRest ─────── */
#define BUSCAR_NOME_INVALIDO -1   /* nome==NULL (parâmetro inválido)    */
/* Retorno >= 0 indica a quantidade de itens copiados para o array   */

/*
 * enterBuscar
 *   Verifica se o usuário está logado (via verificaLogin do módulo Postar)
 *   e libera o fluxo de busca.
 *
 *   Parâmetros:
 *     db  – banco em memória (não pode ser NULL)
 *     cpf – CPF do usuário solicitante (long long int, deve ser > 0)
 *
 *   Retorno (PDF §5.2):
 *     BUSCAR_OK             (0) – login válido, acesso liberado
 *     BUSCAR_CPF_INVALIDO   (1) – CPF não está logado na sessão
 *     BUSCAR_PARAM_INVALIDO (2) – db==NULL ou cpf<=0
 */
int enterBuscar(AppDados *db, long long int cpf);

/*
 * getListaPratos
 *   Pesquisa em db->pratos[] todos os registros cujo nome contenha
 *   `nome_prato` como substring (busca case-insensitive).
 *   Se `nome_prato` for string vazia (""), nada é retornado (PDF §2.4).
 *
 *   Parâmetros:
 *     db           – banco em memória
 *     nome_prato   – substring a buscar (não pode ser NULL)
 *     resultado    – array de Prato alocado pelo chamador
 *     maxResultados– capacidade do array
 *
 *   Retorno (PDF §5.6 getListaPratos):
 *     >= 0               – quantidade de pratos copiados para resultado[]
 *                          (0 = nenhum prato encontrado / nome vazio)
 *     BUSCAR_NOME_INVALIDO(-1) – nome_prato==NULL ou resultado==NULL
 */
int getListaPratos(AppDados *db, const char *nome_prato,
                   Prato *resultado, int maxResultados);

/*
 * getListaRest
 *   Pesquisa em db->restaurantes[] todos os registros cujo nome contenha
 *   `nome_rest` como substring (busca case-insensitive).
 *   Se `nome_rest` for string vazia (""), nada é retornado (PDF §2.4).
 *
 *   Parâmetros:
 *     db           – banco em memória
 *     nome_rest    – substring a buscar (não pode ser NULL)
 *     resultado    – array de Restaurante alocado pelo chamador
 *     maxResultados– capacidade do array
 *
 *   Retorno (PDF §5.5 getListaRest):
 *     >= 0               – quantidade de restaurantes copiados
 *                          (0 = nenhum encontrado / nome vazio)
 *     BUSCAR_NOME_INVALIDO(-1) – nome_rest==NULL ou resultado==NULL
 */
int getListaRest(AppDados *db, const char *nome_rest,
                 Restaurante *resultado, int maxResultados);

#endif /* BUSCAR_H */
