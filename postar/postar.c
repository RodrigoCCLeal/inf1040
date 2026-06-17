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

/* =================================================================
 * ESTADO INTERNO PROTEGIDO (Ocultamento de Informação)
 * ================================================================= */
static long long int cpfLogadoSessao = 0; /* 0 indica que não há usuário logado */

/* =================================================================
 * IMPLEMENTAÇÃO DAS FUNÇÕES DE INTERFACE ENTRE MÓDULOS
 * ================================================================= */

/*
 * Objetivo: registrar o CPF do usuario autenticado como sessao ativa no modulo Postar.
 * Descricao:
 *     O sistema deve armazenar o CPF do usuario apos autenticacao bem-sucedida,
 *     atualizando a variavel estatica privada cpfLogadoSessao. CPF invalido
 *     (cpf <= 0) e ignorado silenciosamente.
 * Acoplamento:
 *     Parametros:
 *       • long long int cpf - CPF do usuario autenticado (deve ser > 0).
 *     Retornos:
 *       • (nenhum)
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • cpf > 0, proveniente de autenticacao bem-sucedida em enterPerfil().
 * Assertivas de Saida:
 *   • cpfLogadoSessao esta definido com o CPF fornecido.
 *   • verificaLogin() retornara LOGIN_OK para este CPF apos a chamada.
 */
void definirSessaoAtual(long long int cpf) {
    if (cpf > 0) {
        cpfLogadoSessao = cpf;
    }
}

/*
 * Objetivo: encerrar a sessao ativa do usuario (logout).
 * Descricao:
 *     O sistema deve zerar a variavel estatica privada cpfLogadoSessao,
 *     sinalizando que nenhum usuario esta logado. Chamada pelo menu principal
 *     ao selecionar a opcao de sair da conta.
 * Acoplamento:
 *     Parametros:
 *       • (nenhum)
 *     Retornos:
 *       • (nenhum)
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • (nenhuma - pode ser chamada mesmo sem sessao ativa)
 * Assertivas de Saida:
 *   • cpfLogadoSessao == 0, indicando ausencia de sessao ativa.
 *   • verificaLogin() retornara LOGIN_OUTRO_CPF para qualquer CPF apos a chamada.
 */
void encerrarSessaoAtual(void) {
    cpfLogadoSessao = 0;
}

/* =================================================================
 * FUNÇÕES DE ACESSO PÚBLICAS
 * ================================================================= */

/*
 * Objetivo: verificar se o CPF fornecido corresponde ao usuario da sessao ativa.
 * Descricao:
 *     O sistema deve comparar o CPF recebido com cpfLogadoSessao. Retorna
 *     LOGIN_OK apenas se houver sessao ativa e o CPF coincidir exatamente.
 *     Usada pelos modulos Buscar, Feed e Avaliacao para controle de acesso.
 * Acoplamento:
 *     Parametros:
 *       • long long int cpf - CPF a verificar (deve ser > 0).
 *     Retornos:
 *       • LOGIN_OK (0)        - CPF confere com a sessao ativa.
 *       • LOGIN_OUTRO_CPF (1) - sem sessao ativa ou CPF diferente.
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • cpf > 0.
 * Assertivas de Saida:
 *   • LOGIN_OK retornado somente se cpfLogadoSessao != 0 e cpfLogadoSessao == cpf.
 *   • Nenhum estado interno e alterado por esta funcao.
 */
int verificaLogin(long long int cpf) {

    /* Parâmetros inválidos → usuário não está logado */
    if (cpf <= 0)
        return LOGIN_OUTRO_CPF;

    /* Compara CPF com a sessão ativa mantida localmente */
    if (cpfLogadoSessao != 0 && cpfLogadoSessao == cpf)
        return LOGIN_OK;

    return LOGIN_OUTRO_CPF;
}

/*
 * Objetivo: validar o acesso do usuario logado a tela de postagem de avaliacoes.
 * Descricao:
 *     O sistema deve verificar se o CPF fornecido e valido e esta autenticado
 *     na sessao ativa via verificaLogin(). Nao acessa arquivos nem altera
 *     qualquer estado interno.
 * Acoplamento:
 *     Parametros:
 *       • long long int cpf - CPF do usuario solicitante (deve ser > 0).
 *     Retornos:
 *       • POSTAR_OK (0)             - login valido, acesso ao fluxo de postagem liberado.
 *       • POSTAR_CPF_INVALIDO (1)   - usuario nao esta logado na sessao ativa.
 *       • POSTAR_PARAM_INVALIDO (2) - cpf <= 0.
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • cpf > 0.
 *       • Uma sessao pode ou nao estar ativa no modulo.
 * Assertivas de Saida:
 *   • POSTAR_OK indica que o fluxo de postagem pode prosseguir.
 *   • Nenhum estado interno e alterado por esta funcao.
 */
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