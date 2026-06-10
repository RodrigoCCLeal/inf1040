#include "pratos.h"
#include "restaurantes.h"

/* ----------------------------------------------------------------
 * Acessores para Prato
 * Recebem void* para que Python não precise definir structs.
 * ---------------------------------------------------------------- */
int         prato_id       (const void *p) { return ((const Prato *)p)->IDPrato;    }
const char *prato_nome     (const void *p) { return ((const Prato *)p)->nome_prato; }
const char *prato_descricao(const void *p) { return ((const Prato *)p)->descricao;  }
long long   prato_cnpj     (const void *p) { return ((const Prato *)p)->CNPJ_rest;  }
const char *prato_endereco (const void *p) { return ((const Prato *)p)->endereco;   }

/* ----------------------------------------------------------------
 * Acessores para ListaPratos
 * ---------------------------------------------------------------- */
int   lista_pratos_qtd (const void *l)        { return ((const ListaPratos *)l)->quantidade;  }
void *lista_pratos_item(const void *l, int i) { return &((ListaPratos *)l)->itens[i];         }

/* ----------------------------------------------------------------
 * Acessores para Restaurante
 * ---------------------------------------------------------------- */
long long   rest_cnpj    (const void *r) { return ((const Restaurante *)r)->CNPJ;      }
const char *rest_nome    (const void *r) { return ((const Restaurante *)r)->nome_rest; }
const char *rest_endereco(const void *r) { return ((const Restaurante *)r)->endereco;  }

/* ----------------------------------------------------------------
 * Acessores para ListaRest
 * ---------------------------------------------------------------- */
int   lista_rest_qtd (const void *l)        { return ((const ListaRest *)l)->quantidade; }
void *lista_rest_item(const void *l, int i) { return &((ListaRest *)l)->itens[i];        }
