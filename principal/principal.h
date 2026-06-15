/*
 * principal.h
 * Modulo Principal - Foodies
 *
 * Responsavel por:
 *   1. Alocar o banco em memoria (AppDados) no heap
 *   2. Chamar as funcoes de carga de cada modulo no inicio
 *   3. Chamar as funcoes de salvamento de cada modulo no encerramento
 *   4. Carregar e salvar o JSON de avaliacoes (avaliacoes.json),
 *      pois e o modulo que coordena toda a persistencia
 *
 * Fluxo obrigatorio ao iniciar o programa:
 *   AppDados *db = iniciarApp();   <- aloca + carrega todos os JSONs
 *
 * Fluxo obrigatorio ao encerrar o programa:
 *   encerrarApp(db);               <- salva todos os JSONs + libera memoria
 *
 * JSONs gerenciados:
 *   perfis.json     - lido/salvo pelo modulo Perfil
 *   avaliacoes.json - lido/salvo pelo modulo Principal (este modulo)
 */

#ifndef PRINCIPAL_H
#define PRINCIPAL_H

#include "../dados/dados.h"

/* --- Nome do arquivo JSON de avaliacoes --- */
#define AVALIACOES_JSON "avaliacoes.json"

/*
 * iniciarApp
 *   Aloca AppDados no heap, zera tudo e carrega os JSONs:
 *     - perfis.json      via carregarPerfis()
 *     - avaliacoes.json  via carregarAvaliacoes() (funcao interna)
 *
 *   Retorno:
 *     AppDados*  - ponteiro para o banco alocado e populado
 *     NULL       - falha de alocacao de memoria
 */
AppDados *iniciarApp(void);

/*
 * encerrarApp
 *   Salva os JSONs e libera a memoria:
 *     - perfis.json      via salvarPerfis()
 *     - avaliacoes.json  via salvarAvaliacoes() (funcao interna)
 *   Depois chama free(db).
 *
 *   Nao faz nada se db == NULL.
 */
void encerrarApp(AppDados *db);

/*
 * carregarAvaliacoes
 *   Le avaliacoes.json e popula db->avaliacoes[].
 *   Chamada apenas por iniciarApp.
 *
 *   Retorno:
 *     0  - carregado com sucesso (ou arquivo inexistente)
 *    -1  - erro de leitura
 */
int carregarAvaliacoes(AppDados *db);

/*
 * salvarAvaliacoes
 *   Serializa db->avaliacoes[] para avaliacoes.json.
 *   Chamada apenas por encerrarApp.
 *
 *   Retorno:
 *     0  - salvo com sucesso
 *    -1  - erro de escrita
 */
int salvarAvaliacoes(AppDados *db);

#endif /* PRINCIPAL_H */