/*
 * principal.c
 * Modulo Principal - Foodies
 *
 * Implementacao das funcoes de orquestracao declaradas em principal.h.
 *
 * Este modulo nao abre mais arquivos diretamente. Ele delega a carga e a
 * gravacao para as APIs publicas especificas de cada modulo especialista.
 */

#include "principal.h"
#include "../perfil/perfil.h"
#include "../restaurante/restaurante.h"
#include "../pratos/pratos.h"
#include "../avaliacao/avaliacao.h"

#include <stdlib.h>

/* =================================================================
 * FUNCOES PUBLICAS DE CICLO DE VIDA DO APP
 * ================================================================= */

/*
 * Objetivo: inicializar o sistema Foodies e carregar os dados de todos os modulos.
 * Descricao:
 *     O sistema deve coordenar a inicializacao de cada modulo especialista
 *     (Perfil, Restaurante, Pratos e Avaliacao), invocando suas funcoes de
 *     carga a partir dos respectivos arquivos JSON.
 * Acoplamento:
 *     Parametros:
 *       • (nenhum)
 *     Retornos:
 *       • int - 0 em caso de sucesso.
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • Os modulos Perfil, Restaurante, Pratos e Avaliacao estao acessiveis.
 *       • Os arquivos JSON existem ou a ausencia deles e tratada por cada modulo.
 * Assertivas de Saida:
 *   • Todas as estruturas internas dos modulos estao populadas.
 *   • O sistema esta pronto para operar.
 */
int iniciarApp(void) {
    
    /* * Cada modulo agora gerencia seu proprio carregamento de JSON 
     * para suas respectivas colecoes estaticas ocultas em memoria.
     */
    carregarPerfis();
    carregarRestaurantes();
    carregarPratos();
    carregarAvaliacoes(); /* Delegado para o modulo especialista de Avaliacao */

    return 0;
}

/*
 * Objetivo: encerrar o sistema Foodies salvando os dados mutaveis em disco.
 * Descricao:
 *     O sistema deve garantir a persistencia dos dados que sofreram alteracoes
 *     durante a execucao, delegando o salvamento para os modulos Perfil e
 *     Avaliacao. Modulos com dados fixos (Pratos, Restaurante) nao sao
 *     chamados pois seus JSONs nao sao regravados.
 * Acoplamento:
 *     Parametros:
 *       • (nenhum)
 *     Retornos:
 *       • (nenhum)
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • iniciarApp() foi chamada anteriormente.
 *       • Os modulos Perfil e Avaliacao possuem dados validos em memoria.
 * Assertivas de Saida:
 *   • Os arquivos perfis.json e avaliacoes.json estao atualizados com os
 *     dados presentes em memoria no momento da chamada.
 */
void encerrarApp(void) {
    
    /*
     * Apenas os modulos que sofrem alteracoes em tempo de execucao 
     * (dados mutaveis) precisam realizar o procedimento de salvamento.
     */
    salvarPerfis();
    salvarAvaliacoes(); /* Delegado para o modulo especialista de Avaliacao */
}