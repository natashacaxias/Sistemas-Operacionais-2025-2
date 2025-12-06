#ifndef MATRIZ_H
#define MATRIZ_H

// Aloca matriz n x n
double **alocar_matriz(int n);

// Libera matriz
void liberar_matriz(double **m, int n);

// Preenche matriz com valores aleatórios
void preencher_matriz_aleatoria(double **m, int n);

// Zera matriz
void zerar_matriz(double **m, int n);

// Impressão (debug)
void imprimir_matriz(double **m, int n);

// Multiplicação sequencial
void multiplicar_matrizes_sequencial(double **A, double **B, double **C, int n);

#endif
