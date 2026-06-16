/*
 * postar.c
 * Módulo Postar – Foodies
 *
 * Implementação do controle de acesso à postagem e gerenciamento de sessão.
 *
 * Este módulo protege o estado da sessão ativa usando uma variável 
 * de escopo de arquivo (static), eliminando a necessidade de uma struct global.
 */

#include "postar.h"
#include <stddef.h>

/* =================================================================
 * ESTADO INTERNO PROTEGIDO (Ocultamento de Informação)
 * ================================================================= */
static long long int cpfLogadoSessao = 0; /* 0 indica que não há usuário logado */

/* =================================================================
 * IMPLEMENTAÇÃO DAS FUNÇÕES DE INTERFACE ENTRE MÓDULOS
 * ================================================================= */

void definirSessaoAtual(long long int cpf) {
    if (cpf > 0) {
        cpfLogadoSessao = cpf;
    }
}

void encerrarSessaoAtual(void) {
    cpfLogadoSessao = 0;
}

/* =================================================================
 * FUNÇÕES DE ACESSO PÚBLICAS
 * ================================================================= */

int verificaLogin(long long int cpf) {

    /* Parâmetros inválidos → usuário não está logado */
    if (cpf <= 0)
        return LOGIN_OUTRO_CPF;

    /* Compara CPF com a sessão ativa mantida localmente */
    if (cpfLogadoSessao != 0 && cpfLogadoSessao == cpf)
        return LOGIN_OK;

    return LOGIN_OUTRO_CPF;
}

int enterPostar(long long int cpf) {

    /* Parâmetros inválidos */
    if (cpf <= 0)
        return POSTAR_PARAM_INVALIDO;

    /* Verificar autenticação através da API local */
    if (verificaLogin(cpf) != LOGIN_OK)
        return POSTAR_CPF_INVALIDO;

    /* Lógica pura: sinaliza sucesso para a camada de interface gráfica */
    return POSTAR_OK;
}