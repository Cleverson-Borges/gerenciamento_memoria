#include "processo.h"
#include <stdio.h>
#include <stdlib.h>

Processo criarProcesso(int id, int tamanho) {
    Processo p;
    p.id_processo = id;
    p.tamanho_logico = tamanho;
    p.memoria_logica = (unsigned char *)malloc(tamanho);
    p.tabela_paginas = (int *)malloc((tamanho / 64) * sizeof(int));

    for (int i = 0; i < tamanho; i++) {
        p.memoria_logica[i] = rand() % 256;
    }

    return p;
}

void visualizarTabelaPaginas(Processo *processo) {
    // Implementação da visualização da tabela de páginas
}
