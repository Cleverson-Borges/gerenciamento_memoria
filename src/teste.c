#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> 

typedef struct pageTableItem_t {
    int page;
    int frame;
} pageTableItem_t;

typedef struct process_t {
    int pid;
    unsigned char *memoryArr;
    pageTableItem_t **pageTable; // array de pageTableItems
    int size;
    struct process_t *next;
} process_t;

typedef struct freeFrame_t {
    struct freeFrame_t *next;
    int frameId;
} freeFrame_t;

int getConfigValue(char *configValueStr);
int getConfigValueLimited(char *configValueStr, int physicalMemorySize);
int getPIDValueFromUser(process_t *processListHead);
int getProcessSize(int processMaxSize, int pageSize, int freeFramesNum, int *processValidated);
int isSizePowerOfTwo(int size);
void zeroAllArray(unsigned char *arr, int arrSize);
void printAddressToValue(unsigned char *arr, int arrSize, int frameSize, float freeFramesPercentage);
void createProcess(int pid, int processSize, unsigned char *physicalMemory, int pageSize, int totalPages, freeFrame_t **freeFramesHeadPtr, process_t **processListHeadPtr);
void insertProcessIntoList(process_t *process, process_t **processListHeadPtr);
void fillAllArray(unsigned char *arr, int arrSize, int pid);
int isFrameEmpty(freeFrame_t *headPtr, int frameNum);
void fillFrame(unsigned char *physicalMemory, unsigned char *virtualMemory, int frameNum, int pageNum, int pageSize);
void removeFrameFromList(freeFrame_t **headPtr, int frameNum);
void printPageTable(process_t *processListHead, int pidInput, int pageSize);
int isPIDAvailable(process_t *processListHead, int pid);
freeFrame_t *insertAllFreeFramesInList(int frameNum);
int getFreeFramesNum(freeFrame_t *headPtr);

int main() {
    printf("Inicializando gerenciador de memória...\n");
    char *physicalMemoryStr = "tamanho da memoria física";
    int physicalMemorySize = getConfigValue(physicalMemoryStr);
    char *pageSizeStr = "tamanho de paginas";
    int pageSize = getConfigValueLimited(pageSizeStr, physicalMemorySize); // 4
    char *processMaxSizeStr = "tamanho maximo de processo";
    int processMaxSize = getConfigValueLimited(processMaxSizeStr, physicalMemorySize); // 16
    int totalPages = physicalMemorySize / pageSize;
    srand(time(0));

    unsigned char physicalMemory[physicalMemorySize];

    zeroAllArray(physicalMemory, physicalMemorySize);
    freeFrame_t *freeFramesHead = insertAllFreeFramesInList(totalPages);

    process_t *processListHead = NULL;

    system("clear");

    for(;;) {
        printf("Escolha uma operacao.\n");
        printf("(v)er a memória física;\n");
        printf("(c)riar processo;\n");
        printf("(b)uscar tabela de páginas;\n");
        printf("(s)air.\n");

        char key;
        scanf(" %c", &key);

        system("clear");

        if(key == 'v') {
            int freeFramesNum = getFreeFramesNum(freeFramesHead);
            float freeFramesPercentage = freeFramesNum / (float) totalPages * 100;
            printAddressToValue(physicalMemory, physicalMemorySize, pageSize, freeFramesPercentage);
        } else if(key == 'c') {
            int PID, processSize;
            int processValidated = 0;
            int freeFramesNum = getFreeFramesNum(freeFramesHead);
            if(freeFramesNum) {
                printf("Vamos criar um processo.\n");
                PID = getPIDValueFromUser(processListHead);
                processSize = getProcessSize(processMaxSize, pageSize, freeFramesNum, &processValidated);
                if(processValidated) {
                    createProcess(PID, processSize, physicalMemory, pageSize, totalPages, &freeFramesHead, &processListHead);
                }
            } else {
                printf("Nao ha memoria disponivel para criar novos processos.\n");
            }
        } else if(key == 'b') {
            int pidInput;
            printf("Informe um PID: ");
            scanf(" %d", &pidInput);

            printPageTable(processListHead, pidInput, pageSize);
        } else if(key == 's') {
            break;
        }

        printf("\n");
    }

    return 0;
}

int getConfigValue(char *configValueStr) {
    int configValue;
    for(;;) {
        printf("Insira um valor para o %s em bytes (potencia de dois): ", configValueStr);
        scanf(" %d", &configValue);
        if(!isSizePowerOfTwo(configValue)) {
            printf("\nTamanho invalido. ");
        } else {
            return configValue;
        }
    }
}

int getConfigValueLimited(char *configValueStr, int physicalMemorySize) {
    int configValue;
    for(;;) {
        printf("Insira um valor para o %s em bytes (potencia de dois): ", configValueStr);
        scanf(" %d", &configValue);
        if(!isSizePowerOfTwo(configValue)) {
            printf("\nTamanho invalido. ");
        } else if (configValue > physicalMemorySize) {
            printf("\nO tamanho das paginas deve ser menor ou igual ao tamanho da memória fisica. ");
        } else {
            return configValue;
        }
    }
}

int getPIDValueFromUser(process_t *processListHead) {
    int PIDInput;
    for(;;) {
        printf("Insira um PID para o processo: ");
        scanf(" %d", &PIDInput);
        if(isPIDAvailable(processListHead, PIDInput)) {
            break;
        } else {
            printf("\nPID já existe em outro processo. ");
        }
    }

    printf("\n");

    return PIDInput;
}

int getProcessSize(int processMaxSize, int pageSize, int freeFramesNum, int *processValidated) {
    int processSize;
    int numOfPagesNeededForProcess;

    for(;;) {
        printf("Insira um tamanho para o processo em bytes (potencia de dois): ");
        scanf(" %d", &processSize);

        if (processSize <= pageSize) {
            numOfPagesNeededForProcess = 1;
        } else {
            numOfPagesNeededForProcess = processSize / pageSize;
        }

        if(processSize > processMaxSize) {
            printf("\nO tamanho máximo para um processo é %d bytes. Tente novamente. ", processMaxSize);
        } else if(numOfPagesNeededForProcess > freeFramesNum) {
            printf("\nNão há memória disponível para alocar este processo. ");
            break;
        } else if(!isSizePowerOfTwo(processSize)) {
            printf("\nTamanho invalido. ");
        } else {
            *processValidated = 1;
            break;
        }
    }

    printf("\n");

    return processSize;
}

int isSizePowerOfTwo(int size) {
    return (((size & (size - 1)) == 0) && size != 0);
}

void zeroAllArray(unsigned char *arr, int arrSize) {
    for(int i=0; i < arrSize; i++) {
        arr[i] = ' ';
    }
}

void printAddressToValue(unsigned char *arr, int arrSize, int frameSize, float freeFramesPercentage) {
    printf("Porcentagem de memoria fisica disponivel: %.2f%%\n", freeFramesPercentage);
    int numOfAddressesPerRow = frameSize;
    for(int i=0; i < arrSize; i++) {
        if((i) % numOfAddressesPerRow == 0) {
            int frameNum = (i / numOfAddressesPerRow) + 1;
            frameNum < 10 ? printf("Quadro nº 0%d -> ", frameNum) : printf("Quadro nº %d -> ", frameNum);
        }
        i < 10 ? printf("| 0%d: %c ", i, arr[i]) : printf("| %d: %c ", i, arr[i]);
        if((i+1) % numOfAddressesPerRow == 0) {
            printf("|\n");
        }
    }
}

void createProcess(int pid, int processSize, unsigned char *physicalMemory, int pageSize, int totalPages, freeFrame_t **freeFramesHeadPtr, process_t **processListHeadPtr) {
    process_t *process = malloc(sizeof(process_t));

    int numPages;

    if(processSize <= pageSize) {
        numPages = 1;
    } else {
        numPages = processSize / pageSize;
    }

    process->pid = pid;
    process->size = processSize;
    process->memoryArr = malloc(processSize);
    process->pageTable = malloc(sizeof(pageTableItem_t) * numPages) ;

    insertProcessIntoList(process, processListHeadPtr);
    fillAllArray(process->memoryArr, processSize, pid);
    
    int pageTableIndex = 0;
    int pageNum = 1;
    int frameNum = 1;
    
    int pagesRemaining = numPages;
    while(pagesRemaining > 0) {
        int maxLimitRandom = 2;
        int randomNum = rand() % (maxLimitRandom + 1);
        if(isFrameEmpty(*freeFramesHeadPtr, frameNum) && randomNum == 2) {
            fillFrame(physicalMemory, process->memoryArr, frameNum, pageNum, pageSize);
            removeFrameFromList(freeFramesHeadPtr, frameNum);

            pageTableItem_t *tableRel = malloc(sizeof(pageTableItem_t));
            tableRel->page = pageNum;
            tableRel->frame = frameNum;

            process->pageTable[pageTableIndex] = tableRel;
            
            pageNum++;
            pagesRemaining--;
            pageTableIndex++;
        }

        if(frameNum == totalPages) {
            frameNum = 1;  
        } else {
            frameNum++;
        }
    }
}

void insertProcessIntoList(process_t *process, process_t **processListHeadPtr) {
    process_t *currentProcess = *processListHeadPtr;
    process_t *prevProcess;

    if(!currentProcess) {
        *processListHeadPtr = process;
        return;
    }

    for(;;) {
        prevProcess = currentProcess;
        currentProcess = currentProcess->next;
        if(!currentProcess) {
            break;
        }
    }

    prevProcess->next = process;

}

void fillAllArray(unsigned char *arr, int arrSize, int pid) {
    for(int i=0; i < arrSize; i++) {
        arr[i] = '0' + pid;
    }
}

int isFrameEmpty(freeFrame_t *headPtr, int frameNum) {
    freeFrame_t *freeFrame = headPtr;

    if(headPtr) {
        for(;;) {
            if(freeFrame->frameId == frameNum) {
                return 1;
            } else if (freeFrame->next) {
                freeFrame = freeFrame->next;
            } else {
                break;
            }
        }
    }

    return 0;
}

void fillFrame(unsigned char *physicalMemory, unsigned char *virtualMemory, int frameNum, int pageNum, int pageSize) {
    for(int i=0; i < pageSize; i++) {
        char valueAtVirtualMemory = virtualMemory[(pageNum - 1) * pageSize + i];
        if(valueAtVirtualMemory != 0) {
            physicalMemory[(frameNum - 1) * pageSize + i] = virtualMemory[(pageNum - 1) * pageSize + i];
        }
    }
}

void removeFrameFromList(freeFrame_t **headPtr, int frameNum) {
    freeFrame_t *freeFrame = *headPtr;

    if(freeFrame->frameId == frameNum && freeFrame->next) {
        *headPtr = freeFrame->next;
        return;
        
    } else if(freeFrame->frameId == frameNum && !freeFrame->next) {
        *headPtr = NULL;
        return;
    }

    freeFrame_t *nextFreeFrame = freeFrame->next;

    for(;;) {
        if(nextFreeFrame->frameId == frameNum && nextFreeFrame->next) {
            freeFrame->next = nextFreeFrame->next;
            break;
        } else if(nextFreeFrame->frameId == frameNum) {
            freeFrame->next = NULL;
            break;
        } else {
            freeFrame = nextFreeFrame;
            nextFreeFrame = nextFreeFrame->next;
        }
    }   
}

void printPageTable(process_t *processListHead, int pidInput, int pageSize) {
    process_t *process = processListHead;

    if(processListHead) {
        for(;;) {
            if(process->pid == pidInput) {
                break;
            }

            if(!process->next) {
                printf("PID nao encontrado.");
                return;
            }

            process = process->next;
        }
        
        int numPages;

        if(process->size <= pageSize) {
            numPages = 1;
        } else {
            numPages = process->size / pageSize;
        }

        printf("Processo %d (%d bytes).\n", pidInput, process->size);

        printf("===========\n");
        printf("| P  | Q  |\n");
        for(int i=0; i < numPages; i++) {
            int page = process->pageTable[i]->page;
            int frame = process->pageTable[i]->frame;

            page < 10 ? printf("| 0%d ", page) : printf("| %d ", page);
            frame < 10 ? printf("| 0%d |\n", frame) : printf("| %d |\n", frame);
        }
        printf("===========\n");

    } else {
        printf("Nenhum processo iniciado.");
        return;
    }

    printf("\n");
}

int isPIDAvailable(process_t *processListHead, int pid) {
    process_t *process = processListHead;

    if(processListHead) {
        for(;;) {
            if(process->pid == pid) {
                return 0;
            } else if(!process->next) {
                break;
            } else {
                process = process->next;
            }
        }
    } 

    return 1;
}

freeFrame_t *insertAllFreeFramesInList(int frameNum) {
    freeFrame_t *head;

    freeFrame_t *newFramePtr = malloc(sizeof(freeFrame_t));
    newFramePtr->frameId = 1;

    head = newFramePtr;
    freeFrame_t *prevFrame = newFramePtr;

    for(int i=2; i < frameNum + 1;i++) {
        freeFrame_t *newFramePtr = malloc(sizeof(freeFrame_t));
        newFramePtr->frameId = i;
        prevFrame->next = newFramePtr;
        prevFrame = newFramePtr;
    }

    prevFrame->next = NULL;

    return head;
}

int getFreeFramesNum(freeFrame_t *headPtr) {
    freeFrame_t *freeFrame = headPtr;

    int counter = 0;

    if(headPtr) {
        for(;;) {
            counter++;
            freeFrame = freeFrame->next;
            if(!freeFrame) {
                break;
            }
        }
    }

    return counter;
}
