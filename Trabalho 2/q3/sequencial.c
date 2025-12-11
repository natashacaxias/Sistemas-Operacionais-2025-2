#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>   // sleep
#include <time.h>
#define MAX_NPCS 100
#define LINE_BUF 128

const char *salas[] = {"Cafeteria", "Elétrica", "Reator", "Admin", "Navegação"};
const int NUM_SALAS = sizeof(salas)/sizeof(salas[0]);

int qtd_mortos = 0;
int qtd_tarefas = 0;
int qtd_trocas = 0;
int jogo_rodando = 1;

void acao_npc(int id, int is_impostor) {
    int escolha = rand() % 3; //0=mover,1=tarefa,2=nada 

    if (is_impostor) {
        int chance = rand() % 100;
        if (chance < 20) {
            qtd_mortos++;
            printf("[IMPOSTOR][NPC%d] matou alguém! -> total mortos: %d\n", id, qtd_mortos);
        } else {
            printf("[IMPOSTOR][NPC%d] rondando... (sem matar)\n", id);
        }
    } else {
        if (escolha == 0) {
            const char *s = salas[rand() % NUM_SALAS];
            printf("[NPC%d] moveu-se para %s\n", id, s);
        } else if (escolha == 1) {
            qtd_tarefas++;
            printf("[NPC%d] fez uma tarefa. Total tarefas: %d\n", id, qtd_tarefas);
        } else {
            printf("[NPC%d] está parado.\n", id);
        }
    }
}

int main() {
    srand((unsigned int)time(NULL));
    printf("=== Among-us simplificado (versao sequencial) ===\n");
    int n;
    printf("Quantos NPCs criar? (1..%d): ", MAX_NPCS);
    if (scanf("%d%*c", &n) != 1) {
        fprintf(stderr, "Entrada inválida.\n");
        return 1;
    }
    if (n < 1) n = 1;
    if (n > MAX_NPCS) n = MAX_NPCS;

    int impostor_idx = rand() % n;
    printf(">>> O impostor será: NPC%d\n", impostor_idx + 1);

    char line[LINE_BUF];
    while (jogo_rodando) {
        printf("\n===== MENU (PLAYER) =====\n");
        printf("Status -> mortos: %d | tarefas: %d | trocas: %d\n", qtd_mortos, qtd_tarefas, qtd_trocas);
        printf("1 - Trocar de sala\n");
        printf("2 - Fazer tarefa\n");
        printf("3 - Ver contadores (imprimir)\n");
        printf("4 - Encerrar jogo\n");
        printf("=========================\n");
        printf("Escolha: ");

        if (!fgets(line, sizeof(line), stdin)) break;
        line[strcspn(line, "\n")] = 0;

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
            printf("[PLAYER] Encerrando jogo.\n");
            jogo_rodando = 0;
            break;
        } else {
            printf("Escolha inválida.\n");
        }

        // Agora os NPCs agem *sequencialmente
        printf("\n--- NPCs agindo (sequencial) ---\n");
        for (int i = 0; i < n; ++i) {
            sleep(1); // simula delay entre ações de NPCs 
            acao_npc(i+1, (i == impostor_idx));
        }
    }

    printf("\nJogo encerrado. Resultado final:\n");
    printf("Mortos: %d\n", qtd_mortos);
    printf("Tarefas: %d\n", qtd_tarefas);
    printf("Trocas: %d\n", qtd_trocas);
    return 0;
}
