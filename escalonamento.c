#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MaximoDeTarefas 100;

typedef struct {
    char nome[50];
    int periodo;
    int deadline;
    int burst;
    int restante;
    int prazo;
} tarefa;


int main(int argc, char *argv[]){
    if (argc != 3){
        fprintf(stderr, "Uso: %s <rate|edf> <arquivo>", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "rate") != 0 && strcmp(argv[1], "edf") != 0){
        fprintf(stderr, "Algoritmo inválido.\n");
        return 1;
    }

    return 0;
}