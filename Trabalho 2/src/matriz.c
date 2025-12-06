#include <stdio.h>
#include <stdlib.h>
#include "matriz.h"

double **alocar_matriz(int n) {
    double **m = malloc(n * sizeof(double *));
    if (!m) {
        printf("Erro ao alocar matriz!\n");
        exit(1);
    }

    for (int i = 0; i < n; i++) {
        m[i] = malloc(n * sizeof(double));
        if (!m[i]) {
            printf("Erro ao alocar linha da matriz!\n");
            exit(1);
        }
    }
    return m;
}

void liberar_matriz(double **m, int n) {
    for (int i = 0; i < n; i++)
        free(m[i]);
    free(m);
}

void preencher_matriz_aleatoria(double **m, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            m[i][j] = rand() % 10;
}

void zerar_matriz(double **m, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            m[i][j] = 0.0;
}

void imprimir_matriz(double **m, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            printf("%.2f ", m[i][j]);
        printf("\n");
    }
}

void multiplicar_matrizes_sequencial(double **A, double **B, double **C, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            for (int k = 0; k < n; k++)
                C[i][j] += A[i][k] * B[k][j];
}
