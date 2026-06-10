#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "pratos.h"

#define BUF_JSON (1 << 20)  /* 1 MB — suficiente para centenas de pratos */

/* ---- banco de dados em memória ---- */
static Prato banco[MAX_PRATOS];
static int   total = 0;

/* ================================================================
 * Utilitários de parsing JSON (sem biblioteca externa)
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

static int parse_int(const char **p, int *val)
{
    skip_ws(p);
    char *end;
    long v = strtol(*p, &end, 10);
    if (end == *p) return 0;
    *val = (int)v;
    *p   = end;
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

/* pula qualquer valor JSON: string, número, objeto ou array */
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

static int parse_prato_obj(const char **p, Prato *pr)
{
    skip_ws(p);
    if (!expect_char(p, '{')) return 0;
    memset(pr, 0, sizeof *pr);

    while (1) {
        skip_ws(p);
        if (**p == '}') { (*p)++; return 1; }
        if (**p == ',') { (*p)++; continue; }
        if (**p == '\0') return 0;

        char key[64];
        if (!parse_string(p, key, sizeof key)) return 0;
        skip_ws(p);
        if (!expect_char(p, ':')) return 0;

        if      (strcmp(key, "IDPrato")    == 0) parse_int   (p, &pr->IDPrato);
        else if (strcmp(key, "nome_prato") == 0) parse_string(p, pr->nome_prato, MAX_NOME_PRATO);
        else if (strcmp(key, "descricao")  == 0) parse_string(p, pr->descricao,  MAX_DESC_PRATO);
        else if (strcmp(key, "CNPJ_rest")  == 0) parse_ll    (p, &pr->CNPJ_rest);
        else if (strcmp(key, "endereco")   == 0) parse_string(p, pr->endereco,   MAX_END_PRATO);
        else                                      skip_value  (p);
    }
}

/* ================================================================
 * Carregamento e persistência JSON
 * ================================================================ */

void initPratos(const char *arquivo)
{
    total = 0;
    srand((unsigned)time(NULL));

    FILE *f = fopen(arquivo, "r");
    if (!f) return;   /* primeira execução: arquivo ainda não existe */

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

        if (strcmp(key, "pratos") != 0) { skip_value(&p); continue; }

        skip_ws(&p);
        if (!expect_char(&p, '[')) break;

        while (1) {
            skip_ws(&p);
            if (*p == ']') { p++; break; }
            if (*p == ',') { p++; continue; }
            if (*p == '\0') break;
            if (total < MAX_PRATOS) {
                if (!parse_prato_obj(&p, &banco[total])) break;
                total++;
            } else {
                skip_value(&p);
            }
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

void savePratos(const char *arquivo)
{
    FILE *f = fopen(arquivo, "w");
    if (!f) return;

    fputs("{\n  \"pratos\": [\n", f);
    for (int i = 0; i < total; i++) {
        const Prato *pr = &banco[i];
        fputs("    {\n", f);
        fprintf(f, "      \"IDPrato\": %d,\n",     pr->IDPrato);
        fputs("      \"nome_prato\": \"", f); write_esc(f, pr->nome_prato); fputs("\",\n", f);
        fputs("      \"descricao\": \"",  f); write_esc(f, pr->descricao);  fputs("\",\n", f);
        fprintf(f, "      \"CNPJ_rest\": %lld,\n", pr->CNPJ_rest);
        fputs("      \"endereco\": \"",   f); write_esc(f, pr->endereco);   fputs("\"\n",  f);
        fprintf(f, "    }%s\n", i < total - 1 ? "," : "");
    }
    fputs("  ]\n}\n", f);
    fclose(f);
}

/* ================================================================
 * Funções de acesso (interface pública)
 * ================================================================ */

Prato *getPratos(int IDPrato)
{
    for (int i = 0; i < total; i++)
        if (banco[i].IDPrato == IDPrato)
            return &banco[i];
    return NULL;
}

ListaPratos *getListaPratos(const char *nome_prato)
{
    ListaPratos *lista = malloc(sizeof *lista);
    if (!lista) return NULL;
    lista->quantidade = 0;
    lista->itens = malloc(sizeof(Prato) * (total > 0 ? total : 1));
    if (!lista->itens) { free(lista); return NULL; }

    /* converte busca para minúsculas */
    char busca[MAX_NOME_PRATO];
    int bi = 0;
    for (const char *s = nome_prato; *s && bi < MAX_NOME_PRATO - 1; s++)
        busca[bi++] = (char)tolower((unsigned char)*s);
    busca[bi] = '\0';

    for (int i = 0; i < total; i++) {
        char lower[MAX_NOME_PRATO];
        int li = 0;
        for (const char *s = banco[i].nome_prato; *s && li < MAX_NOME_PRATO - 1; s++)
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

ListaPratos *getMenu(long long CNPJ)
{
    ListaPratos *lista = malloc(sizeof *lista);
    if (!lista) return NULL;
    lista->quantidade = 0;
    lista->itens = malloc(sizeof(Prato) * (total > 0 ? total : 1));
    if (!lista->itens) { free(lista); return NULL; }

    for (int i = 0; i < total; i++)
        if (banco[i].CNPJ_rest == CNPJ)
            lista->itens[lista->quantidade++] = banco[i];

    if (lista->quantidade == 0) {
        free(lista->itens);
        lista->itens = NULL;
    }
    return lista;
}

ListaPratos *getFeedPratos(void)
{
    int n = total < MAX_FEED ? total : MAX_FEED;

    ListaPratos *lista = malloc(sizeof *lista);
    if (!lista) return NULL;
    lista->quantidade = 0;

    if (n == 0) { lista->itens = NULL; return lista; }

    lista->itens = malloc(sizeof(Prato) * n);
    if (!lista->itens) { free(lista); return NULL; }

    /* Fisher-Yates parcial: seleciona n itens únicos aleatórios */
    int indices[MAX_PRATOS];
    for (int i = 0; i < total; i++) indices[i] = i;
    for (int i = 0; i < n; i++) {
        int j = i + rand() % (total - i);
        int tmp = indices[i]; indices[i] = indices[j]; indices[j] = tmp;
        lista->itens[lista->quantidade++] = banco[indices[i]];
    }
    return lista;
}

void freeListaPratos(ListaPratos *lista)
{
    if (!lista) return;
    free(lista->itens);
    free(lista);
}
