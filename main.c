/*
 * main.c
 * Foodies - Aplicacao principal
 *
 * Fluxo de vida:
 *   1. iniciarApp()  -> aloca banco, carrega todos os JSONs:
 *                       perfis.json, avaliacoes.json,
 *                       restaurantes.json (fixo), pratos.json (fixo)
 *   2. Loop do menu principal
 *   3. encerrarApp() -> salva perfis.json e avaliacoes.json, libera memoria
 *                       (restaurantes.json e pratos.json nao sao alterados)
 *
 * Modulos usados:
 *   principal   -> ciclo de vida e persistencia
 *   perfil      -> login, cadastro e consulta de usuarios
 *   postar      -> acesso a pagina de postagem
 *   avaliacao   -> criar, ver e editar avaliacoes
 *   buscar      -> valida acesso a tela de busca
 *   feed        -> valida acesso a tela de feed
 *   pratos      -> busca e feed de pratos (dados fixos do JSON)
 *   restaurante -> busca e feed de restaurantes (dados fixos do JSON)
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
 * PONTEIRO GLOBAL PARA O BANCO (necessario para o handler de SIGINT)
 *
 * signal handlers nao recebem argumentos proprios do programa, entao
 * o banco precisa ser acessivel via variavel global. O ponteiro so e
 * atribuido apos iniciarApp() e anulado apos encerrarApp() para evitar
 * double-free caso o handler seja invocado num momento inesperado.
 * ================================================================= */
static AppDados *g_db = NULL;

/*
 * tratarSIGINT
 *   Handler registrado para SIGINT (Ctrl+C).
 *   Garante que os dados em memoria sejam persistidos nos JSONs mesmo
 *   quando o usuario encerra o programa de forma abrupta, respeitando
 *   a regra de que os arquivos so sao escritos no momento do encerramento.
 */
static void tratarSIGINT(int sig) {
    (void)sig;
    printf("\nPrograma interrompido. Salvando dados...\n");
    if (g_db != NULL) {
        encerrarApp(g_db);
        g_db = NULL;
    }
    exit(0);
}

/* =================================================================
 * HELPERS DE INPUT
 * ================================================================= */

/*
 * lerLinha
 *   Le uma linha do stdin e remove o '\n' do final.
 *   Retorna 0 em sucesso, -1 se nada foi lido.
 */
static int lerLinha(char *buf, int tam) {
    if (fgets(buf, tam, stdin) == NULL) return -1;
    buf[strcspn(buf, "\n")] = '\0';
    return 0;
}

/*
 * lerInt
 *   Le um inteiro do stdin. Retorna o valor ou -1 em erro.
 */
static int lerInt(void) {
    char buf[32];
    if (lerLinha(buf, sizeof(buf)) < 0) return -1;
    return atoi(buf);
}

/*
 * lerCPF
 *   Le um CPF (11 digitos) do stdin.
 */
static long long int lerCPF(void) {
    char buf[32];
    if (lerLinha(buf, sizeof(buf)) < 0) return 0;
    return atoll(buf);
}

/* =================================================================
 * TELAS DOS MODULOS
 * ================================================================= */

/* -----------------------------------------------------------------
 * telaPerfil
 *   Tela de login e cadastro. Retorna quando o usuario entra
 *   com sucesso (db->cpfLogado fica preenchido).
 * ----------------------------------------------------------------- */
static void telaPerfil(AppDados *db) {
    int opcao;
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

            int ret = enterPerfil(db, cpf, senha);
            if (ret == PERFIL_OK) {
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

            int ret = criarPerfil(db, cpf, nome, senha);
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
            encerrarApp(db);
            g_db = NULL;
            printf("Dados salvos. Ate logo!\n");
            exit(0);
        } else {
            printf("Opcao invalida.\n");
        }

    } while (db->cpfLogado == 0);
}

/* -----------------------------------------------------------------
 * telaPostar
 *   Permite ao usuario logado postar uma avaliacao de um prato.
 * ----------------------------------------------------------------- */
static void telaPostar(AppDados *db) {
    if (enterPostar(db, db->cpfLogado) != POSTAR_OK) {
        printf("Acesso negado. Faca login primeiro.\n");
        return;
    }

    printf("\n=== POSTAR AVALIACAO ===\n");

    /* Mostrar lista de pratos disponíveis */
    printf("Pratos disponiveis:\n");
    for (int i = 0; i < db->nPratos; i++) {
        printf("  [%d] %s\n", db->pratos[i].idPrato, db->pratos[i].nome);
    }

    printf("ID do prato: ");
    int idPrato = lerInt();

    printf("Nota (0 a 5): ");
    int nota = lerInt();

    char comentario[TAM_COMENTARIO];
    printf("Comentario (Enter para deixar vazio): ");
    lerLinha(comentario, sizeof(comentario));

    /* Confirmacao antes de postar (PDF 2.5) */
    printf("Confirmar avaliacao? (1=Sim / 0=Nao): ");
    if (lerInt() != 1) {
        printf("Avaliacao cancelada.\n");
        return;
    }

    const char *txt = (comentario[0] == '\0') ? NULL : comentario;
    int ret = postAval(db, db->cpfLogado, idPrato, txt, (float)nota);

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
 *   Permite buscar pratos e restaurantes pelo nome.
 *   getListaPratos e getListaRest pertencem aos modulos Pratos e
 *   Restaurante, que sao os donos desses dados.
 * ----------------------------------------------------------------- */
static void telaBuscar(AppDados *db) {
    if (enterBuscar(db, db->cpfLogado) != BUSCAR_OK) {
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
        int qtd = getListaPratos(db, nome, resultados, 100);

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

            /* Mostrar avaliacoes do prato */
            Avaliacao avals[100];
            int nAvals = verAval(db, resultados[i].idPrato, avals, 100);
            if (nAvals > 0) {
                float soma = 0;
                for (int j = 0; j < nAvals; j++) soma += avals[j].nota;
                printf("      Media: %.1f (%d avaliacao(oes))\n",
                       soma / nAvals, nAvals);
            } else {
                printf("      Sem avaliacoes ainda.\n");
            }
        }

    } else if (opcao == 2) {
        char nome[TAM_NOME];
        printf("Nome do restaurante: ");
        lerLinha(nome, sizeof(nome));

        Restaurante resultados[100];
        int qtd = getListaRest(db, nome, resultados, 100);

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
 *   Mostra pratos e restaurantes aleatorios ao usuario logado.
 *   getFeedPratos e getFeedRest pertencem aos modulos Pratos e
 *   Restaurante, que sao os donos desses dados.
 * ----------------------------------------------------------------- */
static void telaFeed(AppDados *db) {
    if (enterFeed(db, db->cpfLogado) != FEED_OK) {
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
        int qtd = getFeedPratos(db, db->cpfLogado, pratos, PRATOS_FEED_QTD);

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
        int qtd = getFeedRest(db, rests, REST_FEED_QTD);

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
 *   Permite ao usuario ver e editar suas proprias avaliacoes.
 * ----------------------------------------------------------------- */
static void telaAvaliacao(AppDados *db) {
    if (verificaLogin(db, db->cpfLogado) != LOGIN_OK) {
        printf("Acesso negado. Faca login primeiro.\n");
        return;
    }

    printf("\n=== MINHAS AVALIACOES ===\n");

    int encontradas = 0;
    for (int i = 0; i < db->nAvaliacoes; i++) {
        if (db->avaliacoes[i].cpf != db->cpfLogado) continue;

        /* Buscar nome do prato via modulo Pratos */
        Prato *p = getPratos(db, db->avaliacoes[i].idPrato);
        const char *nomePrato = (p != NULL) ? p->nome : "Prato desconhecido";

        printf("  [ID:%d] %s - Nota: %.0f",
               db->avaliacoes[i].idAval,
               nomePrato,
               db->avaliacoes[i].nota);
        if (db->avaliacoes[i].comentario[0] != '\0')
            printf(" - \"%s\"", db->avaliacoes[i].comentario);
        printf("\n");
        encontradas++;
    }

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

    /* Confirmacao antes de editar (PDF 2.6) */
    printf("Confirmar edicao? (1=Sim / 0=Nao): ");
    if (lerInt() != 1) {
        printf("Edicao cancelada.\n");
        return;
    }

    int ret = editAval(db, idAval, db->cpfLogado, novoTxt, novaNota);
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

/*
 * menuPrincipal
 *   Loop central do programa. So acessivel apos login.
 *   Navega para cada modulo conforme escolha do usuario.
 */
static void menuPrincipal(AppDados *db) {
    int opcao;
    do {
        Usuario *u = getUsuario(db, db->cpfLogado);
        printf("\n=== FOODIES | Logado: %s ===\n",
               u ? u->nome : "Desconhecido");
        printf("1. Postar avaliacao\n");
        printf("2. Buscar pratos e restaurantes\n");
        printf("3. Feed (sugestoes aleatorias)\n");
        printf("4. Minhas avaliacoes\n");
        printf("5. Sair da conta\n");
        printf("0. Encerrar programa\n");
        printf("Opcao: ");
        opcao = lerInt();

        switch (opcao) {
            case 1: telaPostar(db);    break;
            case 2: telaBuscar(db);    break;
            case 3: telaFeed(db);      break;
            case 4: telaAvaliacao(db); break;
            case 5:
                printf("Saindo da conta...\n");
                db->cpfLogado = 0;
                return;
            case 0:
                printf("Encerrando o programa...\n");
                encerrarApp(db);
                g_db = NULL;
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

    /*
     * 1. Iniciar: aloca banco e carrega todos os JSONs.
     *    iniciarApp() chama internamente:
     *      carregarPerfis(db)       -> perfis.json
     *      carregarRestaurantes(db) -> restaurantes.json (fixo, nao alterado)
     *      carregarPratos(db)       -> pratos.json (fixo, nao alterado)
     *      carregarAvaliacoes(db)   -> avaliacoes.json
     */
    AppDados *db = iniciarApp();
    if (db == NULL) {
        printf("Erro fatal: nao foi possivel alocar memoria.\n");
        return 1;
    }

    /*
     * 2. Registrar handler para SIGINT (Ctrl+C).
     *    Feito apos iniciarApp() para que g_db aponte para um banco valido.
     *    O handler chama encerrarApp(), garantindo que os dados sejam
     *    salvos mesmo em caso de interrupcao abrupta.
     */
    g_db = db;
    signal(SIGINT, tratarSIGINT);

    /*
     * 3. Loop: tela de perfil (login/cadastro) -> menu principal.
     *    Repete quando o usuario sai da conta sem encerrar o programa.
     */
    while (1) {
        telaPerfil(db);
        menuPrincipal(db);

        if (db->cpfLogado == 0) break;
    }

    /*
     * 4. Encerrar: salva perfis.json e avaliacoes.json e libera memoria.
     *    Caminho de saida natural (usuario deslogou sem escolher encerrar).
     */
    encerrarApp(db);
    g_db = NULL;
    printf("Dados salvos. Ate logo!\n");
    return 0;
}