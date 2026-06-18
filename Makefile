# Makefile - Foodies
# Modulos: Avaliacao, Postar, Buscar, Feed, Perfil, inicializar, Restaurante, Pratos
#
# Uso:
#   make                    - compila o programa principal e todos os testes
#   make app                - compila apenas o programa principal (gerando 'foodies')
#   make testes             - compila e executa todos os testes
#   make limpar             - remove binarios gerados

CC     = gcc
CFLAGS = -Wall -Wextra -std=c11 -I.

# --- Diretorios ---
DIR_AVAL   = avaliacao
DIR_POST   = postar
DIR_BUSCAR = buscar
DIR_FEED   = feed
DIR_PERF   = perfil
DIR_PRINC  = inicializar
DIR_REST   = restaurante
DIR_PRAT   = pratos
DIR_TEST   = testes

# --- Fontes dos modulos ---
SRC_AVAL   = $(DIR_AVAL)/avaliacao.c
SRC_POST   = $(DIR_POST)/postar.c
SRC_BUSCAR = $(DIR_BUSCAR)/buscar.c
SRC_FEED   = $(DIR_FEED)/feed.c
SRC_PERF   = $(DIR_PERF)/perfil.c
SRC_PRINC  = $(DIR_PRINC)/inicializar.c
SRC_REST   = $(DIR_REST)/restaurante.c
SRC_PRAT   = $(DIR_PRAT)/pratos.c
SRC_MAIN   = main.c

# --- Fontes dos testes ---
TEST_AVAL   = $(DIR_TEST)/teste_avaliacao.c
TEST_POST   = $(DIR_TEST)/teste_postar.c
TEST_BUSCAR = $(DIR_TEST)/teste_buscar.c
TEST_FEED   = $(DIR_TEST)/teste_feed.c
TEST_PERF   = $(DIR_TEST)/teste_perfil.c
TEST_PRINC  = $(DIR_TEST)/teste_inicializar.c
TEST_REST   = $(DIR_TEST)/teste_restaurante.c
TEST_PRAT   = $(DIR_TEST)/teste_pratos.c

# --- Executaveis ---
BIN_AVAL   = $(DIR_TEST)/teste_avaliacao
BIN_POST   = $(DIR_TEST)/teste_postar
BIN_BUSCAR = $(DIR_TEST)/teste_buscar
BIN_FEED   = $(DIR_TEST)/teste_feed
BIN_PERF   = $(DIR_TEST)/teste_perfil
BIN_PRINC  = $(DIR_TEST)/teste_inicializar
BIN_REST   = $(DIR_TEST)/teste_restaurante
BIN_PRAT   = $(DIR_TEST)/teste_pratos
BIN_MAIN   = foodies

# ================================================================
# COMPILACAO
# ================================================================

all: $(BIN_MAIN) $(BIN_AVAL) $(BIN_POST) $(BIN_BUSCAR) $(BIN_FEED) \
     $(BIN_PERF) $(BIN_PRINC) $(BIN_REST) $(BIN_PRAT)

# Programa inicializar - Garante link com todos os modulos do ecossistema
$(BIN_MAIN): $(SRC_MAIN) $(SRC_PRINC) $(SRC_PERF) $(SRC_POST) $(SRC_AVAL) $(SRC_BUSCAR) $(SRC_FEED) $(SRC_REST) $(SRC_PRAT)
	$(CC) $(CFLAGS) -o $@ $^ -lm

# Avaliacao - Agora depende de pratos.c devido a validacao getPratos()
$(BIN_AVAL): $(TEST_AVAL) $(SRC_AVAL) $(SRC_PRAT)
	$(CC) $(CFLAGS) -o $@ $^ -lm

# Postar - Depende exclusivamente do seu proprio motor de sessoes
$(BIN_POST): $(TEST_POST) $(SRC_POST)
	$(CC) $(CFLAGS) -o $@ $^

# Buscar - CORRIGIDO: Agora adicionado $(SRC_AVAL) para resolver 'verificarSeAvaliado'
$(BIN_BUSCAR): $(TEST_BUSCAR) $(SRC_BUSCAR) $(SRC_POST) $(SRC_REST) $(SRC_PRAT) $(SRC_AVAL)
	$(CC) $(CFLAGS) -o $@ $^ -lm

# Feed - CORRIGIDO: Adicionado $(SRC_REST) para resolver 'getFeedRest'
$(BIN_FEED): $(TEST_FEED) $(SRC_FEED) $(SRC_POST) $(SRC_PRAT) $(SRC_AVAL) $(SRC_REST)
	$(CC) $(CFLAGS) -o $@ $^ -lm

# Perfil - Depende de postar para injetar estados de sessao via definirSessaoAtual()
$(BIN_PERF): $(TEST_PERF) $(SRC_PERF) $(SRC_POST)
	$(CC) $(CFLAGS) -o $@ $^

# inicializar - Orquestrador central de cargas e salvamentos JSON
$(BIN_PRINC): $(TEST_PRINC) $(SRC_PRINC) $(SRC_PERF) $(SRC_AVAL) $(SRC_REST) $(SRC_PRAT) $(SRC_POST)
	$(CC) $(CFLAGS) -o $@ $^ -lm

# Restaurante - CORRIGIDO: Adicionado $(SRC_AVAL) para resolver a dependencia de pratos.c -> verificarSeAvaliado
$(BIN_REST): $(TEST_REST) $(SRC_REST) $(SRC_PRAT) $(SRC_AVAL)
	$(CC) $(CFLAGS) -o $@ $^ -lm

# Pratos - Agora depende de avaliacao devido a checagem verificarSeAvaliado() para o feed
$(BIN_PRAT): $(TEST_PRAT) $(SRC_PRAT) $(SRC_AVAL) $(SRC_PERF)
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
	@echo "--- teste_inicializar ---"
	./$(BIN_PRINC)
	@echo ""
	@echo "--- teste_restaurante ---"
	./$(BIN_REST)
	@echo ""
	@echo "--- teste_pratos ---"
	./$(BIN_PRAT)

# Alvos individuais de conveniencia
app: $(BIN_MAIN)

teste_avaliacao:  $(BIN_AVAL)  ; ./$(BIN_AVAL)
teste_postar:     $(BIN_POST)  ; ./$(BIN_POST)
teste_buscar:     $(BIN_BUSCAR); ./$(BIN_BUSCAR)
teste_feed:       $(BIN_FEED)  ; ./$(BIN_FEED)
teste_perfil:     $(BIN_PERF)  ; ./$(BIN_PERF)
teste_inicializar:  $(BIN_PRINC) ; ./$(BIN_PRINC)
teste_restaurante:$(BIN_REST)  ; ./$(BIN_REST)
teste_pratos:     $(BIN_PRAT)  ; ./$(BIN_PRAT)

# ================================================================
# LIMPEZA
# ================================================================

limpar:
	del /f /q $(BIN_MAIN).exe testes\teste_avaliacao.exe testes\teste_postar.exe testes\teste_buscar.exe testes\teste_feed.exe testes\teste_perfil.exe testes\teste_inicializar.exe testes\teste_restaurante.exe testes\teste_pratos.exe 2>nul || exit 0