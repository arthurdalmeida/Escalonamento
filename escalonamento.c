#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MaximoDeTarefas 1000

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
        fprintf(stderr, "Nao foi possivel abrir o arquivo: %s\n", caminho);
    }

    return arquivo;
}

int lerArquivo(FILE *arquivo, int *tempoTotal, tarefa tarefas[], int *qtdTarefas){
    if (fscanf(arquivo, "%d", tempoTotal) != 1){
        fprintf(stderr, "Nao foi possivel ler o tempo total.\n");
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
            fprintf(stderr, "Faltam informacoes da tarefa.\n");
            return 0;
        }

        if (*qtdTarefas >= MaximoDeTarefas){
            fprintf(stderr, "Quantidade de tarefas invalida.\n");
            return 0;
        }

        if (atual.periodo <= 0 || atual.deadline <= 0 || atual.burst <= 0){
            fprintf(stderr, "Os valores da tarefa %s sao invalidos\n", atual.nome);
            return 0;
        }

        if (atual.deadline > atual.periodo){
            fprintf(stderr, "O deadline da tarefa %s é invalido\n", atual.nome);
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

void executarEscalonamento(tarefa tarefas[], int qtdTarefas, int tempoTotal, const char *escalonador, int historico[], char motivo[]){
    for (int tempo=0; tempo < tempoTotal; tempo++){
        historico[tempo] = -1;
        motivo[tempo] = '\0';

        for (int i=0; i<qtdTarefas; i++){
            if (tarefas[i].restante > 0 && tarefas[i].prazo == tempo){
                tarefas[i].perdidas++;

                if (tempo > 0 && historico[tempo-1] == i){
                    motivo[tempo-1] = 'L';
                }
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

        historico[tempo] = escolhida;

        if (escolhida != -1){
            tarefas[escolhida].restante--;

            if (tarefas[escolhida].restante == 0){
                tarefas[escolhida].completas++;
                motivo[tempo] = 'F';
            }
        }
    }

    for (int i=0; i<qtdTarefas; i++){
        if (tarefas[i].restante > 0){
            if (tarefas[i].prazo <= tempoTotal){
                tarefas[i].perdidas++;

                if (tempoTotal > 0 && historico[tempoTotal-1] == i){
                    motivo[tempoTotal-1] = 'L';
                }
            }

            else{
                tarefas[i].mortas++;
            }
            tarefas[i].restante = 0;
        }
    }
}

FILE *criarArquivoSaida(char *escalonador){
    char nomeArquivo[100];

    snprintf(nomeArquivo, sizeof(nomeArquivo), "%s_aao.out", escalonador);

    FILE *saida = fopen(nomeArquivo, "w");

    if (saida == NULL){
        fprintf(stderr, "Nao foi possivel criar o arquivo de saida\n");
    }

    return saida;
}

void escreverSaida(FILE* saida, char *escalonador, tarefa tarefas[], int qtdTarefas, int historico[], char motivo[], int tempoTotal){
    if (strcmp(escalonador, "rate") == 0){
        fprintf(saida, "EXECUTION BY RATE\n");
    }

    else{
        fprintf(saida, "EXECUTION BY EDF\n");
    }

    int inicio = 0;

    while (inicio <tempoTotal){
        int executada = historico[inicio];
        int fim = inicio;

        if (executada == -1){
            while (fim + 1 < tempoTotal && historico[fim +1] == -1){
                fim++;
            }

            fprintf(saida, "idle for %d units\n", fim - inicio + 1);
        }

        else{
            while(fim + 1 <tempoTotal && historico[fim + 1] == executada && motivo[fim] == '\0'){
                fim++;
            }

            char estado = motivo[fim];

            if (estado == '\0'){
                estado = 'H';
            }

            fprintf(saida, "[%s] for %d units - %c\n", tarefas[executada].nome, fim - inicio + 1, estado);
        }

        inicio = fim + 1;
    }

    fprintf(saida, "LOST DEADLINES\n");

    for (int i=0; i< qtdTarefas; i++){
        fprintf(saida, "[%s] %d\n", tarefas[i].nome, tarefas[i].perdidas);
    }

    fprintf(saida, "COMPLETE EXECUTION\n");

    for (int i=0; i< qtdTarefas; i++){
        fprintf(saida, "[%s] %d\n", tarefas[i].nome, tarefas[i].completas);
    }

    fprintf(saida, "KILLED\n");

    for (int i=0; i< qtdTarefas; i++){
        fprintf(saida, "[%s] %d\n", tarefas[i].nome, tarefas[i].mortas);
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
    
    int *historico = malloc(tempoTotal * sizeof(int));

    char *motivo = calloc(tempoTotal, sizeof(char));

    if (historico == NULL || motivo == NULL){
        fprintf(stderr, "Nao foi possivel reservar memoria.\n");
        free(historico);
        free(motivo);
        return 1;
    }

    executarEscalonamento(tarefas, qtdTarefas, tempoTotal, argv[1], historico, motivo);

    FILE* saida = criarArquivoSaida(argv[1]);

    if (saida == NULL){
        free(historico);
        free(motivo);
        return 1;
    }

    escreverSaida(saida, argv[1], tarefas, qtdTarefas, historico, motivo, tempoTotal);

    fclose(saida);

    free(historico);
    free(motivo);

    return 0;
}