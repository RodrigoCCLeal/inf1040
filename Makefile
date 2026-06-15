# Makefile - Foodies
# Modulos: Avaliacao, Postar, Buscar, Feed, Perfil, Principal
#
# Uso:
#   make                 - compila o programa principal e todos os testes
#   make foodies         - compila apenas o programa principal
#   make testes          - compila e executa todos os testes
#   make limpar          - remove binarios gerados

CC     = gcc
CFLAGS = -Wall -Wextra -std=c11 -I.

# --- Diretorios ---
DIR_AVAL   = avaliacao
DIR_POST   = postar
DIR_BUSCAR = buscar
DIR_FEED   = feed
DIR_PERF   = perfil
DIR_PRINC  = principal
DIR_TEST   = testes

# --- Fontes dos modulos ---
SRC_AVAL   = $(DIR_AVAL)/avaliacao.c
SRC_POST   = $(DIR_POST)/postar.c
SRC_BUSCAR = $(DIR_BUSCAR)/buscar.c
SRC_FEED   = $(DIR_FEED)/feed.c
SRC_PERF   = $(DIR_PERF)/perfil.c
SRC_PRINC  = $(DIR_PRINC)/principal.c
SRC_MAIN   = main.c

# --- Fontes dos testes ---
TEST_AVAL   = $(DIR_TEST)/teste_avaliacao.c
TEST_POST   = $(DIR_TEST)/teste_postar.c
TEST_BUSCAR = $(DIR_TEST)/teste_buscar.c
TEST_FEED   = $(DIR_TEST)/teste_feed.c
TEST_PERF   = $(DIR_TEST)/teste_perfil.c
TEST_PRINC  = $(DIR_TEST)/teste_principal.c

# --- Executaveis ---
BIN_AVAL   = $(DIR_TEST)/teste_avaliacao
BIN_POST   = $(DIR_TEST)/teste_postar
BIN_BUSCAR = $(DIR_TEST)/teste_buscar
BIN_FEED   = $(DIR_TEST)/teste_feed
BIN_PERF   = $(DIR_TEST)/teste_perfil
BIN_PRINC  = $(DIR_TEST)/teste_principal
BIN_MAIN   = foodies

# ================================================================
# COMPILACAO
# ================================================================

# Alvo padrao compila o foodies e os testes
all: $(BIN_MAIN) $(BIN_AVAL) $(BIN_POST) $(BIN_BUSCAR) $(BIN_FEED) $(BIN_PERF) $(BIN_PRINC)

# Regra unificada para compilar o executavel do sistema
$(BIN_MAIN): $(SRC_MAIN) $(SRC_PRINC) $(SRC_PERF) $(SRC_POST) $(SRC_AVAL) $(SRC_BUSCAR) $(SRC_FEED)
	$(CC) $(CFLAGS) -o $@ $^ -lm

# Avaliacao - usa math.h, requer -lm
$(BIN_AVAL): $(TEST_AVAL) $(SRC_AVAL)
	$(CC) $(CFLAGS) -o $@ $^ -lm

# Postar
$(BIN_POST): $(TEST_POST) $(SRC_POST)
	$(CC) $(CFLAGS) -o $@ $^

# Buscar - depende de postar.c (verificaLogin)
$(BIN_BUSCAR): $(TEST_BUSCAR) $(SRC_BUSCAR) $(SRC_POST)
	$(CC) $(CFLAGS) -o $@ $^

# Feed - depende de postar.c (verificaLogin)
$(BIN_FEED): $(TEST_FEED) $(SRC_FEED) $(SRC_POST)
	$(CC) $(CFLAGS) -o $@ $^

# Perfil - gerencia perfis.json
$(BIN_PERF): $(TEST_PERF) $(SRC_PERF)
	$(CC) $(CFLAGS) -o $@ $^

# Principal - coordena todos os JSONs; depende de perfil.c e avaliacao.c
$(BIN_PRINC): $(TEST_PRINC) $(SRC_PRINC) $(SRC_PERF) $(SRC_AVAL)
	$(CC) $(CFLAGS) -o $@ $^ -lm

# ================================================================
# EXECUCAO DOS TESTES
# ================================================================

testes: all
	@echo ""
	@echo "--- teste_avaliacao ---"
	./$(BIN_AVAL)
	@echo ""
	@echo "--- teste_postar ---"
	./$(BIN_POST)
	@echo ""
	@echo "--- teste_buscar ---"
	./$(BIN_BUSCAR)
	@echo ""
	@echo "--- teste_feed ---"
	./$(BIN_FEED)
	@echo ""
	@echo "--- teste_perfil ---"
	./$(BIN_PERF)
	@echo ""
	@echo "--- teste_principal ---"
	./$(BIN_PRINC)

# Alvos individuais de conveniencia
foodies: $(BIN_MAIN)

teste_avaliacao: $(BIN_AVAL)
	./$(BIN_AVAL)

teste_postar: $(BIN_POST)
	./$(BIN_POST)

teste_buscar: $(BIN_BUSCAR)
	./$(BIN_BUSCAR)

teste_feed: $(BIN_FEED)
	./$(BIN_FEED)

teste_perfil: $(BIN_PERF)
	./$(BIN_PERF)

teste_principal: $(BIN_PRINC)
	./$(BIN_PRINC)

# ================================================================
# LIMPEZA
# ================================================================

limpar:
	rm -f $(BIN_MAIN) $(BIN_AVAL) $(BIN_POST) $(BIN_BUSCAR) $(BIN_FEED) $(BIN_PERF) $(BIN_PRINC)

.PHONY: all testes limpar foodies \
        teste_avaliacao teste_postar teste_buscar \
        teste_feed teste_perfil teste_principal