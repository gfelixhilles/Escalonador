#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

// Premissas definidas para o funcionamento do simulador
#define QUANTUM 10           // Fatia de tempo dada aos processos na CPU
#define TIPO_DISCO 1
#define TIPO_FITA 2
#define TIPO_IMP 3

// Estados possíveis de um processo
typedef enum { 
    PRONTO, 
    EXECUTANDO, 
    BLOQUEADO, 
    FINALIZADO 
} StatusProcesso;

// Estrutura do Process Control Block (PCB)
typedef struct PCB {
    int pid;                // Identificador do processo
    int ppid;               // Identificador do processo pai
    int prioridade;         // 0 = Alta, 1 = Baixa
    StatusProcesso status;  // Estado atual
    int tempo_chegada;      // Tempo em que entrou no sistema
    int tempo_restante;     // Tempo de serviço na CPU
    int tempo_io_restante;  // Tempo de bloqueio no dispositivo
    int tipo_io_atual;      // Dispositivo atual (Disco, Fita ou Impressora)
} PCB;

// Estrutura do Nó para a lista encadeada
typedef struct No {
    PCB *processo;
    struct No *proximo;
} No;

// Estrutura da Fila
typedef struct {
    No *inicio;
    No *fim;
} Fila;

// Protótipos das funções de manipulação de Fila
Fila* criar_fila();
void inserir_fila(Fila *f, PCB *novo_processo);
PCB* remover_fila(Fila *f);
int fila_vazia(Fila *f);

// Protótipos das funções do Escalonador e Aleatoriedade
void inicializar_aleatoriedade();
int solicitou_io(PCB *processo);
int sortear_tempo_io(int tipo_dispositivo);
void executar_escalonador(Fila *fila_alta, Fila *fila_baixa, Fila *fila_disco, Fila *fila_fita, Fila *fila_impressora);

#endif // ESTRUTURAS_H
