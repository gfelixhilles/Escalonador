#include <stdio.h>
#include <stdlib.h>
#include "estruturas.h"

Fila* criar_fila() {
    Fila *f = (Fila*)malloc(sizeof(Fila));
    if (f != NULL) {
        f->inicio = NULL;
        f->fim = NULL;
    }
    return f;
}

void inserir_fila(Fila *f, PCB *novo_processo) {
    No *novo_no = (No*)malloc(sizeof(No));
    if (novo_no == NULL) {
        printf("Erro de alocação de memória!\n");
        return;
    }
    novo_no->processo = novo_processo;
    novo_no->proximo = NULL;

    if (f->inicio == NULL) {
        f->inicio = novo_no;
        f->fim = novo_no;
    } else {
        f->fim->proximo = novo_no;
        f->fim = novo_no;
    }
}

PCB* remover_fila(Fila *f) {
    if (f->inicio == NULL) {
        return NULL;
    }
    
    No *no_removido = f->inicio;
    PCB *processo_removido = no_removido->processo;
    
    f->inicio = no_removido->proximo;
    if (f->inicio == NULL) {
        f->fim = NULL;
    }
    
    free(no_removido);
    return processo_removido;
}

int fila_vazia(Fila *f) {
    return (f->inicio == NULL);
}
