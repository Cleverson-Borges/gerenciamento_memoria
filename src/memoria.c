#include "memoria.h"
#include <stdio.h>
#include <stdlib.h>

void inicializarMemoria(MemoriaFisica *memoria, int tamanho) {
    memoria->memoria_fisica = (unsigned char *)malloc(tamanho);
    memoria->quadros_livres = (int *)malloc(tamanho / 64 * sizeof(int));
    for (int i = 0; i < tamanho / 64; i++) {
        memoria->quadros_livres[i] = 1;
    }
}

void visualizarMemoria(MemoriaFisica *memoria) {
    // Implementação da visualização da memória
}
