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
    int completas;
    int perdidas;
    int mortas;
} tarefa;

FILE* abrirArquivo(const char *caminho){
    FILE *arquivo = fopen(caminho, "r");

    if (arquivo == NULL){
        fprintf(stderr, "Não foi possível abrir o arquivo: %s\n", caminho);
    }

    return arquivo;
}

int lerArquivo(FILE *arquivo, int *tempoTotal, tarefa tarefas[], int *qtdTarefas){
    if (fscanf(arquivo, "%d", tempoTotal) != 1){
        fprintf(stderr, "Não foi possível ler o tempo total.\n");
        return 0;
    }

    if (*tempoTotal <= 0){
        fprintf(stderr, "O tempo total deve ser maior que zero.\n");
        return 0;
    }


    while (1){
        tarefa atual;

        int resultado = fscanf(arquivo, "%49s %d %d %d", atual.nome, &atual.periodo, &atual.deadline, &atual.burst);

        if (resultado == EOF){
            break;
        }

        if (resultado != 4){
            fprintf(stderr, "Faltam infoemações da tarefa.\n");
            return 0;
        }

        if (*qtdTarefas >= MaximoDeTarefas){
            fprintf(stderr, "Quantidade de taredas inválida.\n");
            return 0;
        }

        if (atual.periodo <= 0 || atual.deadline <= 0 || atual.burst <= 0){
            fprintf(stderr, "Os valores da tarefa %s são inválidos\n", atual.nome);
            return 0;
        }

        if (atual.deadline > atual.periodo){
            fprintf(stderr, "O deadline da tareda %s é invalido\n", atual.nome);
            return 0;
        }

        if (atual.burst > atual.deadline){
            fprintf(stderr, "O burst da tarefa %s é invalido\n", atual.nome);
            return 0;
        }

        atual.restante = 0;
        atual.prazo = 0;

        atual.completas = 0;
        atual.perdidas = 0;
        atual.mortas = 0;

        tarefas[*qtdTarefas] = atual;
        (*qtdTarefas)++;
        
    }
    if (*qtdTarefas == 0){
        fprintf(stderr, "O arquivo nao tem tarefas.\n");
        return 0;
    }

    return 1;
}

int escolherRate(tarefa tarefas[], int qtdTarefas){
    int escolhida = -1;

    for (int i=0; i<qtdTarefas; i++){
        if (tarefas[i].restante > 0){
            if (escolhida == -1 || tarefas[i].periodo < tarefas[escolhida].periodo){
                escolhida = i;
            }
        }
    }
    return escolhida;
}

int escolherEdf(tarefa tarefas[], int qtdTarefas){
    int escolhida = -1;

    for (int i=0; i<qtdTarefas; i++){
        if (tarefas[i].restante > 0){
            if (escolhida == -1 || tarefas[i].prazo < tarefas[escolhida].prazo){
                escolhida = i;
            }
        }
    }
    return escolhida;
}

void executarEscalonamento(tarefa tarefas[], int qtdTarefas, int tempoTotal, const char *escalonador){
    for (int tempo=0; tempo < tempoTotal; tempo++){
        for (int i=0; i<qtdTarefas; i++){
            if (tarefas[i].restante > 0 && tarefas[i].prazo == tempo){
                tarefas[i].perdidas++;
                tarefas[i].restante = 0;
            }
        }

        for (int i=0; i< qtdTarefas; i++){
            if (tempo % tarefas[i].periodo == 0){
                tarefas[i].restante = tarefas[i].burst;
                tarefas[i].prazo = tarefas[i].deadline + tempo;
            }
        }

        int escolhida;

        if (strcmp(escalonador, "rate") == 0){
            escolhida = escolherRate(tarefas, qtdTarefas);
        }

        else{
            escolhida = escolherEdf(tarefas, qtdTarefas);
        }

        if (escolhida != -1){
            tarefas[escolhida].restante--;

            if (tarefas[escolhida].restante == 0){
                tarefas[escolhida].completas++;
            }
        }
    }

    for (int i=0; i<qtdTarefas; i++){
        if (tarefas[i].restante > 0){
            if (tarefas[i].prazo <= tempoTotal){
                tarefas[i].perdidas++;
            }

            else{
                tarefas[i].mortas++;
            }
            tarefas[i].restante = 0;
        }
    }
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
    int qtdTarefas = 0;
    tarefa tarefas[MaximoDeTarefas];

    if (lerArquivo(arquivo, &tempoTotal, tarefas, &qtdTarefas) == 0){
        fclose(arquivo);
        return 1;
    }

    fclose(arquivo);

    executarEscalonamento(tarefas, qtdTarefas, tempoTotal, argv[1]);

    return 0;
}