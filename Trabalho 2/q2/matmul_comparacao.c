// Uso: gcc src/*.c q2/matmul_comparacao.c -Iinclude -lpthread -O2 -o matmul_comparacao
// ./matmul_comparacao

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/stat.h>

#include "../include/matriz.h"
#include "../include/matriz_threads.h"
#include "../include/tempo.h"

#ifdef _WIN32
#include <direct.h>
#endif

void criar_pasta_data() {
#ifdef _WIN32
    _mkdir("data");
#else
    mkdir("data", 0777);
#endif
}

// Função para executar versão paralela
double executar_paralelo(double **A, double **B, double **C, int n, int num_threads) {

    pthread_t threads[num_threads];
    DadosThread dados[num_threads];

    int bloco = n / num_threads;

    double inicio = tempo_atual();

    for (int t = 0; t < num_threads; t++) {

        dados[t].A = A;
        dados[t].B = B;
        dados[t].C = C;
        dados[t].n = n;

        dados[t].linha_inicio = t * bloco;
        dados[t].linha_fim = (t == num_threads - 1 ? n : (t + 1) * bloco);

        pthread_create(&threads[t], NULL, multiplicar_parcial, &dados[t]);
    }

    for (int t = 0; t < num_threads; t++) {
        pthread_join(threads[t], NULL);
    }

    return tempo_atual() - inicio;
}

int main() {
    criar_pasta_data();

    FILE *arquivo = fopen("/data/resultados_speedup.csv", "w");
    if (!arquivo) {
        printf("Erro ao criar CSV!\n");
        return 1;
    }

    fprintf(arquivo, "n,threads,tempo_seq,tempo_par,speedup\n");

    int tamanhos[] = {200, 300, 400, 500, 600, 700, 800};
    int qtd_tam = sizeof(tamanhos) / sizeof(tamanhos[0]);

    int threads_list[] = {2, 4, 6, 8};
    int qtd_thr = sizeof(threads_list) / sizeof(threads_list[0]);

    printf("\n====== COMPARAÇÃO SEQ vs PAR ======\n");

    for (int ti = 0; ti < qtd_tam; ti++) {

        int n = tamanhos[ti];

        double **A = alocar_matriz(n);
        double **B = alocar_matriz(n);

        preencher_matriz_aleatoria(A, n);
        preencher_matriz_aleatoria(B, n);

        // ------ TEMPO SEQUENCIAL ------
        double **C_seq = alocar_matriz(n);
        zerar_matriz(C_seq, n);

        double inicio = tempo_atual();
        multiplicar_matrizes_sequencial(A, B, C_seq, n);
        double tempo_seq = tempo_atual() - inicio;

        printf("\nTamanho n = %d\nTempo sequencial = %.6f s\n", n, tempo_seq);

        liberar_matriz(C_seq, n);

        // ------ TESTE PARALELO ------
        for (int th = 0; th < qtd_thr; th++) {

            int num_threads = threads_list[th];

            double **C_par = alocar_matriz(n);
            zerar_matriz(C_par, n);

            double tempo_par = executar_paralelo(A, B, C_par, n, num_threads);

            double speedup = tempo_seq / tempo_par;

            printf("Threads=%d  Tempo_par=%.6f  Speedup=%.2f\n",
                   num_threads, tempo_par, speedup);

            fprintf(arquivo, "%d,%d,%.6f,%.6f,%.6f\n",
                    n, num_threads, tempo_seq, tempo_par, speedup);

            liberar_matriz(C_par, n);
        }

        liberar_matriz(A, n);
        liberar_matriz(B, n);
    }

    fclose(arquivo);

    printf("\nResultados gravados em data/resultados_speedup.csv\n");

    return 0;
}
