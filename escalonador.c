#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "estruturas.h"

void inicializar_aleatoriedade() {
    srand(time(NULL)); 
}

int solicitou_io(PCB *processo) {
    int chance_io = 10; // 10% de probabilidade a cada ciclo de CPU
    int sorteio = (rand() % 100) + 1;
    if (sorteio <= chance_io) {
        processo->tipo_io_atual = (rand() % 3) + 1; // 1=Disco, 2=Fita, 3=Impressora
        return 1;
    }
    return 0;
}

int sortear_tempo_io(int tipo_dispositivo) {
    // Retorna a duração aleatória conforme o dispositivo sorteado
    switch (tipo_dispositivo) {
        case TIPO_DISCO: return (rand() % 11) + 5;   // 5 a 15 ciclos
        case TIPO_FITA:  return (rand() % 21) + 30;  // 30 a 50 ciclos
        case TIPO_IMP:   return (rand() % 16) + 10;  // 10 a 25 ciclos
        default:         return 10; 
    }
}

// Função auxiliar interna para gerir o avanço de tempo nos dispositivos de I/O
void atualizar_filas_io(Fila *fila_alta, Fila *fila_baixa, Fila *fila_disco, Fila *fila_fita, Fila *fila_impressora, int tempo_global) {
    // I/O de Disco (Retorna para Baixa Prioridade)
    if (!fila_vazia(fila_disco)) {
        PCB *p = fila_disco->inicio->processo;
        p->tempo_io_restante--;
        if (p->tempo_io_restante <= 0) {
            p = remover_fila(fila_disco);
            p->status = PRONTO;
            p->prioridade = 1; // Baixa prioridade
            inserir_fila(fila_baixa, p);
            printf("Tempo %d: PID %d terminou I/O (Disco) -> Foi para Fila Baixa.\n", tempo_global, p->pid);
        }
    }

    // I/O de Fita Magnética (Retorna para Alta Prioridade)
    if (!fila_vazia(fila_fita)) {
        PCB *p = fila_fita->inicio->processo;
        p->tempo_io_restante--;
        if (p->tempo_io_restante <= 0) {
            p = remover_fila(fila_fita);
            p->status = PRONTO;
            p->prioridade = 0; // Alta prioridade
            inserir_fila(fila_alta, p);
            printf("Tempo %d: PID %d terminou I/O (Fita) -> Foi para Fila Alta.\n", tempo_global, p->pid);
        }
    }

    // I/O de Impressora (Retorna para Alta Prioridade)
    if (!fila_vazia(fila_impressora)) {
        PCB *p = fila_impressora->inicio->processo;
        p->tempo_io_restante--;
        if (p->tempo_io_restante <= 0) {
            p = remover_fila(fila_impressora);
            p->status = PRONTO;
            p->prioridade = 0; // Alta prioridade
            inserir_fila(fila_alta, p);
            printf("Tempo %d: PID %d terminou I/O (Impressora) -> Foi para Fila Alta.\n", tempo_global, p->pid);
        }
    }
}

void executar_escalonador(Fila *fila_alta, Fila *fila_baixa, Fila *fila_disco, Fila *fila_fita, Fila *fila_impressora) {
    int tempo_global = 0;
    PCB *processo_atual = NULL;
    int tempo_gasto_quantum = 0;

    while (!fila_vazia(fila_alta) || !fila_vazia(fila_baixa) || 
           !fila_vazia(fila_disco) || !fila_vazia(fila_fita) || !fila_vazia(fila_impressora) ||
           processo_atual != NULL) {

        // 1. Escolher o próximo processo se a CPU estiver livre
        if (processo_atual == NULL) {
            if (!fila_vazia(fila_alta)) {
                processo_atual = remover_fila(fila_alta);
                processo_atual->status = EXECUTANDO;
            } else if (!fila_vazia(fila_baixa)) {
                processo_atual = remover_fila(fila_baixa);
                processo_atual->status = EXECUTANDO;
            }
            tempo_gasto_quantum = 0; // Reinicia o cronómetro do quantum
        }

        // 2. Executar processo na CPU por 1 ciclo
        if (processo_atual != NULL) {
            processo_atual->tempo_restante--;
            tempo_gasto_quantum++;
        }

        // 3. Atualizar processos bloqueados em I/O
        atualizar_filas_io(fila_alta, fila_baixa, fila_disco, fila_fita, fila_impressora, tempo_global);

        // 4. Analisar os eventos do processo atual
        if (processo_atual != NULL) {
            // A) Processo concluído
            if (processo_atual->tempo_restante <= 0) {
                processo_atual->status = FINALIZADO;
                printf("Tempo %d: PID %d FINALIZOU a sua execucao.\n", tempo_global, processo_atual->pid);
                free(processo_atual);
                processo_atual = NULL;
            }
            // B) Processo solicita I/O
            else if (solicitou_io(processo_atual)) {
                processo_atual->status = BLOQUEADO;
                processo_atual->tempo_io_restante = sortear_tempo_io(processo_atual->tipo_io_atual);
                
                if (processo_atual->tipo_io_atual == TIPO_DISCO) {
                    inserir_fila(fila_disco, processo_atual);
                    printf("Tempo %d: PID %d foi para I/O de Disco.\n", tempo_global, processo_atual->pid);
                } else if (processo_atual->tipo_io_atual == TIPO_FITA) {
                    inserir_fila(fila_fita, processo_atual);
                    printf("Tempo %d: PID %d foi para I/O de Fita Magnetica.\n", tempo_global, processo_atual->pid);
                } else {
                    inserir_fila(fila_impressora, processo_atual);
                    printf("Tempo %d: PID %d foi para I/O de Impressora.\n", tempo_global, processo_atual->pid);
                }
                processo_atual = NULL; // Liberta a CPU
            }
            // C) Preempção por esgotamento do Quantum
            else if (tempo_gasto_quantum >= QUANTUM) {
                processo_atual->status = PRONTO;
                processo_atual->prioridade = 1; 
                inserir_fila(fila_baixa, processo_atual);
                printf("Tempo %d: PID %d sofreu preempcao (Quantum esgotado) -> Foi para Fila Baixa.\n", tempo_global, processo_atual->pid);
                processo_atual = NULL; // Liberta a CPU
            }
        }

        // 5. Avançar relógio global
        tempo_global++;
    }
    printf("\nSimulacao concluida no tempo %d ciclos.\n", tempo_global);
}
