#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "restaurantes.h"
#include "pratos.h"     /* RESTAURANTES é cliente de PRATOS */

#define BUF_JSON (1 << 20)

/* ---- banco de dados em memória ---- */
static Restaurante banco[MAX_RESTAURANTES];
static int         total = 0;

/* ================================================================
 * Utilitários de parsing JSON
 * ================================================================ */

static void skip_ws(const char **p)
{
    while (**p && isspace((unsigned char)**p)) (*p)++;
}

static int expect_char(const char **p, char c)
{
    skip_ws(p);
    if (**p != c) return 0;
    (*p)++;
    return 1;
}

static int parse_string(const char **p, char *buf, int maxlen)
{
    skip_ws(p);
    if (**p != '"') return 0;
    (*p)++;
    int i = 0;
    while (**p && **p != '"') {
        char ch;
        if (**p == '\\') {
            (*p)++;
            switch (**p) {
                case '"':  ch = '"';  break;
                case '\\': ch = '\\'; break;
                case 'n':  ch = '\n'; break;
                case 't':  ch = '\t'; break;
                default:   ch = **p;  break;
            }
        } else {
            ch = **p;
        }
        if (i < maxlen - 1) buf[i++] = ch;
        (*p)++;
    }
    if (**p == '"') (*p)++;
    buf[i] = '\0';
    return 1;
}

static int parse_ll(const char **p, long long *val)
{
    skip_ws(p);
    char *end;
    *val = strtoll(*p, &end, 10);
    if (end == *p) return 0;
    *p   = end;
    return 1;
}

static void skip_value(const char **p)
{
    skip_ws(p);
    if (**p == '"') {
        (*p)++;
        while (**p && **p != '"') {
            if (**p == '\\') (*p)++;
            if (**p) (*p)++;
        }
        if (**p == '"') (*p)++;
    } else if (**p == '{' || **p == '[') {
        char open = **p, close = (**p == '{') ? '}' : ']';
        int depth = 1;
        (*p)++;
        while (**p && depth > 0) {
            if (**p == '"') {
                (*p)++;
                while (**p && **p != '"') {
                    if (**p == '\\') (*p)++;
                    if (**p) (*p)++;
                }
                if (**p == '"') (*p)++;
            } else {
                if (**p == open)  depth++;
                if (**p == close) depth--;
                (*p)++;
            }
        }
    } else {
        while (**p && **p != ',' && **p != '}' && **p != ']' &&
               !isspace((unsigned char)**p))
            (*p)++;
    }
}

static int parse_rest_obj(const char **p, Restaurante *r)
{
    skip_ws(p);
    if (!expect_char(p, '{')) return 0;
    memset(r, 0, sizeof *r);

    while (1) {
        skip_ws(p);
        if (**p == '}') { (*p)++; return 1; }
        if (**p == ',') { (*p)++; continue; }
        if (**p == '\0') return 0;

        char key[64];
        if (!parse_string(p, key, sizeof key)) return 0;
        skip_ws(p);
        if (!expect_char(p, ':')) return 0;

        if      (strcmp(key, "CNPJ")      == 0) parse_ll    (p, &r->CNPJ);
        else if (strcmp(key, "endereco")  == 0) parse_string(p, r->endereco,  MAX_END_REST);
        else if (strcmp(key, "nome_rest") == 0) parse_string(p, r->nome_rest, MAX_NOME_REST);
        else                                     skip_value  (p);
    }
}

/* ================================================================
 * Carregamento e persistência JSON
 * ================================================================ */

/*
 * Regra: restaurante único = mesmo CNPJ + mesmo endereço.
 * Restaurantes sem nenhum prato cadastrado são descartados (requer
 * que initPratos() já tenha sido chamado antes de initRestaurantes()).
 */
void initRestaurantes(const char *arquivo)
{
    total = 0;
    srand((unsigned)time(NULL));

    FILE *f = fopen(arquivo, "r");
    if (!f) return;

    char *buf = malloc(BUF_JSON);
    if (!buf) { fclose(f); return; }

    size_t lido = fread(buf, 1, BUF_JSON - 1, f);
    buf[lido] = '\0';
    fclose(f);

    const char *p = buf;
    if (!expect_char(&p, '{')) { free(buf); return; }

    while (1) {
        skip_ws(&p);
        if (*p == '}' || *p == '\0') break;
        if (*p == ',') { p++; continue; }

        char key[64];
        if (!parse_string(&p, key, sizeof key)) break;
        skip_ws(&p);
        if (!expect_char(&p, ':')) break;

        if (strcmp(key, "restaurantes") != 0) { skip_value(&p); continue; }

        skip_ws(&p);
        if (!expect_char(&p, '[')) break;

        while (1) {
            skip_ws(&p);
            if (*p == ']') { p++; break; }
            if (*p == ',') { p++; continue; }
            if (*p == '\0') break;

            Restaurante r;
            if (!parse_rest_obj(&p, &r)) break;

            /* ignora duplicatas: mesmo CNPJ e mesmo endereço */
            int duplicado = 0;
            for (int i = 0; i < total; i++) {
                if (banco[i].CNPJ == r.CNPJ &&
                    strcmp(banco[i].endereco, r.endereco) == 0) {
                    duplicado = 1;
                    break;
                }
            }
            if (duplicado) continue;

            /* descarta restaurantes sem pratos */
            ListaPratos *menu = getMenu(r.CNPJ);
            int tem_prato = menu && menu->quantidade > 0;
            freeListaPratos(menu);
            if (!tem_prato) continue;

            if (total < MAX_RESTAURANTES)
                banco[total++] = r;
        }
        break;
    }

    free(buf);
}

static void write_esc(FILE *f, const char *s)
{
    while (*s) {
        switch (*s) {
            case '"':  fputs("\\\"", f); break;
            case '\\': fputs("\\\\", f); break;
            case '\n': fputs("\\n",  f); break;
            case '\t': fputs("\\t",  f); break;
            default:   fputc(*s, f);     break;
        }
        s++;
    }
}

void saveRestaurantes(const char *arquivo)
{
    FILE *f = fopen(arquivo, "w");
    if (!f) return;

    fputs("{\n  \"restaurantes\": [\n", f);
    for (int i = 0; i < total; i++) {
        const Restaurante *r = &banco[i];
        fputs("    {\n", f);
        fprintf(f, "      \"CNPJ\": %lld,\n",       r->CNPJ);
        fputs("      \"endereco\": \"",  f); write_esc(f, r->endereco);  fputs("\",\n", f);
        fputs("      \"nome_rest\": \"", f); write_esc(f, r->nome_rest); fputs("\"\n",  f);
        fprintf(f, "    }%s\n", i < total - 1 ? "," : "");
    }
    fputs("  ]\n}\n", f);
    fclose(f);
}

/* ================================================================
 * Funções de acesso (interface pública)
 * ================================================================ */

ListaRest *getListaRest(const char *nome_rest)
{
    ListaRest *lista = malloc(sizeof *lista);
    if (!lista) return NULL;
    lista->quantidade = 0;
    lista->itens = malloc(sizeof(Restaurante) * (total > 0 ? total : 1));
    if (!lista->itens) { free(lista); return NULL; }

    char busca[MAX_NOME_REST];
    int bi = 0;
    for (const char *s = nome_rest; *s && bi < MAX_NOME_REST - 1; s++)
        busca[bi++] = (char)tolower((unsigned char)*s);
    busca[bi] = '\0';

    for (int i = 0; i < total; i++) {
        char lower[MAX_NOME_REST];
        int li = 0;
        for (const char *s = banco[i].nome_rest; *s && li < MAX_NOME_REST - 1; s++)
            lower[li++] = (char)tolower((unsigned char)*s);
        lower[li] = '\0';

        if (strstr(lower, busca))
            lista->itens[lista->quantidade++] = banco[i];
    }

    if (lista->quantidade == 0) {
        free(lista->itens);
        lista->itens = NULL;
    }
    return lista;
}

ListaRest *getFeedRest(void)
{
    int n = total < MAX_FEED_REST ? total : MAX_FEED_REST;

    ListaRest *lista = malloc(sizeof *lista);
    if (!lista) return NULL;
    lista->quantidade = 0;

    if (n == 0) { lista->itens = NULL; return lista; }

    lista->itens = malloc(sizeof(Restaurante) * n);
    if (!lista->itens) { free(lista); return NULL; }

    int indices[MAX_RESTAURANTES];
    for (int i = 0; i < total; i++) indices[i] = i;
    for (int i = 0; i < n; i++) {
        int j = i + rand() % (total - i);
        int tmp = indices[i]; indices[i] = indices[j]; indices[j] = tmp;
        lista->itens[lista->quantidade++] = banco[indices[i]];
    }
    return lista;
}

void freeListaRest(ListaRest *lista)
{
    if (!lista) return;
    free(lista->itens);
    free(lista);
}
