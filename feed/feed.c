/*
 * feed.c
 * Módulo Feed – Foodies
 *
 * Implementação das funções de acesso declaradas em feed.h.
 *
 * REGRA FUNDAMENTAL: ZERO I/O de arquivo neste módulo.
 * Todas as operações leem de db->pratos[] e db->restaurantes[] (memória).
 * O JSON é de responsabilidade exclusiva da main.
 *
 * Aleatoriedade: usa Fisher-Yates shuffle sobre array de índices elegíveis,
 * garantindo seleção sem repetição e distribuição uniforme.
 *
 * Dependência externa: verificaLogin() do módulo Postar (PDF §3.4.8).
 */

#include "feed.h"
#include "../postar/postar.h"   /* verificaLogin */

#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <time.h>

/* ═══════════════════════════════════════════════════════════════
 * FUNÇÕES AUXILIARES PRIVADAS (static – invisíveis externamente)
 * ═══════════════════════════════════════════════════════════════ */

/*
 * sementeInicializada
 *   Flag estático para garantir que srand() seja chamado apenas uma vez
 *   durante toda a execução do programa.
 */
static int sementeInicializada = 0;

/*
 * inicializarSemente
 *   Inicializa o gerador de números pseudoaleatórios com o tempo atual.
 *   Chamada uma única vez antes de qualquer operação de shuffle.
 */
static void inicializarSemente(void) {
    if (!sementeInicializada) {
        srand((unsigned int)time(NULL));
        sementeInicializada = 1;
    }
}

/*
 * shuffleIndices
 *   Aplica o algoritmo Fisher-Yates (Knuth shuffle) sobre o array
 *   `indices[]` de tamanho `n`, embaralhando-o in-place.
 *
 *   Garante que cada permutação seja igualmente provável, resultando
 *   em seleção aleatória uniforme sem repetição.
 *
 *   Utilizada tanto por getFeedPratos quanto por getFeedRest.
 */
static void shuffleIndices(int *indices, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        /* Troca indices[i] com indices[j] */
        int tmp    = indices[i];
        indices[i] = indices[j];
        indices[j] = tmp;
    }
}

/*
 * pratoJaAvaliado
 *   Verifica se o usuário identificado por `cpf` já possui uma avaliação
 *   para o prato de índice `idPrato` no banco.
 *   Retorna 1 se já avaliou, 0 caso contrário.
 *
 *   Utilizada por getFeedPratos para excluir pratos já avaliados do feed
 *   (PDF §2.7: "Pratos já avaliados pelo usuário não devem aparecer no Feed").
 */
static int pratoJaAvaliado(const AppDados *db, long long int cpf, int idPrato) {
    for (int i = 0; i < db->nAvaliacoes; i++) {
        if (db->avaliacoes[i].cpf == cpf &&
            db->avaliacoes[i].idPrato == idPrato) {
            return 1;
        }
    }
    return 0;
}

/* ═══════════════════════════════════════════════════════════════
 * FUNÇÕES DE ACESSO PÚBLICAS (PDF §3.4.4)
 * ═══════════════════════════════════════════════════════════════ */

/*
 * enterFeed
 *   Valida o login via verificaLogin e libera o fluxo do feed.
 *   Ver contrato completo em feed.h.
 */
int enterFeed(AppDados *db, long long int cpf) {

    /* Parâmetros inválidos */
    if (db == NULL || cpf <= 0)
        return FEED_PARAM_INVALIDO;

    /* Delega verificação de sessão ao módulo Postar */
    if (verificaLogin(db, cpf) != LOGIN_OK)
        return FEED_CPF_INVALIDO;

    /*
     * Em produção, aqui a main navegaria para a tela de feed.
     * Este módulo é lógica pura: sinaliza sucesso para o Principal.
     */
    return FEED_OK;
}

/*
 * getFeedPratos
 *   Seleciona aleatoriamente até 20 pratos não avaliados pelo usuário.
 *   Ver contrato completo em feed.h.
 */
int getFeedPratos(AppDados *db, long long int cpf,
                  Prato *resultado, int maxResultados) {

    /* Validação de parâmetros */
    if (db == NULL || resultado == NULL || cpf <= 0)
        return FEED_ERRO_PARAM;

    inicializarSemente();

    /*
     * Passo 1: montar array de índices dos pratos ELEGÍVEIS,
     * ou seja, que ainda não foram avaliados pelo usuário (PDF §2.7).
     */
    int elegíveis[MAX_PRATOS];
    int nElegiveis = 0;

    for (int i = 0; i < db->nPratos; i++) {
        if (!pratoJaAvaliado(db, cpf, db->pratos[i].idPrato)) {
            elegíveis[nElegiveis++] = i;
        }
    }

    /*
     * Passo 2: banco insuficiente?
     * PDF §5.6 getFeedPratos: "nulo - Falha, menos de 20 Pratos"
     */
    if (nElegiveis < FEED_QTD_PRATOS)
        return FEED_INSUFICIENTE;

    /*
     * Passo 3: embaralhar os índices elegíveis com Fisher-Yates
     * e copiar os primeiros FEED_QTD_PRATOS para o resultado.
     */
    shuffleIndices(elegíveis, nElegiveis);

    int copiar = FEED_QTD_PRATOS;
    if (copiar > maxResultados) copiar = maxResultados;

    for (int i = 0; i < copiar; i++) {
        resultado[i] = db->pratos[elegíveis[i]];
    }

    return copiar;
}

/*
 * getFeedRest
 *   Seleciona aleatoriamente 6 restaurantes do banco.
 *   Ver contrato completo em feed.h.
 */
int getFeedRest(AppDados *db, Restaurante *resultado, int maxResultados) {

    /* Validação de parâmetros */
    if (db == NULL || resultado == NULL)
        return FEED_ERRO_PARAM;

    inicializarSemente();

    /*
     * PDF §5.5 getFeedRest: "nulo - Falha, menos de 6 restaurantes"
     */
    if (db->nRestaurantes < FEED_QTD_RESTAURANTES)
        return FEED_INSUFICIENTE;

    /*
     * Montar array de todos os índices de restaurantes,
     * embaralhar com Fisher-Yates e copiar os primeiros 6.
     */
    int indices[MAX_RESTAURANTES];
    for (int i = 0; i < db->nRestaurantes; i++) indices[i] = i;

    shuffleIndices(indices, db->nRestaurantes);

    int copiar = FEED_QTD_RESTAURANTES;
    if (copiar > maxResultados) copiar = maxResultados;

    for (int i = 0; i < copiar; i++) {
        resultado[i] = db->restaurantes[indices[i]];
    }

    return copiar;
}
