#include <stdio.h>
#include "memoria.h"
#include "processo.h"

int main() {
    MemoriaFisica memoria;
    inicializarMemoria(&memoria, 1024);

    int opcao;
    do {
        printf("Menu:\n1. Visualizar memória\n2. Criar processo\n3. Visualizar tabela de páginas\n4. Sair\n");
        scanf("%d", &opcao);
        switch (opcao) {
            case 1:
                visualizarMemoria(&memoria);
                break;
            case 2:
                // Código para criação de processo
                break;
            case 3:
                // Código para visualização da tabela de páginas
                break;
            case 4:
                printf("Saindo...\n");
                break;
            default:
                printf("Opção inválida!\n");
        }
    } while (opcao != 4);

    return 0;
}
