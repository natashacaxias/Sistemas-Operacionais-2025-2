#ifndef MATRIZ_THREADS_H
#define MATRIZ_THREADS_H

typedef struct {
    double **A;
    double **B;
    double **C;
    int n;
    int linha_inicio;
    int linha_fim;
} DadosThread;

// Função executada por cada thread
void *multiplicar_parcial(void *args);

#endif
