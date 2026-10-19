#include <stdio.h>
#include <stdlib.h>
#include "estruturas.h"

void gerar_processos_teste(Fila *fila_alta, int quantidade) {
    for (int i = 1; i <= quantidade; i++) {
        PCB *novo_processo = (PCB*)malloc(sizeof(PCB));
        
        novo_processo->pid = i;
        novo_processo->ppid = 1;
        novo_processo->status = PRONTO;
        novo_processo->prioridade = 0; // Novos processos iniciam na alta prioridade
        novo_processo->tempo_chegada = 0;
        
        // Tempo de serviço aleatório entre 20 e 100 ciclos
        novo_processo->tempo_restante = (rand() % 81) + 20; 
        
        novo_processo->tempo_io_restante = 0;
        novo_processo->tipo_io_atual = 0;

        inserir_fila(fila_alta, novo_processo);
        
        printf("Processo PID %d criado. (Tempo de Servico: %d ciclos)\n", 
               novo_processo->pid, novo_processo->tempo_restante);
    }
}

int main() {
    inicializar_aleatoriedade();

    // 1. Criar as 5 filas necessárias
    Fila *fila_alta = criar_fila();
    Fila *fila_baixa = criar_fila();
    Fila *fila_disco = criar_fila();
    Fila *fila_fita = criar_fila();
    Fila *fila_impressora = criar_fila();

    // 2. Definir o limite máximo de processos
    int limite_maximo_processos = 5; 
    
    printf("=== A INICIAR A CRIACAO DE PROCESSOS ===\n");
    gerar_processos_teste(fila_alta, limite_maximo_processos);
    printf("========================================\n\n");

    // 3. Executar o Escalonador
    printf("A iniciar Simulador Round Robin com Feedback...\n");
    executar_escalonador(fila_alta, fila_baixa, fila_disco, fila_fita, fila_impressora);

    // Boa prática: libertar a memória das filas no fim (não estritamente necessário num simulador simples, mas recomendado em C)
    free(fila_alta); free(fila_baixa); 
    free(fila_disco); free(fila_fita); free(fila_impressora);

    return 0;
}
