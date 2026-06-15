/*
 * avaliacao.c
 * Módulo de Avaliação – Foodies
 *
 * Implementação das funções de acesso declaradas em avaliacao.h.
 *
 * REGRA FUNDAMENTAL: ZERO I/O de arquivo neste módulo.
 * Todas as leituras e escritas são sobre db->avaliacoes[] (memória).
 * O JSON é de responsabilidade exclusiva da main.
 */

#include "avaliacao.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

/* ═══════════════════════════════════════════════════════════════
 * FUNÇÕES AUXILIARES PRIVADAS (static – invisíveis externamente)
 * ═══════════════════════════════════════════════════════════════ */

/*
 * notaEhValida
 *   Retorna 1 se `nota` é um número inteiro no intervalo [0, 5].
 *   Retorna 0 caso contrário.
 *   Utilizada por postAval e editAval para validação da nota.
 */
static int notaEhValida(float nota) {
    if (nota < 0.0f || nota > 5.0f) return 0;
    /* A nota deve ser inteira: diferença para o inteiro mais próximo < 0.001 */
    if (fabsf(nota - roundf(nota)) > 0.001f) return 0;
    return 1;
}

/*
 * buscarIndicePrato
 *   Percorre db->pratos[] à procura de idPrato.
 *   Retorna o índice do prato encontrado, ou -1 se não existir.
 *   Utilizada por postAval para validar que o prato existe no banco.
 */
static int buscarIndicePrato(const AppDados *db, int idPrato) {
    for (int i = 0; i < db->nPratos; i++) {
        if (db->pratos[i].idPrato == idPrato)
            return i;
    }
    return -1;
}

/*
 * buscarIndiceAval
 *   Percorre db->avaliacoes[] à procura de idAval.
 *   Retorna o índice da avaliação encontrada, ou -1 se não existir.
 *   Utilizada por editAval para localizar a avaliação a modificar.
 */
static int buscarIndiceAval(const AppDados *db, int idAval) {
    for (int i = 0; i < db->nAvaliacoes; i++) {
        if (db->avaliacoes[i].idAval == idAval)
            return i;
    }
    return -1;
}

/* ═══════════════════════════════════════════════════════════════
 * FUNÇÕES DE ACESSO PÚBLICAS (PDF §3.4.5)
 * ═══════════════════════════════════════════════════════════════ */

/*
 * postAval
 *   Insere uma nova avaliação diretamente em db->avaliacoes[].
 *   O ID é gerado pelo campo db->proximoIdAval (auto-increment).
 *   Ver contrato completo em avaliacao.h.
 */
int postAval(AppDados *db, long long int cpf, int idPrato,
             const char *txt, float nota) {

    /* --- Validação de parâmetros obrigatórios --- */
    if (db == NULL || cpf <= 0 || idPrato <= 0)
        return AVAL_PARAM_INVALIDO;

    /* --- Validação da nota: inteiro em [0, 5] --- */
    if (!notaEhValida(nota))
        return AVAL_ERRO_NOTA;

    /* --- O prato deve existir no banco em memória --- */
    if (buscarIndicePrato(db, idPrato) < 0)
        return AVAL_ERRO_ID_PRATO;

    /* --- Verificação de capacidade do banco --- */
    if (db->nAvaliacoes >= MAX_AVALIACOES)
        return AVAL_BANCO_CHEIO;

    /* --- Preencher novo registro na tabela em memória --- */
    Avaliacao *nova = &db->avaliacoes[db->nAvaliacoes];
    nova->idAval    = db->proximoIdAval++;
    nova->nota      = nota;
    nova->cpf       = cpf;
    nova->idPrato   = idPrato;

    int semComentario = (txt == NULL || txt[0] == '\0');
    if (semComentario) {
        nova->comentario[0] = '\0';
    } else {
        strncpy(nova->comentario, txt, TAM_COMENTARIO - 1);
        nova->comentario[TAM_COMENTARIO - 1] = '\0';
    }

    db->nAvaliacoes++;

    /* Nenhum arquivo é aberto ou gravado aqui */
    return semComentario ? AVAL_OK_SEM_TXT : AVAL_OK;
}

/*
 * verAval
 *   Copia para resultado[] todas as avaliações vinculadas a idPrato.
 *   Operação de somente leitura – não altera db.
 *   Ver contrato completo em avaliacao.h.
 */
int verAval(AppDados *db, int idPrato,
            Avaliacao *resultado, int maxResultados) {

    /* --- Validação de parâmetros --- */
    if (db == NULL || idPrato <= 0 || resultado == NULL || maxResultados <= 0)
        return AVAL_PARAM_INVALIDO;

    int encontradas = 0;
    for (int i = 0; i < db->nAvaliacoes && encontradas < maxResultados; i++) {
        if (db->avaliacoes[i].idPrato != idPrato) continue;

        /* Cópia direta da struct para o array do chamador */
        resultado[encontradas] = db->avaliacoes[i];
        encontradas++;
    }

    /*
     * Retorna 0 se o prato existe mas não tem avaliações (lista vazia).
     * Retorna > 0 com as avaliações encontradas (lista_aval).
     * Não diferenciamos "prato inexistente" aqui pois verAval é chamada
     * após confirmação de existência pelo módulo Pratos.
     */
    return encontradas;
}

/*
 * editAval
 *   Edita nota e/ou comentário de uma avaliação existente em memória.
 *   Somente o autor (verificado via cpf) pode editar (§2.6).
 *   Ver contrato completo em avaliacao.h.
 */
int editAval(AppDados *db, int idAval, long long int cpf,
             const char *novaTxt, float novaNota) {

    /* --- Validação de parâmetros --- */
    if (db == NULL || idAval <= 0 || cpf <= 0)
        return AVAL_PARAM_INVALIDO;

    /* --- Localizar a avaliação pelo ID --- */
    int idx = buscarIndiceAval(db, idAval);
    if (idx < 0)
        return AVAL_ERRO_EDICAO;

    /* --- Verificar posse: apenas o autor pode editar --- */
    if (db->avaliacoes[idx].cpf != cpf)
        return AVAL_ERRO_PERMISSAO;

    /* --- Validar nova nota, se fornecida (novaNota >= 0 indica mudança) --- */
    if (novaNota >= 0.0f) {
        if (!notaEhValida(novaNota))
            return AVAL_ERRO_NOTA;
        db->avaliacoes[idx].nota = novaNota;
    }

    /* --- Atualizar comentário, se fornecido (NULL = manter anterior) --- */
    if (novaTxt != NULL) {
        strncpy(db->avaliacoes[idx].comentario, novaTxt, TAM_COMENTARIO - 1);
        db->avaliacoes[idx].comentario[TAM_COMENTARIO - 1] = '\0';
    }

    /* Modificação aplicada em memória; main salva no encerramento */
    return AVAL_OK;
}
