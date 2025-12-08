// USO: gcc src/*.c q2/matmul_seq.c -Iinclude -O2 -o matmul_seq
// ./matmul_seq

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>   // _mkdir no Windows
#endif

#include "../include/matriz.h"
#include "../include/tempo.h"

void criar_pasta_data() {
#ifdef _WIN32
    _mkdir("q2/data");  // Ignora erro caso já exista
#else
    mkdir("q2/data", 0777); 
#endif
}

int main() {

    // Criar a pasta "data" se não existir
    criar_pasta_data();

    // Abrir arquivo CSV dentro de /data/
    FILE *arquivo = fopen("q2/data/resultados_seq.csv", "w");
    if (!arquivo) {
        printf("Erro ao criar arquivo CSV!\n");
        return 1;
    }

    // Escrever cabeçalho
    fprintf(arquivo, "n,tempo\n");

    // Tamanhos a testar
    int tamanhos[] = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
    int qtd = sizeof(tamanhos) / sizeof(tamanhos[0]);

    printf("Executando multiplicacao sequencial...\n");
    printf("Gerando arquivo: data/resultados_seq.csv\n\n");
    printf("n,tempo\n");

    for (int t = 0; t < qtd; t++) {

        int n = tamanhos[t];

        // Alocar matrizes
        double **A = alocar_matriz(n);
        double **B = alocar_matriz(n);
        double **C = alocar_matriz(n);

        preencher_matriz_aleatoria(A, n);
        preencher_matriz_aleatoria(B, n);
        zerar_matriz(C, n);

        double inicio = tempo_atual();
        multiplicar_matrizes_sequencial(A, B, C, n);
        double fim = tempo_atual();

        double tempo = fim - inicio;

        // Imprimir na tela
        printf("%d,%.6f\n", n, tempo);

        // Registrar no CSV
        fprintf(arquivo, "%d,%.6f\n", n, tempo);

        liberar_matriz(A, n);
        liberar_matriz(B, n);
        liberar_matriz(C, n);
    }

    fclose(arquivo);

    printf("\nArquivo salvo em: data/resultados_seq.csv\n");

    return 0;
}
