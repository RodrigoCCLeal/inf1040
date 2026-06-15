/*
 * main.c
 * Foodies - Aplicacao principal
 *
 * Fluxo de vida:
 * 1. iniciarApp()  -> aloca banco, carrega perfis.json e avaliacoes.json
 * 2. Loop do menu principal
 * 3. encerrarApp() -> salva perfis.json e avaliacoes.json, libera memoria
 *
 * Modulos usados:
 * principal -> ciclo de vida e persistencia de avaliacoes
 * perfil    -> login, cadastro e consulta de usuarios
 * postar    -> acesso a pagina de postagem
 * avaliacao -> criar, ver e editar avaliacoes
 * buscar    -> pesquisar pratos e restaurantes
 * feed      -> recomendacoes aleatorias
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "principal/principal.h"
#include "perfil/perfil.h"
#include "postar/postar.h"
#include "avaliacao/avaliacao.h"
#include "buscar/buscar.h"
#include "feed/feed.h"

/* =================================================================
 * DADOS INICIAIS (pratos e restaurantes fixos no sistema)
 * O projeto especifica que usuarios nao podem editar pratos ou
 * restaurantes, entao eles sao carregados aqui como dados fixos.
 * Em producao viriam de um JSON proprio; aqui populamos direto.
 * ================================================================= */

/*
 * popularDadosFixos
 * Insere os restaurantes e pratos pre-cadastrados no sistema.
 * Chamada uma vez por iniciarApp antes do menu principal.
 * Nao sobrescreve se ja houver pratos (segunda execucao).
 */
static void popularDadosFixos(AppDados *db) {
    if (db->nPratos > 0) return;  /* ja populado em execucao anterior */

    /* --- Restaurantes --- */
    struct { long long int cnpj; const char *nome; const char *end; } rests[] = {
        { 11111111000101LL, "Toca da Traira - Barra",     "Av. Niemeyer, 121"    },
        { 11111111000102LL, "Toca da Traira - Freguesia", "Rua Laurinda, 36"     },
        { 22222222000103LL, "Soba",                       "R. Bambina, 124"      },
        { 33333333000104LL, "Aprazivel",                  "R. Aprazivel, 62"     },
        { 44444444000105LL, "Bar do Mineiro",             "R. Paschoal, 99"      },
        { 55555555000106LL, "Churrascaria Fogo de Chao",  "Av. das Americas, 777"},
        { 66666666000107LL, "Boteco do Brasil",           "R. Voluntarios, 10"   },
        { 77777777000108LL, "Outback Barra",              "Shopping Downtown"    },
    };
    int nR = (int)(sizeof(rests) / sizeof(rests[0]));
    for (int i = 0; i < nR && db->nRestaurantes < MAX_RESTAURANTES; i++) {
        db->restaurantes[db->nRestaurantes].cnpj = rests[i].cnpj;
        strncpy(db->restaurantes[db->nRestaurantes].nome, rests[i].nome,
                TAM_NOME - 1);
        strncpy(db->restaurantes[db->nRestaurantes].endereco, rests[i].end,
                TAM_ENDERECO - 1);
        db->nRestaurantes++;
    }

    /* --- Pratos --- */
    struct { long long int cnpj; const char *nome; const char *desc; } pratos[] = {
        { 11111111000101LL, "Peixe Frito",         "Peixe fresco frito na hora"          },
        { 11111111000101LL, "Camarao Grelhado",    "Camarao grelhado com alho"            },
        { 11111111000102LL, "Traira Assada",       "Traira assada com ervas"              },
        { 11111111000102LL, "Caldeirada",          "Caldeirada de frutos do mar"          },
        { 22222222000103LL, "Soba Tradicional",    "Macarrao japones com caldo"           },
        { 22222222000103LL, "Gyoza",               "Pastel japones grelhado"              },
        { 33333333000104LL, "Frango ao Molho",     "Frango caipira ao molho pardo"        },
        { 33333333000104LL, "Feijao Tropeiro",     "Feijao com farinha e torresmo"        },
        { 44444444000105LL, "Bolinho de Bacalhau", "Bolinhos crocantes de bacalhau"       },
        { 44444444000105LL, "Porcao de Frango",    "Frango a passarinho com farofa"       },
        { 55555555000106LL, "Picanha",             "Picanha grelhada no carvao"           },
        { 55555555000106LL, "Costela",             "Costela bovina assada lentamente"     },
        { 66666666000107LL, "Porcao de Calabresa", "Calabresa acebolada com pao"          },
        { 66666666000107LL, "Bolinho de Mandioca", "Bolinho frito de mandioca com queijo" },
        { 77777777000108LL, "Bloomin Onion",       "Cebola empanada com molho especial"  },
        { 77777777000108LL, "Baby Back Ribs",      "Costelinha suina ao molho barbecue"   },
        { 11111111000101LL, "Moqueca de Peixe",    "Moqueca baiana com dendê"             },
        { 22222222000103LL, "Temaki",              "Cone de alga com salmao e arroz"      },
        { 33333333000104LL, "Rabada",              "Rabada com agriao"                    },
        { 44444444000105LL, "Coxinha de Frango",   "Coxinha artesanal de frango"          },
        { 55555555000106LL, "Fraldinha",           "Fraldinha grelhada com chimichurri"   },
        { 66666666000107LL, "Torresmo",            "Torresmo crocante frito"              },
        { 77777777000108LL, "Steak de Frango",     "Frango grelhado com batatas"          },
        { 11111111000102LL, "Atum Selado",         "Atum selado com gergelim"             },
        { 33333333000104LL, "Feijoada",            "Feijoada completa aos sabados"        },
    };
    int nP = (int)(sizeof(pratos) / sizeof(pratos[0]));
    for (int i = 0; i < nP && db->nPratos < MAX_PRATOS; i++) {
        db->pratos[db->nPratos].idPrato         = db->proximoIdPrato++;
        db->pratos[db->nPratos].cnpjRestaurante = pratos[i].cnpj;
        strncpy(db->pratos[db->nPratos].nome,     pratos[i].nome, TAM_NOME     - 1);
        strncpy(db->pratos[db->nPratos].descricao, pratos[i].desc, TAM_COMENTARIO - 1);
        db->nPratos++;
    }
}

/* =================================================================
 * HELPERS DE INPUT
 * ================================================================= */

/*
 * lerLinha
 * Le uma linha do stdin e remove o '\n' do final.
 * Retorna 0 em sucesso, -1 se nada foi lido.
 */
static int lerLinha(char *buf, int tam) {
    if (fgets(buf, tam, stdin) == NULL) return -1;
    buf[strcspn(buf, "\n")] = '\0';
    return 0;
}

/*
 * lerInt
 * Le um inteiro do stdin. Retorna o valor ou -1 em erro.
 */
static int lerInt(void) {
    char buf[32];
    if (lerLinha(buf, sizeof(buf)) < 0) return -1;
    return atoi(buf);
}

/*
 * lerCPF
 * Le um CPF (11 digitos) do stdin.
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
 * Tela de login e cadastro. Retorna quando o usuario entra
 * com sucesso (db->cpfLogado fica preenchido).
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
            /* --- Login --- */
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
            /* --- Cadastro --- */
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
            printf("Dados salvos. Ate logo!\n");
            exit(0);
        } else {
            printf("Opcao invalida.\n");
        }

    } while (db->cpfLogado == 0);
}

/* -----------------------------------------------------------------
 * telaPostar
 * Permite ao usuario logado postar uma avaliacao de um prato.
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
    int confirma = lerInt();
    if (confirma != 1) {
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
 * Permite buscar pratos e restaurantes pelo nome.
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

        Prato resultados[BUSCAR_MAX_RESULTADOS];
        int qtd = getListaPratos(db, nome, resultados, BUSCAR_MAX_RESULTADOS);

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

        Restaurante resultados[BUSCAR_MAX_RESULTADOS];
        int qtd = getListaRest(db, nome, resultados, BUSCAR_MAX_RESULTADOS);

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
 * Mostra pratos e restaurantes aleatorios ao usuario logado.
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
        Prato pratos[FEED_QTD_PRATOS];
        int qtd = getFeedPratos(db, db->cpfLogado, pratos, FEED_QTD_PRATOS);

        if (qtd == FEED_INSUFICIENTE) {
            printf("Nao ha pratos suficientes para o feed (minimo 20).\n");
            return;
        }
        if (qtd == FEED_ERRO_PARAM) {
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
        Restaurante rests[FEED_QTD_RESTAURANTES];
        int qtd = getFeedRest(db, rests, FEED_QTD_RESTAURANTES);

        if (qtd == FEED_INSUFICIENTE) {
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
 * Permite ao usuario ver e editar suas proprias avaliacoes.
 * ----------------------------------------------------------------- */
static void telaAvaliacao(AppDados *db) {
    if (verificaLogin(db, db->cpfLogado) != LOGIN_OK) {
        printf("Acesso negado. Faca login primeiro.\n");
        return;
    }

    printf("\n=== MINHAS AVALIACOES ===\n");

    /* Listar todas as avaliacoes do usuario logado */
    int encontradas = 0;
    for (int i = 0; i < db->nAvaliacoes; i++) {
        if (db->avaliacoes[i].cpf != db->cpfLogado) continue;

        /* Achar nome do prato */
        char nomePrato[TAM_NOME] = "Prato desconhecido";
        for (int j = 0; j < db->nPratos; j++) {
            if (db->pratos[j].idPrato == db->avaliacoes[i].idPrato) {
                strncpy(nomePrato, db->pratos[j].nome, TAM_NOME - 1);
                break;
            }
        }

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
 * Loop central do programa. So acessivel apos login.
 * Navega para cada modulo conforme escolha do usuario.
 */
static void menuPrincipal(AppDados *db) {
    int opcao;
    do {
        /* Mostrar usuario logado */
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
                return;  /* volta para telaPerfil */
            case 0:
                printf("Encerrando o programa...\n");
                encerrarApp(db);                  /* MODIFICADO: Salva dados antes de cair fora */
                printf("Dados salvos. Ate logo!\n");
                exit(0);                          /* MODIFICADO: Termina o processo imediatamente */
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
     * 1. Iniciar: aloca banco, carrega perfis.json e avaliacoes.json.
     * Se os arquivos nao existirem, comeca com banco vazio.
     */
    AppDados *db = iniciarApp();
    if (db == NULL) {
        printf("Erro fatal: nao foi possivel alocar memoria.\n");
        return 1;
    }

    /*
     * 2. Popular pratos e restaurantes fixos do sistema.
     * Nao faz nada se ja existirem (segunda execucao).
     */
    popularDadosFixos(db);

    /*
     * 3. Loop: tela de perfil (login/cadastro) -> menu principal.
     * Repete quando o usuario sai da conta sem encerrar.
     */
    while (1) {
        telaPerfil(db);      /* bloqueia ate login bem-sucedido */
        menuPrincipal(db);   /* retorna quando sai da conta     */

        /* Se cpfLogado ainda for 0 apos menuPrincipal, opcao 0 foi escolhida */
        if (db->cpfLogado == 0) break;
    }

    /*
     * 4. Encerrar de segurança: limpa memória caso saia naturalmente por algum motivo.
     */
    encerrarApp(db);

    printf("Dados salvos. Ate logo!\n");
    return 0;
}