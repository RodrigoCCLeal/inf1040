#include "feed.h"
#include "pratos.h"
#include "restaurantes.h"
#include "perfil.h"
#include "avaliacao.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Função para entrar no Feed
// Retorna:
// 0 - Entrou na página Feed
// 1 - Tentativa de entrar na página Feed com CPF inválido
// 2 - Parâmetro inválido
int enterFeed(int CPF) {
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

// Função auxiliar para verificar se um prato foi avaliado pelo usuário
static int pratoAvaliado(int IDPrato, int CPF_usuario) {
    ListaAvaliacoes* avaliacoes = verAval(IDPrato);
    
    if (avaliacoes == NULL) {
        return 0; // Sem avaliações
    }

    int resultado = 0;
    for (int i = 0; i < avaliacoes->total; i++) {
        if (avaliacoes->avaliacoes[i].CPF == CPF_usuario) {
            resultado = 1;
            break;
        }
    }

    liberarListaAvaliacoes(avaliacoes);
    return resultado;
}

// Função para obter pratos aleatórios excluindo os já avaliados
ListaPratos* getFeedPratos(int CPF_usuario) {
    // Inicializar seed para números aleatórios
    srand(time(NULL));

    // Obter todos os pratos disponíveis
    ListaPratos* todosPratos = getListaPratos("");
    
    if (todosPratos == NULL || todosPratos->total == 0) {
        return NULL; // Falha: menos de 20 pratos disponíveis
    }

    // Contar pratos não avaliados
    int* pratosPossiveis = (int*)malloc(todosPratos->total * sizeof(int));
    int totalPossivel = 0;

    for (int i = 0; i < todosPratos->total; i++) {
        if (!pratoAvaliado(todosPratos->pratos[i].IDPrato, CPF_usuario)) {
            pratosPossiveis[totalPossivel++] = i;
        }
    }

    // Verificar se há pelo menos 20 pratos não avaliados
    if (totalPossivel < 20) {
        free(pratosPossiveis);
        liberarListaPratos(todosPratos);
        return NULL; // Falha: menos de 20 pratos disponíveis
    }

    // Criar lista de pratos para o feed
    ListaPratos* feed = (ListaPratos*)malloc(sizeof(ListaPratos));
    feed->pratos = (Prato*)malloc(20 * sizeof(Prato));
    feed->total = 20;

    // Selecionar 20 pratos aleatórios do conjunto de possíveis
    int* selecionados = (int*)malloc(20 * sizeof(int));
    int count = 0;

    while (count < 20) {
        int indiceAleatorio = rand() % totalPossivel;
        int jaFoiSelecionado = 0;

        // Verificar se já foi selecionado
        for (int i = 0; i < count; i++) {
            if (selecionados[i] == indiceAleatorio) {
                jaFoiSelecionado = 1;
                break;
            }
        }

        if (!jaFoiSelecionado) {
            selecionados[count] = indiceAleatorio;
            int pratoIndex = pratosPossiveis[indiceAleatorio];
            feed->pratos[count] = todosPratos->pratos[pratoIndex];
            count++;
        }
    }

    // Libertar memória temporária
    free(selecionados);
    free(pratosPossiveis);
    liberarListaPratos(todosPratos);

    return feed;
}

// Função para obter restaurantes aleatórios (6 restaurantes)
// Esta função delega para o módulo RESTAURANTES
ListaRestaurantes* getFeedRestaurantes(void) {
    // Inicializar seed para números aleatórios
    srand(time(NULL));

    // Obter todos os restaurantes através da API do módulo RESTAURANTES
    // (Implementação real virá do módulo restaurantes.c)
    // Para agora, retornar NULL indicando que não há 6 restaurantes disponíveis
    return NULL;
}
