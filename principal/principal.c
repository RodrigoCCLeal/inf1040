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
 * iniciarApp
 * Inicializa e popula o estado interno de todos os modulos do sistema.
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
 * encerrarApp
 * Solicita que cada modulo salve suas alteracoes em disco.
 */
void encerrarApp(void) {
    
    /*
     * Apenas os modulos que sofrem alteracoes em tempo de execucao 
     * (dados mutaveis) precisam realizar o procedimento de salvamento.
     */
    salvarPerfis();
    salvarAvaliacoes(); /* Delegado para o modulo especialista de Avaliacao */
}