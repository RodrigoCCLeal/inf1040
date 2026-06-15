#ifndef FEED_H
#define FEED_H

#include "pratos.h"
#include "restaurantes.h"
#include "perfil.h"

// Função para entrar no Feed
int enterFeed(int CPF);

// Função para obter pratos aleatórios (20 pratos)
// Exclui pratos já avaliados pelo usuário
ListaPratos* getFeedPratos(int CPF_usuario);

// Função para obter restaurantes aleatórios (6 restaurantes)
ListaRestaurantes* getFeedRestaurantes(void);

#endif
