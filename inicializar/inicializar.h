/*
 * principal.h
 * Modulo Principal - Foodies
 *
 * Responsavel por:
 * 1. Chamar as funcoes de carga de cada modulo oculto no inicio.
 * 2. Chamar as funcoes de salvamento de cada modulo oculto no encerramento.
 *
 * Fluxo obrigatorio ao iniciar o programa:
 * iniciarApp();     <- Inicializa o estado de todos os modulos.
 *
 * Fluxo obrigatorio ao encerrar o programa:
 * encerrarApp();    <- Salva as alteracoes pendentes de todos os modulos.
 */

#ifndef PRINCIPAL_H
#define PRINCIPAL_H

/* * Nota: Nao inclui mais "../dados/dados.h" pois a estrutura global unificada 
 * foi desmembrada em favor do encapsulamento puro de cada modulo.
 */

/*
 * iniciarApp
 * Chama a funcao de carga de cada modulo isolado para popular as
 * estruturas internas ocultas (static) a partir dos respectivos JSONs.
 *
 * Retorno:
 * 0  - Sucesso na inicializacao de todos os modulos.
 * -1  - Falha critica ao carregar componentes fundamentais.
 */
int iniciarApp(void);

/*
 * encerrarApp
 * Garante a persistencia chamando as funcoes de salvamento dos modulos
 * que modificam dados durante a execucao (Perfil e Avaliacao).
 */
void encerrarApp(void);

#endif /* PRINCIPAL_H */