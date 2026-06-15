/*
 * avaliacao.h
 * Módulo de Avaliação – Foodies
 *
 * Responsável por: criar, editar e consultar avaliações de pratos.
 *
 * REGRA DE I/O: este módulo NÃO abre nem fecha arquivos JSON.
 * Todas as operações são feitas sobre AppDados* (banco em memória).
 * O JSON é aberto/fechado exclusivamente pela main.
 *
 * Funções de acesso definidas no PDF §3.4.5:
 *   postAval  – cria uma avaliação no banco em memória
 *   verAval   – retorna avaliações de um prato (somente leitura)
 *
 * Função adicional de edição (§2.6 Requisitos Módulo Avaliação):
 *   editAval  – edita nota/comentário de avaliação própria
 */

#ifndef AVALIACAO_H
#define AVALIACAO_H

#include "../dados/dados.h"

/* ─── Códigos de retorno ─────────────────────────────────────── */
#define AVAL_OK               0   /* sucesso (com comentário)            */
#define AVAL_OK_SEM_TXT       1   /* sucesso (sem comentário)            */
#define AVAL_ERRO_ID_PRATO    2   /* idPrato não existe no banco         */
#define AVAL_ERRO_NOTA        4   /* nota fora de [0,5] ou não-inteira   */
#define AVAL_PARAM_INVALIDO  -1   /* ponteiro nulo ou valor ilegal       */
#define AVAL_ERRO_EDICAO     -2   /* avaliação não encontrada            */
#define AVAL_ERRO_PERMISSAO  -3   /* CPF não é o autor da avaliação      */
#define AVAL_BANCO_CHEIO     -4   /* MAX_AVALIACOES atingido             */

/* ─── Funções de acesso (PDF §3.4.5) ────────────────────────── */

/*
 * postAval
 *   Cria uma avaliação e a insere em db->avaliacoes[].
 *   O ID é gerado internamente (db->proximoIdAval).
 *   Nenhum arquivo é aberto ou gravado.
 *
 *   Parâmetros:
 *     db      – banco em memória (não pode ser NULL)
 *     cpf     – CPF do autor (long long int, deve ser > 0)
 *     idPrato – ID do prato avaliado (deve existir em db->pratos[])
 *     txt     – comentário opcional (NULL ou "" para omitir)
 *     nota    – número inteiro de 0 a 5 (passado como float)
 *
 *   Retorno (PDF §5.4):
 *     AVAL_OK            (0)  – postado com comentário
 *     AVAL_OK_SEM_TXT    (1)  – postado sem comentário
 *     AVAL_ERRO_ID_PRATO (2)  – idPrato inexistente
 *     AVAL_ERRO_NOTA     (4)  – nota inválida
 *     AVAL_PARAM_INVALIDO(-1) – db==NULL, cpf<=0 ou idPrato<=0
 *     AVAL_BANCO_CHEIO   (-4) – capacidade esgotada
 */
int postAval(AppDados *db, long long int cpf, int idPrato,
             const char *txt, float nota);

/*
 * verAval
 *   Copia para resultado[] todas as avaliações do prato indicado.
 *   Operação de somente leitura — não modifica db.
 *
 *   Parâmetros:
 *     db            – banco em memória
 *     idPrato       – ID do prato a consultar (deve ser > 0)
 *     resultado     – array de Avaliacao alocado pelo chamador
 *     maxResultados – capacidade do array
 *
 *   Retorno (PDF §5.4):
 *     >= 0               – quantidade de avaliações copiadas
 *                          (0 = prato existe mas sem avaliações)
 *     AVAL_PARAM_INVALIDO(-1) – db==NULL, idPrato<=0 ou resultado==NULL
 */
int verAval(AppDados *db, int idPrato,
            Avaliacao *resultado, int maxResultados);

/*
 * editAval
 *   Edita nota e/ou comentário de uma avaliação existente.
 *   Apenas o autor (CPF) pode editar a própria avaliação (§2.6).
 *   A confirmação do usuário deve ocorrer na camada de UI
 *   antes de chamar esta função.
 *
 *   Parâmetros:
 *     db       – banco em memória
 *     idAval   – ID da avaliação (deve ser > 0)
 *     cpf      – CPF do solicitante
 *     novaTxt  – novo comentário; NULL = mantém o anterior
 *     novaNota – nova nota [0,5]; valor < 0 = mantém a anterior
 *
 *   Retorno:
 *     AVAL_OK             (0)  – edição aplicada em memória
 *     AVAL_ERRO_EDICAO   (-2)  – avaliação não encontrada
 *     AVAL_ERRO_PERMISSAO(-3)  – CPF não é o autor
 *     AVAL_ERRO_NOTA      (4)  – nova nota inválida
 *     AVAL_PARAM_INVALIDO(-1)  – db==NULL, idAval<=0 ou cpf<=0
 */
int editAval(AppDados *db, int idAval, long long int cpf,
             const char *novaTxt, float novaNota);

#endif /* AVALIACAO_H */
