CC = gcc
CFLAGS = -Wall
DEPS = memoria.h processo.h
OBJ = main.o memoria.o processo.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

projeto_gerenciamento_memoria: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o projeto_gerenciamento_memoria
