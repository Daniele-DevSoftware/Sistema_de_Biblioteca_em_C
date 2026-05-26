#ifndef BANCO_DE_DADOS_H
#define BANCO_DE_DADOS_H

#include "../Base/Livro.h"

int inicializarBancoDeDados(void);
void fecharBancoDeDados(void);

Biblioteca* criarBibliotecaComBanco(int capacidadeInicial);

int carregarLivrosDoBanco(Biblioteca *bib);
int inserirLivroNoBanco(const Livro *livro);
int removerLivroDoBanco(int id);
int marcarLivroComoLidoNoBanco(int id);

#endif
