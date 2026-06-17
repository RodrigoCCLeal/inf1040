/*
 * main.c
 * Foodies - Aplicacao principal
 *
 * Fluxo de vida:
 * 1. iniciarApp()  -> Inicializa e carrega os estados de todos os modulos.
 * 2. Loop do menu principal (Baseado no estado de sessao do modulo Postar)
 * 3. encerrarApp() -> Solicita que cada modulo salve seus dados mutaveis.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "principal/principal.h"
#include "perfil/perfil.h"
#include "postar/postar.h"
#include "avaliacao/avaliacao.h"
#include "buscar/buscar.h"
#include "feed/feed.h"
#include "pratos/pratos.h"
#include "restaurante/restaurante.h"

/* =================================================================
 * SINALIZADOR GLOBAL DE INICIALIZAÇÃO (Para o handler de SIGINT)
 * ================================================================= */
static int app_inicializado = 0;

/*
 * tratarSIGINT
 * Handler registrado para SIGINT (Ctrl+C).
 * Garante a persistencia delegando o salvamento ao encerrarApp().
 */
static void tratarSIGINT(int sig) {
    (void)sig;
    printf("\nPrograma interrompido. Salvando dados...\n");
    if (app_inicializado) {
        encerrarApp();
        app_inicializado = 0;
    }
    exit(0);
}

/* =================================================================
 * HELPERS DE INPUT
 * ================================================================= */

static int lerLinha(char *buf, int tam) {
    if (fgets(buf, tam, stdin) == NULL) return -1;
    buf[strcspn(buf, "\n")] = '\0';
    return 0;
}

static int lerInt(void) {
    char buf[32];
    if (lerLinha(buf, sizeof(buf)) < 0) return -1;
    return atoi(buf);
}

static long long int lerCPF(void) {
    char buf[32];
    if (lerLinha(buf, sizeof(buf)) < 0) return 0;
    return atoll(buf);
}

/* =================================================================
 * TELAS DOS MÓDULOS
 * ================================================================= */

/* -----------------------------------------------------------------
 * telaPerfil
 * ----------------------------------------------------------------- */
static void telaPerfil(void) {
    int opcao;
    long long int cpf_atual;
    
    do {
        printf("\n=== PERFIL ===\n");
        printf("1. Entrar (login)\n");
        printf("2. Criar conta\n");
        printf("0. Sair do programa\n");
        printf("Opcao: ");
        opcao = lerInt();

        if (opcao == 1) {
            long long int cpf;
            char senha[TAM_SENHA];

            printf("CPF (11 digitos): ");
            cpf = lerCPF();
            printf("Senha: ");
            lerLinha(senha, sizeof(senha));

            /* O modulo Perfil valida os dados e injeta o CPF no modulo Postar via API */
            int ret = enterPerfil(cpf, senha);
            if (ret == PERFIL_OK) {
                definirSessaoAtual(cpf);
                printf("Login realizado com sucesso! Bem-vindo(a).\n");
            } else if (ret == PERFIL_DADOS_INVALIDOS) {
                printf("CPF ou senha incorretos.\n");
            } else {
                printf("Parametro invalido.\n");
            }

        } else if (opcao == 2) {
            long long int cpf;
            char nome[TAM_NOME];
            char senha[TAM_SENHA];

            printf("CPF (11 digitos): ");
            cpf = lerCPF();
            printf("Nome: ");
            lerLinha(nome, sizeof(nome));
            printf("Senha: ");
            lerLinha(senha, sizeof(senha));

            int ret = criarPerfil(cpf, nome, senha);
            if (ret == PERFIL_CRIADO) {
                printf("Conta criada com sucesso!\n");
            } else if (ret == PERFIL_JA_EXISTE) {
                printf("CPF ja cadastrado.\n");
            } else if (ret == PERFIL_VALORES_INV) {
                printf("CPF deve ter 11 digitos e nome/senha nao podem ser vazios.\n");
            } else {
                printf("Parametro invalido.\n");
            }

        } else if (opcao == 0) {
            printf("Encerrando o programa...\n");
            encerrarApp();
            app_inicializado = 0;
            printf("Dados salvos. Ate logo!\n");
            exit(0);
        } else {
            printf("Opcao invalida.\n");
        }

        /* Verifica se ha alguma sessao ativa consultando a API de Postar */
        cpf_atual = obterCpfLogado(); 

    } while (cpf_atual == 0);
}

/* -----------------------------------------------------------------
 * telaPostar
 * ----------------------------------------------------------------- */
static void telaPostar(long long int cpfLogado) {
    if (enterPostar(cpfLogado) != POSTAR_OK) {
        printf("Acesso negado. Faca login primeiro.\n");
        return;
    }

    printf("\n=== POSTAR AVALIACAO ===\n");

    /* Solicita as informacoes basicas dos pratos usando a listagem do modulo */
    printf("Pratos disponiveis:\n");
    listarNomesPratos();

    printf("ID do prato: ");
    int idPrato = lerInt();

    printf("Nota (0 a 5): ");
    int nota = lerInt();

    char comentario[TAM_COMENTARIO];
    printf("Comentario (Enter para deixar vazio): ");
    lerLinha(comentario, sizeof(comentario));

    printf("Confirmar avaliacao? (1=Sim / 0=Nao): ");
    if (lerInt() != 1) {
        printf("Avaliacao cancelada.\n");
        return;
    }

    const char *txt = (comentario[0] == '\0') ? NULL : comentario;
    int ret = postAval(cpfLogado, idPrato, txt, (float)nota);

    if (ret == AVAL_OK || ret == AVAL_OK_SEM_TXT) {
        printf("Avaliacao postada com sucesso!\n");
    } else if (ret == AVAL_ERRO_ID_PRATO) {
        printf("Prato nao encontrado.\n");
    } else if (ret == AVAL_ERRO_NOTA) {
        printf("Nota invalida. Use um numero inteiro de 0 a 5.\n");
    } else {
        printf("Erro ao postar avaliacao.\n");
    }
}

/* -----------------------------------------------------------------
 * telaBuscar
 * ----------------------------------------------------------------- */
static void telaBuscar(long long int cpfLogado) {
    if (enterBuscar(cpfLogado) != BUSCAR_OK) {
        printf("Acesso negado. Faca login primeiro.\n");
        return;
    }

    int opcao;
    printf("\n=== BUSCAR ===\n");
    printf("1. Buscar prato por nome\n");
    printf("2. Buscar restaurante por nome\n");
    printf("0. Voltar\n");
    printf("Opcao: ");
    opcao = lerInt();

    if (opcao == 1) {
        char nome[TAM_NOME];
        printf("Nome do prato: ");
        lerLinha(nome, sizeof(nome));

        Prato resultados[100];
        int qtd = getListaPratos(nome, resultados, 100);

        if (qtd <= 0) {
            printf("Nenhum prato encontrado.\n");
            return;
        }
        printf("Encontrados %d prato(s):\n", qtd);
        for (int i = 0; i < qtd; i++) {
            printf("  [%d] %s - %s\n",
                   resultados[i].idPrato,
                   resultados[i].nome,
                   resultados[i].descricao);

            /* Busca a media calculada via motor de avaliacoes */
            float media;
            int nAvals = obterMediaPrato(resultados[i].idPrato, &media);
            if (nAvals > 0) {
                printf("      Media: %.1f (%d avaliacao(oes))\n", media, nAvals);
            } else {
                printf("      Sem avaliacoes ainda.\n");
            }
        }

    } else if (opcao == 2) {
        char nome[TAM_NOME];
        printf("Nome do restaurante: ");
        lerLinha(nome, sizeof(nome));

        Restaurante resultados[100];
        int qtd = getListaRest(nome, resultados, 100);

        if (qtd <= 0) {
            printf("Nenhum restaurante encontrado.\n");
            return;
        }
        printf("Encontrados %d restaurante(s):\n", qtd);
        for (int i = 0; i < qtd; i++) {
            printf("  %s - %s\n",
                   resultados[i].nome,
                   resultados[i].endereco);
        }
    }
}

/* -----------------------------------------------------------------
 * telaFeed
 * ----------------------------------------------------------------- */
static void telaFeed(long long int cpfLogado) {
    if (enterFeed(cpfLogado) != FEED_OK) {
        printf("Acesso negado. Faca login primeiro.\n");
        return;
    }

    int opcao;
    printf("\n=== FEED ===\n");
    printf("1. Ver pratos aleatorios (20)\n");
    printf("2. Ver restaurantes aleatorios (6)\n");
    printf("0. Voltar\n");
    printf("Opcao: ");
    opcao = lerInt();

    if (opcao == 1) {
        Prato pratos[PRATOS_FEED_QTD];
        int qtd = getFeedPratos(cpfLogado, pratos, PRATOS_FEED_QTD);

        if (qtd == PRATOS_INSUFICIENTE) {
            printf("Nao ha pratos suficientes para o feed (minimo 20).\n");
            return;
        }
        if (qtd == PRATOS_PARAM_INVALIDO) {
            printf("Erro de parametro.\n");
            return;
        }
        printf("Sugestoes de pratos:\n");
        for (int i = 0; i < qtd; i++) {
            printf("  [%d] %s - %s\n",
                   pratos[i].idPrato,
                   pratos[i].nome,
                   pratos[i].descricao);
        }

    } else if (opcao == 2) {
        Restaurante rests[REST_FEED_QTD];
        int qtd = getFeedRest(rests, REST_FEED_QTD);

        if (qtd == REST_INSUFICIENTE) {
            printf("Nao ha restaurantes suficientes para o feed (minimo 6).\n");
            return;
        }
        printf("Sugestoes de restaurantes:\n");
        for (int i = 0; i < qtd; i++) {
            printf("  %s - %s\n", rests[i].nome, rests[i].endereco);
        }
    }
}

/* -----------------------------------------------------------------
 * telaAvaliacao
 * ----------------------------------------------------------------- */
static void telaAvaliacao(long long int cpfLogado) {
    if (verificaLogin(cpfLogado) != LOGIN_OK) {
        printf("Acesso negado. Faca login primeiro.\n");
        return;
    }

    printf("\n=== MINHAS AVALIACOES ===\n");

    /* Solicita ao modulo de Avaliacao para renderizar as avaliacoes deste CPF */
    int encontradas = imprimirAvaliacoesUsuario(cpfLogado);

    if (encontradas == 0) {
        printf("Voce ainda nao fez nenhuma avaliacao.\n");
        return;
    }

    printf("\nDeseja editar uma avaliacao? (1=Sim / 0=Nao): ");
    if (lerInt() != 1) return;

    printf("ID da avaliacao a editar: ");
    int idAval = lerInt();

    printf("Nova nota (0 a 5, Enter para manter): ");
    char bufNota[16];
    lerLinha(bufNota, sizeof(bufNota));
    float novaNota = (bufNota[0] == '\0') ? -1.0f : (float)atoi(bufNota);

    printf("Novo comentario (Enter para manter): ");
    char novoComentario[TAM_COMENTARIO];
    lerLinha(novoComentario, sizeof(novoComentario));
    const char *novoTxt = (novoComentario[0] == '\0') ? NULL : novoComentario;

    printf("Confirmar edicao? (1=Sim / 0=Nao): ");
    if (lerInt() != 1) {
        printf("Edicao cancelada.\n");
        return;
    }

    int ret = editAval(idAval, cpfLogado, novoTxt, novaNota);
    if (ret == AVAL_OK) {
        printf("Avaliacao editada com sucesso!\n");
    } else if (ret == AVAL_ERRO_EDICAO) {
        printf("Avaliacao nao encontrada.\n");
    } else if (ret == AVAL_ERRO_PERMISSAO) {
        printf("Voce nao pode editar a avaliacao de outro usuario.\n");
    } else if (ret == AVAL_ERRO_NOTA) {
        printf("Nota invalida.\n");
    } else {
        printf("Erro ao editar.\n");
    }
}

/* =================================================================
 * MENU PRINCIPAL
 * ================================================================= */

static void menuPrincipal(void) {
    int opcao;
    long long int cpfLogado = obterCpfLogado();
    char nomeUsuario[TAM_NOME];
    
    obterNomeUsuario(cpfLogado, nomeUsuario);

    do {
        printf("\n=== FOODIES | Logado: %s ===\n", nomeUsuario);
        printf("1. Postar avaliacao\n");
        printf("2. Buscar pratos e restaurantes\n");
        printf("3. Feed (sugestoes aleatorias)\n");
        printf("4. Minhas avaliacoes\n");
        printf("5. Sair da conta\n");
        printf("0. Encerrar programa\n");
        printf("Opcao: ");
        opcao = lerInt();

        switch (opcao) {
            case 1: telaPostar(cpfLogado);    break;
            case 2: telaBuscar(cpfLogado);    break;
            case 3: telaFeed(cpfLogado);      break;
            case 4: telaAvaliacao(cpfLogado); break;
            case 5:
                printf("Saindo da conta...\n");
                encerrarSessaoAtual(); /* Limpa sessao no modulo Postar */
                return;
            case 0:
                printf("Encerrando o programa...\n");
                encerrarApp();
                app_inicializado = 0;
                printf("Dados salvos. Ate logo!\n");
                exit(0);
            default:
                printf("Opcao invalida.\n");
        }
    } while (opcao != 0);
}

/* =================================================================
 * MAIN
 * ================================================================= */

int main(void) {
    printf("=================================\n");
    printf("   Bem-vindo ao Foodies!\n");
    printf("=================================\n");

    /* 1. Iniciar: Coordenacao limpa e sem vazamento de ponteiros heap */
    if (iniciarApp() != 0) {
        printf("Erro fatal: nao foi possivel inicializar o sistema.\n");
        return 1;
    }

    app_inicializado = 1;
    signal(SIGINT, tratarSIGINT);

    /* 2. Loop principal controlado por APIs */
    while (1) {
        telaPerfil();
        menuPrincipal();
    }

    /* 3. Encerrar */
    encerrarApp();
    app_inicializado = 0;
    printf("Dados salvos. Ate logo!\n");
    return 0;
}