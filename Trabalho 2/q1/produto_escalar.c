// USO: gcc src/*.c q1/produto_escalar.c -Iinclude -lpthread -O2 -o produto_escalar
// ./produto_escalar

#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

const int MAX = 10000000;

typedef struct {
    int *vet1;
    int *vet2;
    int st;
    int end;
    long long res_parc;
} DadosThread;


// Função que cada thread executará
void *produto_escalar_parcial(void *args) {
    DadosThread *dados = (DadosThread *)args;

    int *vet1 = dados->vet1;
    int *vet2 = dados->vet2;
    int st = dados->st;
    int end = dados->end;
    dados->res_parc = 0;

    long long sum = 0;
    for (int i = st; i < end; i++) {
        sum += vet1[i] * vet2[i];
    }

    dados->res_parc = sum;  // Atualiza resposta parcial via referência

    pthread_exit(NULL);
}

int* cria_vetor_aleatorio(int tam){
    int* v = (int*)malloc(tam * sizeof(int));
    for(int i = 0; i < tam; i++) v[i] = rand() % 1000;
    return v;
}

double produto_escalar_sequencial(int* vet1, int* vet2, int tam, FILE *file){

    struct timespec start, end;
    double cpu_time_used;
    long long pe = 0;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < tam; i++) {
        pe += vet1[i] * vet2[i];
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    cpu_time_used = (end.tv_sec - start.tv_sec) +
                    (end.tv_nsec - start.tv_nsec) / 1e9;

    if (file != NULL) {
        printf("Produto Escalar Sequencial: %lld\n", pe);
        fprintf(file, "%d,%f\n", tam, cpu_time_used);
    }

    return cpu_time_used;
}

double produto_escalar_paralelo(int* vet1, int* vet2, int qtd_t, FILE *file, FILE *file_speeedup){

    struct timespec start, end;
    double cpu_time_used;
    long long pe = 0;

    for(int tam = 10000; tam <= 10000000; tam+=1000000){
        DadosThread *threadsInfo = (DadosThread*)malloc(qtd_t * sizeof(DadosThread));
        pthread_t *threads = (pthread_t*)malloc(qtd_t * sizeof(pthread_t));
        if (!threadsInfo || !threads) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }

        pe = 0;
        int intervalo = tam / qtd_t; // tamanho base do chunk

        clock_gettime(CLOCK_MONOTONIC, &start);

        // Cria exatamente threads_count threads
        for (int t = 0; t < qtd_t; t++) {
            threadsInfo[t].vet1 = vet1;
            threadsInfo[t].vet2 = vet2;
            threadsInfo[t].st = t * intervalo;
            // último thread pega o restante
            threadsInfo[t].end = (t == qtd_t - 1) ? tam : (t + 1) * intervalo;
            threadsInfo[t].res_parc = 0;

            if (pthread_create(&threads[t], NULL, produto_escalar_parcial, &threadsInfo[t]) != 0) {
                perror("pthread_create");
                exit(EXIT_FAILURE);
            }
        }

        // Esperar threads terminarem e somar parciais
        for (int t = 0; t < qtd_t; t++) {
            if (pthread_join(threads[t], NULL) != 0) {
                perror("pthread_join");
                exit(EXIT_FAILURE);
            }
            pe += threadsInfo[t].res_parc;
        }

        clock_gettime(CLOCK_MONOTONIC, &end);

        printf("Produto Escalar Paralelo (%d threads): %lld\n", qtd_t, pe);

        cpu_time_used = (end.tv_sec - start.tv_sec) +
                        (end.tv_nsec - start.tv_nsec) / 1e9;

        if (file != NULL) {
            fprintf(file, "%d,%d,%f\n", tam, qtd_t, cpu_time_used);
        }

        if(file_speeedup != NULL){
            double ts = produto_escalar_sequencial(vet1, vet2, tam, NULL);
            fprintf(file_speeedup, "%d,%d,%f,%f,%f\n", tam, qtd_t, ts, cpu_time_used, ts / cpu_time_used);
        }

        free(threadsInfo);
        free(threads);
    }
    printf("\n");
}

void comparacao(){

}

int main() {
    
    FILE *file_seq = fopen("q1/data/resultados_seq.csv", "w");
    FILE *file_par = fopen("q1/data/resultados_par.csv", "w");
    FILE *file_speedup = fopen("q1/data/resultados_speedup.csv", "w");
    

    fprintf(file_par, "n,threads,tempo\n");
    fprintf(file_seq, "n,tempo\n");
    fprintf(file_speedup, "n,threads,tempo_seq,tempo_par,speedup\n");

    int* vet1 = cria_vetor_aleatorio(MAX);
    int* vet2 = cria_vetor_aleatorio(MAX);

    for(int tam = 10000; tam <= 10000000; tam+=1000000){
        produto_escalar_sequencial(vet1, vet2, tam, file_seq);
    }
    printf("\n");

    // Testa para tamanhos de vetor de 100 a 1000, incrementando de 1000 em 1000
    for(int t = 2; t <= 8; t += 2) {
        
        printf("Quantidade de Threads: %d\n", t);
        produto_escalar_paralelo(vet1, vet2, t, file_par, file_speedup);
    }

    fclose(file_seq);
    fclose(file_par);
    return 0;
}
