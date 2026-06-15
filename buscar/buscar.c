/*
 * buscar.c
 * Módulo Buscar – Foodies
 *
 * Implementação das funções de acesso declaradas em buscar.h.
 *
 * REGRA FUNDAMENTAL: ZERO I/O de arquivo neste módulo.
 * Todas as operações leem de db->pratos[] e db->restaurantes[] (memória).
 * O JSON é de responsabilidade exclusiva da main.
 *
 * Dependência externa: verificaLogin() do módulo Postar (PDF §3.4.8),
 * usada por enterBuscar para validar a sessão ativa.
 */

#include "buscar.h"
#include "../postar/postar.h"   /* verificaLogin */

#include <string.h>
#include <ctype.h>
#include <stddef.h>

/* ═══════════════════════════════════════════════════════════════
 * FUNÇÕES AUXILIARES PRIVADAS (static – invisíveis externamente)
 * ═══════════════════════════════════════════════════════════════ */

/*
 * contemSubstring
 *   Verifica se `haystack` contém `needle` como substring,
 *   de forma case-insensitive.
 *   Retorna 1 se contém, 0 caso contrário.
 *
 *   Utilizada por getListaPratos e getListaRest para a pesquisa parcial
 *   pelo nome, permitindo que o usuário encontre resultados com buscas
 *   incompletas (ex: "fran" encontra "Frango Grelhado").
 */
static int contemSubstring(const char *haystack, const char *needle) {
    if (needle[0] == '\0') return 0;  /* string vazia não casa com nada */

    /* Avança por cada posição de haystack e tenta casar needle */
    for (int i = 0; haystack[i] != '\0'; i++) {
        int j = 0;
        while (needle[j] != '\0' &&
               tolower((unsigned char)haystack[i + j]) ==
               tolower((unsigned char)needle[j])) {
            j++;
        }
        if (needle[j] == '\0') return 1;  /* casamento completo */
    }
    return 0;
}

/* ═══════════════════════════════════════════════════════════════
 * FUNÇÕES DE ACESSO PÚBLICAS (PDF §3.4.3)
 * ═══════════════════════════════════════════════════════════════ */

/*
 * enterBuscar
 *   Valida o login via verificaLogin e libera o fluxo de busca.
 *   Ver contrato completo em buscar.h.
 */
int enterBuscar(AppDados *db, long long int cpf) {

    /* Parâmetros inválidos */
    if (db == NULL || cpf <= 0)
        return BUSCAR_PARAM_INVALIDO;

    /* Delega verificação de sessão ao módulo Postar */
    if (verificaLogin(db, cpf) != LOGIN_OK)
        return BUSCAR_CPF_INVALIDO;

    /*
     * Em produção, aqui a main navegaria para a tela de busca.
     * Este módulo é lógica pura: sinaliza sucesso para o Principal.
     */
    return BUSCAR_OK;
}

/*
 * getListaPratos
 *   Pesquisa pratos cujo nome contenha nome_prato como substring.
 *   Ver contrato completo em buscar.h.
 */
int getListaPratos(AppDados *db, const char *nome_prato,
                   Prato *resultado, int maxResultados) {

    /* Validação de parâmetros */
    if (db == NULL || nome_prato == NULL || resultado == NULL || maxResultados <= 0)
        return BUSCAR_NOME_INVALIDO;

    /* Nome vazio: PDF §2.4 – "Se o usuário não digitar nada, nada deve aparecer" */
    if (nome_prato[0] == '\0')
        return 0;

    int encontrados = 0;
    for (int i = 0; i < db->nPratos && encontrados < maxResultados; i++) {
        if (contemSubstring(db->pratos[i].nome, nome_prato)) {
            resultado[encontrados] = db->pratos[i];
            encontrados++;
        }
    }

    return encontrados;
}

/*
 * getListaRest
 *   Pesquisa restaurantes cujo nome contenha nome_rest como substring.
 *   Ver contrato completo em buscar.h.
 */
int getListaRest(AppDados *db, const char *nome_rest,
                 Restaurante *resultado, int maxResultados) {

    /* Validação de parâmetros */
    if (db == NULL || nome_rest == NULL || resultado == NULL || maxResultados <= 0)
        return BUSCAR_NOME_INVALIDO;

    /* Nome vazio: PDF §2.4 – "Se o usuário não digitar nada, nada deve aparecer" */
    if (nome_rest[0] == '\0')
        return 0;

    int encontrados = 0;
    for (int i = 0; i < db->nRestaurantes && encontrados < maxResultados; i++) {
        if (contemSubstring(db->restaurantes[i].nome, nome_rest)) {
            resultado[encontrados] = db->restaurantes[i];
            encontrados++;
        }
    }

    return encontrados;
}
