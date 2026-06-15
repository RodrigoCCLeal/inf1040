#include "buscar.h"
#include "pratos.h"
#include "restaurantes.h"
#include "perfil.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Função auxiliar para converter string para lowercase
static void converterParaLowercase(char* str) {
    if (str == NULL) return;
    for (int i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

// Função auxiliar para comparação case-insensitive
static int contemSubstring(const char* texto, const char* substring) {
    if (texto == NULL || substring == NULL) {
        return 0;
    }

    // Criar cópias para converter para lowercase
    char* texto_lower = (char*)malloc(strlen(texto) + 1);
    char* substring_lower = (char*)malloc(strlen(substring) + 1);
    
    strcpy(texto_lower, texto);
    strcpy(substring_lower, substring);
    
    converterParaLowercase(texto_lower);
    converterParaLowercase(substring_lower);
    
    int resultado = strstr(texto_lower, substring_lower) != NULL ? 1 : 0;
    
    free(texto_lower);
    free(substring_lower);
    
    return resultado;
}

// Função para entrar na página de busca
// Retorna:
// 0 - Entrou na página Buscar
// 1 - Tentativa de entrar na página Buscar com CPF inválido
// 2 - Parâmetro inválido
int enterBuscar(int CPF) {
    // Validar parâmetro
    if (CPF == 0) {
        return 2; // Parâmetro inválido
    }

    // Verificar se o CPF é válido (verifica se usuário existe)
    Perfil* usuario = getUsuario(CPF);
    if (usuario == NULL) {
        return 1; // CPF inválido
    }

    liberarPerfil(usuario);
    return 0; // Sucesso
}

// Função para buscar pratos por nome
// Retorna:
// - lista_pratos com os pratos encontrados
// - NULL se o nome é inválido
// - lista vazia se nenhum prato encontrado
ListaPratos* buscarPratos(const char* nome_prato) {
    // Validar parâmetro
    if (nome_prato == NULL) {
        return NULL; // Parâmetro inválido
    }

    // Se não digitou nada, nada deverá aparecer
    if (strlen(nome_prato) == 0) {
        ListaPratos* listaVazia = (ListaPratos*)malloc(sizeof(ListaPratos));
        listaVazia->pratos = NULL;
        listaVazia->total = 0;
        return listaVazia;
    }

    // Obter a lista de pratos que correspondem ao nome
    return getListaPratos(nome_prato);
}

// Função para buscar restaurantes por nome
// Retorna:
// - lista_rest com os restaurantes encontrados
// - NULL se o nome é inválido
// - lista vazia se nenhum restaurante encontrado
ListaRestaurantes* buscarRestaurantes(const char* nome_rest) {
    // Validar parâmetro
    if (nome_rest == NULL) {
        return NULL; // Parâmetro inválido
    }

    // Se não digitou nada, nada deverá aparecer
    if (strlen(nome_rest) == 0) {
        ListaRestaurantes* listaVazia = (ListaRestaurantes*)malloc(sizeof(ListaRestaurantes));
        listaVazia->restaurantes = NULL;
        listaVazia->total = 0;
        return listaVazia;
    }

    // Obter a lista de restaurantes que correspondem ao nome
    return getListaRest(nome_rest);
}
