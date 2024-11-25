#ifndef PROCESSO_H
#define PROCESSO_H

typedef struct {
    int id_processo;
    unsigned char *memoria_logica;
    int tamanho_logico;
    int *tabela_paginas;
} Processo;

Processo criarProcesso(int id, int tamanho);
void visualizarTabelaPaginas(Processo *processo);

#endif
