/*
 * perfil.c
 * Modulo Perfil - Foodies
 *
 * Implementacao das funcoes declaradas em perfil.h.
 *
 * Este modulo e o UNICO responsavel por ler e escrever perfis.json.
 * O JSON e aberto apenas em carregarPerfis() (inicio do programa,
 * chamada pelo Principal) e escrito apenas em salvarPerfis()
 * (encerramento do programa, chamada pelo Principal).
 * Durante a execucao, todas as operacoes sao feitas em memoria.
 *
 * Formato do JSON (uma linha por registro):
 *   {"cpf":12345678901,"nome":"Joao","senha":"abc123"}
 */

#include "perfil.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* =================================================================
 * FUNCOES AUXILIARES PRIVADAS (static - invisiveis externamente)
 * ================================================================= */

/*
 * contarDigitos
 *   Conta quantos digitos decimais tem um numero long long int positivo.
 *   Utilizada por criarPerfil para validar que o CPF tem 11 digitos.
 */
static int contarDigitos(long long int n) {
    if (n <= 0) return 0;
    int count = 0;
    while (n > 0) { n /= 10; count++; }
    return count;
}

/*
 * buscarIndiceUsuario
 *   Percorre db->usuarios[] procurando pelo CPF.
 *   Retorna o indice encontrado, ou -1 se nao existir.
 *   Utilizada por enterPerfil, criarPerfil e getUsuario.
 */
static int buscarIndiceUsuario(const AppDados *db, long long int cpf) {
    for (int i = 0; i < db->nUsuarios; i++) {
        if (db->usuarios[i].cpf == cpf)
            return i;
    }
    return -1;
}

/*
 * escaparString
 *   Copia src para dst escapando aspas duplas (\" em JSON).
 *   Garante que o JSON gerado seja valido mesmo com aspas no nome/senha.
 *   Utilizada por salvarPerfis.
 */
static void escaparString(char *dst, const char *src, int maxDst) {
    int j = 0;
    for (int i = 0; src[i] != '\0' && j < maxDst - 2; i++) {
        if (src[i] == '"' || src[i] == '\\') {
            dst[j++] = '\\';
        }
        dst[j++] = src[i];
    }
    dst[j] = '\0';
}

/* =================================================================
 * FUNCOES DE I/O DO JSON (chamadas exclusivamente pelo Principal)
 * ================================================================= */

/*
 * carregarPerfis
 *   Le perfis.json linha a linha e popula db->usuarios[].
 *   Chamada uma unica vez pelo Principal no inicio da execucao.
 *   Se o arquivo nao existir, retorna 0 (banco vazio, sem erro).
 *   Ver contrato em perfil.h.
 */
int carregarPerfis(AppDados *db) {
    if (db == NULL) return -1;

    db->nUsuarios = 0;

    FILE *fp = fopen(PERFIL_JSON, "r");
    if (fp == NULL) {
        /* Arquivo ainda nao existe - primeira execucao, sem erro */
        return 0;
    }

    char linha[300];
    while (fgets(linha, sizeof(linha), fp) &&
           db->nUsuarios < MAX_USUARIOS) {

        Usuario *u = &db->usuarios[db->nUsuarios];
        char nomeBuf [TAM_NOME]  = "";
        char senhaBuf[TAM_SENHA] = "";

        /*
         * Parsing do formato:
         *   {"cpf":12345678901,"nome":"Joao","senha":"abc123"}
         */
        int lidos = sscanf(linha,
            " {\"cpf\":%lld,\"nome\":\"%100[^\"]\",\"senha\":\"%50[^\"]\"}",
            &u->cpf, nomeBuf, senhaBuf);

        if (lidos < 3) continue;  /* linha malformada, pula */

        strncpy(u->nome,  nomeBuf,  TAM_NOME  - 1);
        strncpy(u->senha, senhaBuf, TAM_SENHA - 1);
        u->nome [TAM_NOME  - 1] = '\0';
        u->senha[TAM_SENHA - 1] = '\0';

        db->nUsuarios++;
    }

    fclose(fp);
    return 0;
}

/*
 * salvarPerfis
 *   Serializa db->usuarios[] para perfis.json.
 *   Chamada uma unica vez pelo Principal ao encerrar a execucao.
 *   Sobrescreve o arquivo existente com o estado atual da memoria.
 *   Ver contrato em perfil.h.
 */
int salvarPerfis(AppDados *db) {
    if (db == NULL) return -1;

    FILE *fp = fopen(PERFIL_JSON, "w");
    if (fp == NULL) return -1;

    char nomeEsc [TAM_NOME  * 2];
    char senhaEsc[TAM_SENHA * 2];

    for (int i = 0; i < db->nUsuarios; i++) {
        escaparString(nomeEsc,  db->usuarios[i].nome,  sizeof(nomeEsc));
        escaparString(senhaEsc, db->usuarios[i].senha, sizeof(senhaEsc));

        fprintf(fp,
            "{\"cpf\":%lld,\"nome\":\"%s\",\"senha\":\"%s\"}\n",
            db->usuarios[i].cpf,
            nomeEsc,
            senhaEsc);
    }

    fclose(fp);
    return 0;
}

/* =================================================================
 * FUNCOES DE ACESSO PUBLICAS (PDF 3.4.8)
 * ================================================================= */

/*
 * enterPerfil
 *   Autentica o usuario verificando CPF e senha no banco em memoria.
 *   Se autenticado, registra db->cpfLogado = cpf (inicia sessao).
 *   Ver contrato em perfil.h.
 */
int enterPerfil(AppDados *db, long long int cpf, const char *senha) {

    /* Validacao de parametros */
    if (db == NULL || cpf <= 0 || senha == NULL)
        return PERFIL_PARAM_INVALIDO;

    /* Buscar usuario pelo CPF */
    int idx = buscarIndiceUsuario(db, cpf);
    if (idx < 0)
        return PERFIL_DADOS_INVALIDOS;  /* CPF nao encontrado */

    /* Verificar senha */
    if (strncmp(db->usuarios[idx].senha, senha, TAM_SENHA) != 0)
        return PERFIL_DADOS_INVALIDOS;  /* senha errada */

    /* Autenticado: registrar sessao ativa no banco em memoria */
    db->cpfLogado = cpf;

    return PERFIL_OK;
}

/*
 * criarPerfil
 *   Valida os dados, verifica duplicata e insere novo usuario.
 *   Ver contrato em perfil.h.
 */
int criarPerfil(AppDados *db, long long int cpf,
                const char *nome, const char *senha) {

    /* Validacao de parametros obrigatorios */
    if (db == NULL || cpf <= 0 || nome == NULL || senha == NULL)
        return PERFIL_PARAM_INVALIDO;

    /* CPF deve ter exatamente 11 digitos */
    if (contarDigitos(cpf) != 11)
        return PERFIL_VALORES_INV;

    /* Nome e senha nao podem ser vazios */
    if (nome[0] == '\0' || senha[0] == '\0')
        return PERFIL_VALORES_INV;

    /* CPF nao pode ja estar cadastrado */
    if (buscarIndiceUsuario(db, cpf) >= 0)
        return PERFIL_JA_EXISTE;

    /* Verificar capacidade do banco */
    if (db->nUsuarios >= MAX_USUARIOS)
        return PERFIL_PARAM_INVALIDO;

    /* Inserir novo usuario no banco em memoria */
    Usuario *novo = &db->usuarios[db->nUsuarios];
    novo->cpf = cpf;
    strncpy(novo->nome,  nome,  TAM_NOME  - 1);
    strncpy(novo->senha, senha, TAM_SENHA - 1);
    novo->nome [TAM_NOME  - 1] = '\0';
    novo->senha[TAM_SENHA - 1] = '\0';

    db->nUsuarios++;

    return PERFIL_CRIADO;
}

/*
 * getUsuario
 *   Retorna ponteiro direto para o registro do usuario no banco.
 *   Ver contrato em perfil.h.
 */
Usuario *getUsuario(AppDados *db, long long int cpf) {

    if (db == NULL || cpf <= 0)
        return NULL;

    int idx = buscarIndiceUsuario(db, cpf);
    if (idx < 0)
        return NULL;

    return &db->usuarios[idx];
}