// USO: gcc src/*.c q2/matmul_par.c -Iinclude -lpthread -O2 -o matmul_par
// ./matmul_par

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#endif

#include "../include/matriz.h"
#include "../include/matriz_threads.h"
#include "../include/tempo.h"

void criar_pasta_data() {
#ifdef _WIN32
    _mkdir("data");
#else
    mkdir("data", 0777);
#endif
}

int main() {

    // Criar pasta data/
    criar_pasta_data();

    FILE *arquivo = fopen("/data/resultados_par.csv", "w");
    if (!arquivo) {
        printf("Erro ao criar CSV!\n");
        return 1;
    }

    // Cabeçalho do CSV
    fprintf(arquivo, "n,threads,tempo\n");

    // Tamanhos de matrizes que serão testados
    int tamanhos[] = {100, 200, 300, 400, 500, 600, 700, 800};
    int qtd_tam = sizeof(tamanhos) / sizeof(tamanhos[0]);

    // Quantidades de threads
    int threads_testar[] = {2, 4, 6, 8};
    int qtd_thr = sizeof(threads_testar) / sizeof(threads_testar[0]);

    printf("Executando versão paralela...\n");
    printf("Gerando data/resultados_par.csv\n\n");

    for (int tt = 0; tt < qtd_thr; tt++) {

        int num_threads = threads_testar[tt];

        for (int t = 0; t < qtd_tam; t++) {

            int n = tamanhos[t];

            double **A = alocar_matriz(n);
            double **B = alocar_matriz(n);
            double **C = alocar_matriz(n);

            preencher_matriz_aleatoria(A, n);
            preencher_matriz_aleatoria(B, n);
            zerar_matriz(C, n);

            pthread_t threads[num_threads];
            DadosThread dados[num_threads];

            int bloco = n / num_threads;

            double inicio = tempo_atual();

            // Criar threads
            for (int i = 0; i < num_threads; i++) {

                dados[i].A = A;
                dados[i].B = B;
                dados[i].C = C;
                dados[i].n = n;

                dados[i].linha_inicio = i * bloco;
                dados[i].linha_fim =
                    (i == num_threads - 1) ? n : (i + 1) * bloco;

                pthread_create(&threads[i], NULL, multiplicar_parcial, &dados[i]);
            }

            // Esperar threads terminarem
            for (int i = 0; i < num_threads; i++) {
                pthread_join(threads[i], NULL);
            }

            double fim = tempo_atual();
            double tempo = fim - inicio;

            printf("n=%d threads=%d tempo=%.6f\n", n, num_threads, tempo);
            fprintf(arquivo, "%d,%d,%.6f\n", n, num_threads, tempo);

            liberar_matriz(A, n);
            liberar_matriz(B, n);
            liberar_matriz(C, n);
        }
    }

    fclose(arquivo);

    printf("\nArquivo salvo em: data/resultados_par.csv\n");

    return 0;
}
