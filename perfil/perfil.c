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

/*
 * Objetivo: retornar o CPF do usuario atualmente autenticado na sessao.
 * Descricao:
 *     O sistema deve expor o valor da variavel estatica privada cpfLogado
 *     sem conceder acesso direto a ela. Retorna 0 se nenhum usuario estiver
 *     logado. Usada pelo main para consultar o estado da sessao.
 * Acoplamento:
 *     Parametros:
 *       • (nenhum)
 *     Retornos:
 *       • long long int - CPF do usuario logado, ou 0 se nao ha sessao ativa.
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • (nenhuma)
 * Assertivas de Saida:
 *   • Retorna valor >= 0.
 *   • O valor 0 indica ausencia de sessao ativa.
 *   • Nenhum estado interno e alterado por esta funcao.
 */
long long int obterCpfLogado(void) {
    return cpfLogado;
}

/*
 * Objetivo: definir ou limpar a sessao do usuario logado no modulo Perfil.
 * Descricao:
 *     O sistema deve atualizar a variavel estatica privada cpfLogado com o
 *     CPF fornecido. Passar 0 limpa a sessao (logout). Valores negativos
 *     sao ignorados silenciosamente.
 * Acoplamento:
 *     Parametros:
 *       • long long int cpf - CPF a definir como sessao ativa (0 para limpar).
 *     Retornos:
 *       • (nenhum)
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • cpf >= 0.
 * Assertivas de Saida:
 *   • cpfLogado esta atualizado com o valor fornecido se cpf >= 0.
 */
void definirCpfLogado(long long int cpf) {
    if (cpf >= 0) {
        cpfLogado = cpf;
    }
}

/* =================================================================
 * FUNCOES DE I/O DO JSON
 * ================================================================= */

/*
 * Objetivo: carregar os dados de usuarios a partir do arquivo JSON.
 * Descricao:
 *     O sistema deve persistir os dados contidos no modulo Perfil.
 *     O sistema deve carregar os dados do modulo Perfil a partir do arquivo
 *     "perfis.json", populando o vetor estatico interno de usuarios linha a
 *     linha. Se o arquivo nao existir (primeira execucao), retorna 0 sem erro.
 * Acoplamento:
 *     Parametros:
 *       • (nenhum)
 *     Retornos:
 *       • int - 0 em caso de sucesso ou arquivo inexistente.
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • A estrutura de dados interna de usuarios esta disponivel.
 *       • O arquivo "perfis.json" existe ou e a primeira execucao do sistema.
 * Assertivas de Saida:
 *   • O vetor usuarios[] esta populado com os registros do JSON e nUsuarios
 *     reflete a quantidade carregada, ou nUsuarios == 0 em caso de falha.
 */
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

/*
 * Objetivo: salvar os dados de usuarios no arquivo JSON.
 * Descricao:
 *     O sistema deve persistir os dados contidos no modulo Perfil.
 *     O sistema deve serializar o vetor estatico interno de usuarios para o
 *     arquivo "perfis.json", gravando um registro por linha em formato JSON.
 *     Chamada pelo Principal ao encerrar a execucao.
 * Acoplamento:
 *     Parametros:
 *       • (nenhum)
 *     Retornos:
 *       • int - 0 em caso de sucesso; -1 se nao for possivel abrir o arquivo.
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • O vetor usuarios[] e nUsuarios estao em estado valido em memoria.
 * Assertivas de Saida:
 *   • O arquivo "perfis.json" contem os dados atualizados de todos os
 *     usuarios em memoria, ou o arquivo permanece inalterado em caso de erro.
 */
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

/*
 * Objetivo: autenticar um usuario no sistema com CPF e senha.
 * Descricao:
 *     O sistema deve buscar o usuario pelo CPF no vetor interno, verificar
 *     a senha e, se valido, registrar a sessao ativa na variavel estatica
 *     privada cpfLogado. Retorna codigo de erro especifico para cada falha.
 * Acoplamento:
 *     Parametros:
 *       • long long int cpf    - CPF do usuario (deve ser > 0).
 *       • const char  *senha   - senha do usuario (nao pode ser NULL).
 *     Retornos:
 *       • PERFIL_OK (0)              - autenticado com sucesso.
 *       • PERFIL_DADOS_INVALIDOS (1) - CPF nao encontrado ou senha incorreta.
 *       • PERFIL_PARAM_INVALIDO (2)  - cpf <= 0 ou senha == NULL.
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • cpf > 0 e senha != NULL.
 *       • carregarPerfis() foi chamada anteriormente.
 * Assertivas de Saida:
 *   • Se PERFIL_OK, cpfLogado esta definido com o CPF autenticado.
 *   • Se falha, cpfLogado permanece inalterado.
 */
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

/*
 * Objetivo: criar um novo perfil de usuario no sistema.
 * Descricao:
 *     O sistema deve validar o CPF (exatamente 11 digitos), nome e senha
 *     nao vazios, verificar duplicidade de CPF e inserir o novo usuario no
 *     vetor estatico interno. Nao salva em disco imediatamente.
 * Acoplamento:
 *     Parametros:
 *       • long long int cpf  - CPF de 11 digitos do novo usuario.
 *       • const char  *nome  - nome do usuario (nao vazio, nao NULL).
 *       • const char  *senha - senha do usuario (nao vazia, nao NULL).
 *     Retornos:
 *       • PERFIL_CRIADO (0)       - perfil criado com sucesso.
 *       • PERFIL_VALORES_INV (1)  - CPF nao tem 11 digitos, nome ou senha vazios.
 *       • PERFIL_PARAM_INVALIDO (2) - cpf <= 0, NULL nos parametros ou banco cheio.
 *       • PERFIL_JA_EXISTE (3)    - CPF ja cadastrado no sistema.
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • cpf > 0, nome != NULL, senha != NULL.
 *       • carregarPerfis() foi chamada anteriormente.
 * Assertivas de Saida:
 *   • Se PERFIL_CRIADO, o novo usuario esta no vetor interno e nUsuarios
 *     foi incrementado.
 *   • Se falha, o vetor interno permanece inalterado.
 */
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

/*
 * Objetivo: obter um ponteiro para o registro do usuario com o CPF informado.
 * Descricao:
 *     O sistema deve percorrer o vetor estatico interno buscando o usuario
 *     pelo CPF e retornar um ponteiro direto para ele. O ponteiro e valido
 *     enquanto o vetor interno nao for realocado.
 * Acoplamento:
 *     Parametros:
 *       • long long int cpf - CPF do usuario a buscar (deve ser > 0).
 *     Retornos:
 *       • Usuario* - ponteiro para o usuario encontrado no vetor interno.
 *       • NULL     - se CPF invalido (cpf <= 0) ou nao encontrado.
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • cpf > 0.
 *       • carregarPerfis() foi chamada anteriormente.
 * Assertivas de Saida:
 *   • Retorna ponteiro valido se o CPF existe, NULL caso contrario.
 *   • Nenhum estado interno e alterado por esta funcao.
 */
Usuario *getUsuario(long long int cpf) {
    if (cpf <= 0)
        return NULL;

    int idx = buscarIndiceUsuario(cpf);
    if (idx < 0)
        return NULL;

    return &usuarios[idx];
}

/*
 * Objetivo: copiar o nome do usuario com o CPF informado para um buffer de destino.
 * Descricao:
 *     O sistema deve buscar o usuario pelo CPF via getUsuario(). Se encontrado,
 *     copia o nome para nomeDestino; caso contrario, copia a string padrao
 *     "Usuario". Garante que nomeDestino sempre contera uma string valida.
 * Acoplamento:
 *     Parametros:
 *       • long long int cpf       - CPF do usuario (deve ser > 0).
 *       • char         *nomeDestino - buffer de destino (nao pode ser NULL,
 *                                    deve ter espaco para TAM_NOME caracteres).
 *     Retornos:
 *       • (nenhum)
 * Condicoes de Acoplamento:
 *     Assertivas de Entrada:
 *       • cpf > 0.
 *       • nomeDestino != NULL e com espaco minimo de TAM_NOME bytes alocado.
 *       • carregarPerfis() foi chamada anteriormente.
 * Assertivas de Saida:
 *   • nomeDestino contem o nome do usuario se encontrado, ou "Usuario"
 *     se o CPF nao existir no sistema.
 */
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