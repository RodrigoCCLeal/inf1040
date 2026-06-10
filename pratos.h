#ifndef PRATOS_H
#define PRATOS_H

#define MAX_PRATOS     500
#define MAX_NOME_PRATO 100
#define MAX_DESC_PRATO 300
#define MAX_END_PRATO  200
#define MAX_FEED        20

typedef struct {
    int       IDPrato;
    char      nome_prato[MAX_NOME_PRATO];
    char      descricao [MAX_DESC_PRATO];
    long long CNPJ_rest;          /* CNPJ tem 14 dígitos → long long */
    char      endereco  [MAX_END_PRATO];
} Prato;

typedef struct {
    Prato *itens;
    int    quantidade;
} ListaPratos;

/* ciclo de vida do módulo */
void         initPratos(const char *arquivo);
void         savePratos(const char *arquivo);

/* funções de acesso */
Prato       *getPratos     (int IDPrato);
ListaPratos *getListaPratos(const char *nome_prato);
ListaPratos *getMenu       (long long CNPJ);
ListaPratos *getFeedPratos (void);

void         freeListaPratos(ListaPratos *lista);

#endif /* PRATOS_H */
