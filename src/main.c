#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

// Definição de estruturas
typedef struct {
    int pagina;
    int quadro;
} ItemTabelaPagina;

typedef struct {
    int pid;
    unsigned char *memoriaVirtual;
    int tamanho;
    ItemTabelaPagina *tabelaPaginas;
    int numPaginas;
} Processo;

typedef struct {
    unsigned char *bitmap; 
    int *pids;             
    int *paginas;          
    int totalQuadros;
    int quadrosLivres;
    int tamQuadro;
} MemoriaFisica;

// Funções auxiliares
int ehPotenciaDeDois(int valor) {
    return (valor > 0) && ((valor & (valor - 1)) == 0);
}

int obterValorConfiguracao(const char *descricao, int limite) {
    int valor;
    do {
        printf("Digite o %s (em bytes, potência de dois", descricao);
        if (limite > 0) printf(", <= %d", limite);
        printf("): ");
        scanf("%d", &valor);
        if (!ehPotenciaDeDois(valor) || (limite > 0 && valor > limite)) {
            printf("Valor inválido.\n");
        }
    } while (!ehPotenciaDeDois(valor) || (limite > 0 && valor > limite));
    return valor;
}

void inicializarMemoriaFisica(MemoriaFisica *memoria, int tamanhoMemFisica, int tamQuadro) {
    memoria->totalQuadros = tamanhoMemFisica / tamQuadro;
    memoria->bitmap = calloc(memoria->totalQuadros, sizeof(unsigned char)); 
    memoria->pids = calloc(memoria->totalQuadros, sizeof(int));             
    memoria->paginas = calloc(memoria->totalQuadros, sizeof(int));          
    for (int i = 0; i < memoria->totalQuadros; i++) {
        memoria->paginas[i] = -1; 
    }
    memoria->quadrosLivres = memoria->totalQuadros;
    memoria->tamQuadro = tamQuadro;
}

void imprimirMemoriaFisica(MemoriaFisica *memoria) {
    printf("Memória Física (%d quadros):\n", memoria->totalQuadros);
    for (int i = 0; i < memoria->totalQuadros; i++) {
        printf("Quadro %02d: ", i);
        if (memoria->bitmap[i] == 0) { // Livre
            for (int j = 0; j < memoria->tamQuadro; j++) {
                printf("livre ");
            }
        } else { // Ocupado
            printf("ocupado[PID=%d, Pág=%d] ", memoria->pids[i], memoria->paginas[i]);
        }
        printf("\n");
    }
    printf("\n");
    printf("Porcentagem de memória livre: %.2f%%\n", (memoria->quadrosLivres / (float)memoria->totalQuadros) * 100.0);
}

int gerarPID() {
    return rand() % 10000 + 1; 
}

int alocarQuadroAleatorio(MemoriaFisica *memoria, int pid, int pagina) {
    if (memoria->quadrosLivres == 0) return -1; 

    int tentativa;
    do {
        tentativa = rand() % memoria->totalQuadros;
    } while (memoria->bitmap[tentativa]); 

    memoria->bitmap[tentativa] = 1; 
    memoria->pids[tentativa] = pid; 
    memoria->paginas[tentativa] = pagina; 
    memoria->quadrosLivres--;
    return tentativa;
}

Processo criarProcesso(int tamanho, MemoriaFisica *memoria) {
    int pid = gerarPID();
    Processo proc;
    proc.pid = pid;
    proc.tamanho = tamanho;
    proc.numPaginas = (tamanho + memoria->tamQuadro - 1) / memoria->tamQuadro;
    proc.memoriaVirtual = malloc(tamanho);
    proc.tabelaPaginas = malloc(proc.numPaginas * sizeof(ItemTabelaPagina));

    for (int i = 0; i < tamanho; i++) {
        proc.memoriaVirtual[i] = rand() % 256; 
    }

    for (int i = 0; i < proc.numPaginas; i++) {
        int quadro = alocarQuadroAleatorio(memoria, pid, i);
        if (quadro == -1) {
            printf("Memória física insuficiente. Processo incompleto.\n");
            break;
        }
        proc.tabelaPaginas[i].pagina = i;
        proc.tabelaPaginas[i].quadro = quadro;
    }

    return proc;
}

void imprimirTabelaPaginas(Processo *proc) {
    printf("Tabela de Páginas - Processo PID: %d\n", proc->pid);
    for (int i = 0; i < proc->numPaginas; i++) {
        printf("Página %d -> Quadro %d\n", proc->tabelaPaginas[i].pagina, proc->tabelaPaginas[i].quadro);
    }
}

// Função principal
int main() {
    srand(time(0)); 

    int tamMemFisica = obterValorConfiguracao("tamanho da memória física", 0);
    int tamPagina = obterValorConfiguracao("tamanho das páginas", tamMemFisica);
    int tamMaxProcesso = obterValorConfiguracao("tamanho máximo de processo", tamMemFisica);

    MemoriaFisica memoria;
    inicializarMemoriaFisica(&memoria, tamMemFisica, tamPagina);

    Processo processos[100]; 
    int numProcessos = 0;

    while (1) {
        printf("\n=== Menu de Operações ===\n");
        printf("1. Ver Memória Física\n");
        printf("2. Criar Processo\n");
        printf("3. Tabela de Páginas de um Processo\n");
        printf("4. Sair\n");
        printf("Escolha uma opção: ");
        int opcao;
        scanf("%d", &opcao);
        printf("\n");

        switch (opcao) {
            case 1:
                imprimirMemoriaFisica(&memoria);
                break;
            case 2: {
                if (memoria.quadrosLivres == 0) {
                    printf("Memória física cheia. Não é possível criar novos processos.\n");
                    break;
                }
                int tamanho;
                printf("Digite o tamanho do processo: ");
                scanf("%d", &tamanho);

                if (!ehPotenciaDeDois(tamanho) || tamanho > tamMaxProcesso) {
                    printf("Tamanho do processo inválido.\n");
                    break;
                }

                processos[numProcessos++] = criarProcesso(tamanho, &memoria);
                printf("Processo criado com PID %d.\n", processos[numProcessos - 1].pid);
                break;
            }
            case 3: {
                int pid;
                printf("Digite o PID do processo: ");
                scanf("%d", &pid);
                int encontrado = 0;
                for (int i = 0; i < numProcessos; i++) {
                    if (processos[i].pid == pid) {
                        imprimirTabelaPaginas(&processos[i]);
                        encontrado = 1;
                        break;
                    }
                }
                if (!encontrado) {
                    printf("Processo com PID %d não encontrado.\n", pid);
                }
                break;
            }
            case 4:
                printf("\n");
                printf("Programa Encerrado!\n");
                for (int i = 0; i < numProcessos; i++) {
                    free(processos[i].memoriaVirtual);
                    free(processos[i].tabelaPaginas);
                }
                free(memoria.bitmap);
                free(memoria.pids);
                free(memoria.paginas);
                return 0;
            default:
                printf("Opção inválida.\n");
        }
    }
}
