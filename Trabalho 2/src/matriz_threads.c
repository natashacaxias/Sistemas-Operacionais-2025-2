#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "../include/matriz_threads.h"

void *multiplicar_parcial(void *args) {
    DadosThread *dados = (DadosThread *)args;

    double **A = dados->A;
    double **B = dados->B;
    double **C = dados->C;
    int n = dados->n;

    for (int i = dados->linha_inicio; i < dados->linha_fim; i++) {
        for (int j = 0; j < n; j++) {
            double soma = 0.0;
            for (int k = 0; k < n; k++) {
                soma += A[i][k] * B[k][j];
            }
            C[i][j] = soma;
        }
    }

    pthread_exit(NULL);
}
