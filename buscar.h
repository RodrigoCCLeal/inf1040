#ifndef BUSCAR_H
#define BUSCAR_H

#include "pratos.h"
#include "restaurantes.h"
#include "perfil.h"

// Função para entrar na página de busca
int enterBuscar(int CPF);

// Função para buscar pratos por nome
ListaPratos* buscarPratos(const char* nome_prato);

// Função para buscar restaurantes por nome
ListaRestaurantes* buscarRestaurantes(const char* nome_rest);

#endif
