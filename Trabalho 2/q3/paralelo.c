#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>     // sleep
#include <time.h>

#define MAX_NPCS 100
#define LINE_BUF 128

typedef struct {
    int id;
    int is_impostor;
} npc_arg_t;

// Variáveis compartilhadas 
int qtd_mortos = 0;
int qtd_tarefas = 0;
int qtd_trocas = 0;
int jogo_rodando = 1; // 1 = rodando, 0 = encerrar 

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// Salas do jogo 
const char *salas[] = {"Cafeteria", "Elétrica", "Reator", "Admin", "Navegação"};
const int NUM_SALAS = sizeof(salas)/sizeof(salas[0]);

// Função que representa comportamento de cada NPC (thread) 
void *npc_thread(void *arg) {
    npc_arg_t a = *(npc_arg_t *)arg;
    free(arg); // liberamos a struct passada 
    int id = a.id;
    int is_impostor = a.is_impostor;
    const char *nome = (is_impostor ? "IMPOSTOR" : "NPC");

    // semente local opcional para rand_r 
    unsigned int rnd = (unsigned int)time(NULL) ^ (unsigned int)(id*7919);

    while (1) {
        // Verifica se jogo continua 
        pthread_mutex_lock(&lock);
        int running = jogo_rodando;
        pthread_mutex_unlock(&lock);
        if (!running) break;

        sleep(1); //ação a cada ~1 segundo 

        int escolha = rand_r(&rnd) % 3; // 0=mover,1=tarefa,2=nada 

        pthread_mutex_lock(&lock);
        if (is_impostor) {
            // Impostor tem 20% chance de matar a cada "tick" 
            int chance = rand_r(&rnd) % 100;
            if (chance < 20) {
                qtd_mortos++;
                printf("[IMPOSTOR][%s%d] matou alguém! -> total mortos: %d\n", nome, id, qtd_mortos);
            } else {
                printf("[IMPOSTOR][%s%d] rondando... (sem matar)\n", nome, id);
            }
        } else {
            if (escolha == 0) {
                const char *s = salas[rand_r(&rnd) % NUM_SALAS];
                printf("[NPC%d] moveu-se para %s\n", id, s);
            } else if (escolha == 1) {
                qtd_tarefas++;
                printf("[NPC%d] fez uma tarefa. Total tarefas: %d\n", id, qtd_tarefas);
            } else {
                printf("[NPC%d] está parado.\n", id);
            }
        }
        pthread_mutex_unlock(&lock);
    }

    printf("[THREAD NPC%d] encerrando.\n", id);
    return NULL;
}

// Menu interativo do usuário (rodando no main thread) 
void menu_usuario() {
    char line[LINE_BUF];
    while (1) {
        pthread_mutex_lock(&lock);
        int running = jogo_rodando;
        int mortos = qtd_mortos;
        int tarefas = qtd_tarefas;
        int trocas = qtd_trocas;
        pthread_mutex_unlock(&lock);

        if (!running) break;

        printf("\n===== MENU (PLAYER) =====\n");
        printf("Status -> mortos: %d | tarefas: %d | trocas: %d\n", mortos, tarefas, trocas);
        printf("1 - Trocar de sala\n");
        printf("2 - Fazer tarefa\n");
        printf("3 - Ver contadores (imprimir)\n");
        printf("4 - Encerrar jogo\n");
        printf("=========================\n");
        printf("Escolha: ");
        if (!fgets(line, sizeof(line), stdin)) {
            // EOF ou erro 
            break;
        }

        // remove newline 
        line[strcspn(line, "\n")] = 0;

        pthread_mutex_lock(&lock);
        if (strcmp(line, "1") == 0) {
            const char *s = salas[rand() % NUM_SALAS];
            qtd_trocas++;
            printf("[PLAYER] Você trocou para %s. total trocas: %d\n", s, qtd_trocas);
        } else if (strcmp(line, "2") == 0) {
            qtd_tarefas++;
            printf("[PLAYER] Você fez uma tarefa! total tarefas: %d\n", qtd_tarefas);
        } else if (strcmp(line, "3") == 0) {
            printf("[PLAYER] Contadores -> mortos: %d | tarefas: %d | trocas: %d\n",
                   qtd_mortos, qtd_tarefas, qtd_trocas);
        } else if (strcmp(line, "4") == 0) {
            printf("[PLAYER] Solicitou encerrar o jogo.\n");
            jogo_rodando = 0;
            pthread_mutex_unlock(&lock);
            break;
        } else {
            printf("Escolha inválida.\n");
        }
        pthread_mutex_unlock(&lock);
    }
}

int main() {
    srand((unsigned int)time(NULL));
    printf("=== Among-us simplificado (versao com pthreads) ===\n");
    int n;
    printf("Quantos NPCs (threads) criar? (1..%d): ", MAX_NPCS);
    if (scanf("%d%*c", &n) != 1) {
        fprintf(stderr, "Entrada inválida.\n");
        return 1;
    }
    if (n < 1) n = 1;
    if (n > MAX_NPCS) n = MAX_NPCS;

    pthread_t threads[MAX_NPCS];

    // Escolhe índices de NPCs e seleciona um impostor aleatório 
    int impostor_idx = rand() % n;
    printf(">>> O impostor será: NPC%d (internamente)\n", impostor_idx + 1);

    for (int i = 0; i < n; ++i) {
        npc_arg_t *arg = malloc(sizeof(npc_arg_t));
        if (!arg) {
            fprintf(stderr, "malloc fail\n");
            return 1;
        }
        arg->id = i + 1;
        arg->is_impostor = (i == impostor_idx) ? 1 : 0;
        if (pthread_create(&threads[i], NULL, npc_thread, arg) != 0) {
            fprintf(stderr, "Erro ao criar thread %d\n", i+1);
            free(arg);
            jogo_rodando = 0;
            // join already created threads 
            for (int j = 0; j < i; ++j) pthread_join(threads[j], NULL);
            return 1;
        }
    }

    // Roda menu do usuário (bloqueante de input) 
    menu_usuario();

    // Espera todas as threads terminarem 
    for (int i = 0; i < n; ++i) {
        pthread_join(threads[i], NULL);
    }

    printf("\nJogo encerrado. Resultado final:\n");
    printf("Mortos: %d\n", qtd_mortos);
    printf("Tarefas: %d\n", qtd_tarefas);
    printf("Trocas: %d\n", qtd_trocas);
    return 0;
}
