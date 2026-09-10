# Scheduler

- O Scheduler é um programa desenvolvido em C.

- Serve para simular o escalonamento de tarefas periódicas.

- Utiliza os algoritmos Rate-Monotonic e Earliest Deadline First.

- O programa lê um arquivo com o tempo total da simulação e as informaçÕes de cada tarefa.

- Durante a execução, o programa controla as tarefas que foram concluídas, as que perderam o deadline e as que foram encerradas no fim da simulação.

## Compilação

```bash
make clean
make