#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MaximoDeTarefas 100

typedef struct {
    char nome[50];
    int periodo;
    int deadline;
    int burst;
    int restante;
    int prazo;
} tarefa;

FILE* abrirArquivo(const char *caminho){
    FILE *arquivo = fopen(caminho, "r");

    if (arquivo == NULL){
        fprintf(stderr, "Não foi possível abrir o arquivo: %s\n", caminho);
    }

    return arquivo;
}

int lerArquivo(FILE *arquivo, int *tempoTotal){
    if (fscanf(arquivo, "%d", tempoTotal) != 1){
        fprintf(stderr, "Não foi possível ler o tempo total.\n");
        return 0;
    }

    if (*tempoTotal <= 0){
        fprintf(stderr, "O tempo total deve ser maior que zero.\n");
        return 0;
    }

    return 1;
}


int main(int argc, char *argv[]){
    if (argc != 3){
        fprintf(stderr, "Uso: %s <rate|edf> <arquivo>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "rate") != 0 && strcmp(argv[1], "edf") != 0){
        fprintf(stderr, "Algoritmo inválido.\n");
        return 1;
    }

    FILE *arquivo = abrirArquivo(argv[2]);

    if (arquivo == NULL){
        return 1;
    }

    int tempoTotal;

    if (lerArquivo(arquivo, &tempoTotal) == 0){
        fclose(arquivo);
        return 1;
    }

    fclose(arquivo);

    return 0;
}