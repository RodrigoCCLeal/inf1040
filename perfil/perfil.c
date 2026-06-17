/*
 * perfil.c
 * Modulo Perfil - Foodies
 *
 * Implementacao das funcoes declaradas em perfil.h.
 *
 * Este modulo agora centraliza e protege a memoria global de usuarios
 * usando variaveis de escopo de arquivo (static).
 */

#include "perfil.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* =================================================================
 * ARMAZENAMENTO INTERNO PROTEGIDO (Ocultamento de Informacao)
 * ================================================================= */
static Usuario usuarios[MAX_USUARIOS];
static int nUsuarios = 0;
static long long int cpfLogado = 0; /* Controla a sessao ativa do app */

/* =================================================================
 * FUNCOES AUXILIARES PRIVADAS (static - invisiveis externamente)
 * ================================================================= */

static int contarDigitos(long long int n) {
    if (n <= 0) return 0;
    int count = 0;
    while (n > 0) { n /= 10; count++; }
    return count;
}

/* Alterado para varrer diretamente o vetor estatico local 'usuarios' */
static int buscarIndiceUsuario(long long int cpf) {
    for (int i = 0; i < nUsuarios; i++) {
        if (usuarios[i].cpf == cpf)
            return i;
    }
    return -1;
}

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
 * IMPLEMENTACAO DAS FUNCOES DE INTERFACE DE CONTROLE DE SESSAO
 * ================================================================= */

long long int obterCpfLogado(void) {
    return cpfLogado;
}

void definirCpfLogado(long long int cpf) {
    if (cpf >= 0) {
        cpfLogado = cpf;
    }
}

/* =================================================================
 * FUNCOES DE I/O DO JSON
 * ================================================================= */

int carregarPerfis(void) {
    nUsuarios = 0;

    FILE *fp = fopen(PERFIL_JSON, "r");
    if (fp == NULL) {
        /* Arquivo ainda nao existe - primeira execucao, sem erro */
        return 0;
    }

    char linha[300];
    while (fgets(linha, sizeof(linha), fp) && nUsuarios < MAX_USUARIOS) {

        Usuario *u = &usuarios[nUsuarios];
        char nomeBuf [TAM_NOME]  = "";
        char senhaBuf[TAM_SENHA] = "";

        int lidos = sscanf(linha,
            " {\"cpf\":%lld,\"nome\":\"%100[^\"]\",\"senha\":\"%50[^\"]\"}",
            &u->cpf, nomeBuf, senhaBuf);

        if (lidos < 3) continue;

        strncpy(u->nome,  nomeBuf,  TAM_NOME  - 1);
        strncpy(u->senha, senhaBuf, TAM_SENHA - 1);
        u->nome [TAM_NOME  - 1] = '\0';
        u->senha[TAM_SENHA - 1] = '\0';

        nUsuarios++;
    }

    fclose(fp);
    return 0;
}

int salvarPerfis(void) {
    FILE *fp = fopen(PERFIL_JSON, "w");
    if (fp == NULL) return -1;

    char nomeEsc [TAM_NOME  * 2];
    char senhaEsc[TAM_SENHA * 2];

    for (int i = 0; i < nUsuarios; i++) {
        escaparString(nomeEsc,  usuarios[i].nome,  sizeof(nomeEsc));
        escaparString(senhaEsc, usuarios[i].senha, sizeof(senhaEsc));

        fprintf(fp,
            "{\"cpf\":%lld,\"nome\":\"%s\",\"senha\":\"%s\"}\n",
            usuarios[i].cpf,
            nomeEsc,
            senhaEsc);
    }

    fclose(fp);
    return 0;
}

/* =================================================================
 * FUNCOES DE ACESSO PUBLICAS
 * ================================================================= */

int enterPerfil(long long int cpf, const char *senha) {
    /* Validacao de parametros */
    if (cpf <= 0 || senha == NULL)
        return PERFIL_PARAM_INVALIDO;

    /* Buscar usuario pelo CPF na base interna */
    int idx = buscarIndiceUsuario(cpf);
    if (idx < 0)
        return PERFIL_DADOS_INVALIDOS;

    /* Verificar senha */
    if (strncmp(usuarios[idx].senha, senha, TAM_SENHA) != 0)
        return PERFIL_DADOS_INVALIDOS;

    /* Autenticado: salvar o estado na variavel estatica privada */
    cpfLogado = cpf;

    return PERFIL_OK;
}

int criarPerfil(long long int cpf, const char *nome, const char *senha) {
    /* Validacao de parametros */
    if (cpf <= 0 || nome == NULL || senha == NULL)
        return PERFIL_PARAM_INVALIDO;

    if (contarDigitos(cpf) != 11)
        return PERFIL_VALORES_INV;

    if (nome[0] == '\0' || senha[0] == '\0')
        return PERFIL_VALORES_INV;

    if (buscarIndiceUsuario(cpf) >= 0)
        return PERFIL_JA_EXISTE;

    if (nUsuarios >= MAX_USUARIOS)
        return PERFIL_PARAM_INVALIDO;

    /* Inserir novo usuario no banco interno estatico */
    Usuario *novo = &usuarios[nUsuarios];
    novo->cpf = cpf;
    strncpy(novo->nome,  nome,  TAM_NOME  - 1);
    strncpy(novo->senha, senha, TAM_SENHA - 1);
    novo->nome [TAM_NOME  - 1] = '\0';
    novo->senha[TAM_SENHA - 1] = '\0';

    nUsuarios++;

    return PERFIL_CRIADO;
}

Usuario *getUsuario(long long int cpf) {
    if (cpf <= 0)
        return NULL;

    int idx = buscarIndiceUsuario(cpf);
    if (idx < 0)
        return NULL;

    return &usuarios[idx];
}

/* ---------------------------------------------------------------
 * obterNomeUsuario
 * Busca o nome de um usuario cadastrado no sistema a partir do CPF.
 * Caso o CPF nao seja encontrado, atribui um nome padrao ("Usuario").
 * --------------------------------------------------------------- */
void obterNomeUsuario(long long int cpf, char *nomeDestino) {
    /* Assertiva de entrada */
    assert(cpf > 0);
    assert(nomeDestino != NULL);

    // Busca pelo usuario com o cpf da entrada
    Usuario *usuarioEncontrado;
    usuarioEncontrado = getUsuario(cpf);
    if (usuarioEncontrado != NULL){
        strcpy(nomeDestino, usuarioEncontrado->nome);
        return;
    }
    // Se não achar, devolva uma string padrão vazia ou "Usuario".
    strcpy(nomeDestino, "Usuario"); 
}