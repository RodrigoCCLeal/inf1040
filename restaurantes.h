#ifndef RESTAURANTES_H
#define RESTAURANTES_H

#define MAX_RESTAURANTES 200
#define MAX_NOME_REST    150
#define MAX_END_REST     200
#define MAX_FEED_REST     10

typedef struct {
    long long CNPJ;                  /* CNPJ tem 14 dígitos → long long */
    char      endereco [MAX_END_REST];
    char      nome_rest[MAX_NOME_REST];
} Restaurante;

typedef struct {
    Restaurante *itens;
    int          quantidade;
} ListaRest;

/* ciclo de vida do módulo — initRestaurantes deve ser chamado APÓS initPratos */
void       initRestaurantes(const char *arquivo);
void       saveRestaurantes(const char *arquivo);

/* funções de acesso */
ListaRest *getListaRest(const char *nome_rest);
ListaRest *getFeedRest (void);

void       freeListaRest(ListaRest *lista);

#endif /* RESTAURANTES_H */
