#ifndef MEMORIA_H
#define MEMORIA_H

typedef struct {
    unsigned char *memoria_fisica;
    int *quadros_livres;
} MemoriaFisica;

void inicializarMemoria(MemoriaFisica *memoria, int tamanho);
void visualizarMemoria(MemoriaFisica *memoria);

#endif
