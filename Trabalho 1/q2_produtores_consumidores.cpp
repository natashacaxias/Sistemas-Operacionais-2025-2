#include <bits/stdc++.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "ranking.cpp"   
using namespace std;


#define BUFFER_SIZE 5
#define SENTINELA -1   // Marca fim da produção (nenhum jogador restante)

// Buffer compartilhado — representa a fila de resultados das partidas
vector<int> buffer;

// Semáforos de sincronização
sem_t sem_cheio, sem_vazio, sem_mutex;

// Controle global
pthread_mutex_t mutex_flag = PTHREAD_MUTEX_INITIALIZER;
int produtores_finalizados = 0;
int nProd_global = 0;
int nCons_global = 0;

// Estrutura de parâmetros de thread
struct argsThread {
    int id;
    int versao;
};

// Ranking global compartilhado entre todas as threads
ranking global_ranking;

// ==================================================================
//  FUNÇÃO DO PRODUTOR – Jogador enviando resultados de partidas
// ==================================================================
void* produtor(void* arg) {
    argsThread* args = (argsThread*)arg;
    int id = args->id;
    int versao = args->versao;
    int max_iter = 5; // Cada jogador joga 5 partidas

    for (int i = 0; i < max_iter; i++) {
        int valor = rand() % 100 + 1; // Pontuação aleatória obtida em uma partida
        usleep((rand() % 500 + 300) * 1000); // Simula tempo entre partidas

        // ---------------- SEM CONTROLE ----------------
        if (versao == 3) {
            if ((int)buffer.size() < BUFFER_SIZE) {
                buffer.push_back(valor);
                printf("[Jogador %d] enviou %d pontos | Buffer: ", id, valor);
                for (int v : buffer) printf("%d ", v);
                printf("\n");
            } else {
                printf("[Jogador %d] tentou enviar, mas buffer cheio! (sem controle)\n", id);
            }
        }
        // ---------------- COM CONTROLE (versões 1 e 2) ----------------
        else {
            sem_wait(&sem_vazio);  // Espera espaço livre
            sem_wait(&sem_mutex);  // Entra na região crítica

            buffer.push_back(valor);
            printf("[Jogador %d] enviou %d pontos | Buffer: ", id, valor);
            for (int v : buffer) printf("%d ", v);
            printf("\n");

            sem_post(&sem_mutex);  // Sai da região crítica
            sem_post(&sem_cheio);  // Notifica que há novo dado
        }
        usleep(300000);
    }

    // Marca jogador como finalizado
    pthread_mutex_lock(&mutex_flag);
    produtores_finalizados++;
    bool ultimo_produtor = (produtores_finalizados == nProd_global);
    pthread_mutex_unlock(&mutex_flag);

    // Último jogador insere sentinelas para encerrar os consumidores
    if (ultimo_produtor && versao != 3) {
        for (int i = 0; i < nCons_global; i++) {
            sem_wait(&sem_vazio);
            sem_wait(&sem_mutex);

            buffer.push_back(SENTINELA);
            printf("[Jogador %d] inseriu SENTINELA (-1) [%d/%d] | Buffer: ", id, i+1, nCons_global);
            for (int v : buffer) printf("%d ", v);
            printf("\n");

            sem_post(&sem_mutex);
            sem_post(&sem_cheio);
        }
    }

    printf("[Jogador %d] terminou suas partidas.\n", id);
    delete args;
    return NULL;
}

// ==================================================================
//  FUNÇÃO DO CONSUMIDOR – Servidor processando pontuações
// ==================================================================
void* consumidor(void* arg) {
    argsThread* args = (argsThread*)arg;
    int id = args->id;
    int versao = args->versao;

    while (true) {
        usleep((rand() % 800 + 300) * 1000); // Atraso aleatório (latência servidor)

        // ---------------- SEM CONTROLE ----------------
        if (versao == 3) {
            if (!buffer.empty()) {
                int valor = buffer.front();
                buffer.erase(buffer.begin());
                if (valor == SENTINELA) break;

                printf("[Servidor %d] processou %d pontos | Buffer: ", id, valor);
                for (int v : buffer) printf("%d ", v);
                printf("\n");

                // Atualiza ranking sem sincronização (problemas possíveis)
                global_ranking.simular_evento();
                global_ranking.atualizarRanking();
                global_ranking.exibir_validar_ranking();
            } else {
                printf("[Servidor %d] aguardando pontuacoes... (sem controle)\n", id);
            }
        }
        // ---------------- COM CONTROLE ----------------
        else {
            sem_wait(&sem_cheio);
            sem_wait(&sem_mutex);

            int valor = buffer.front();
            buffer.erase(buffer.begin());

            if (valor == SENTINELA) {
                printf("[Servidor %d] recebeu SENTINELA (-1). Encerrando.\n", id);
                sem_post(&sem_mutex);
                sem_post(&sem_vazio);
                break;
            }

            printf("[Servidor %d] aplicando %d pontos ao ranking...\n", id, valor);

            sem_post(&sem_mutex);
            sem_post(&sem_vazio);

            // Simula atualização real do ranking
            global_ranking.simular_evento();    // Escolhe jogador aleatório e altera pontos
            global_ranking.atualizarRanking();  // Reordena ranking
            global_ranking.exibir_validar_ranking();
        }

        usleep(500000); // Intervalo entre atualizações
    }

    delete args;
    return NULL;
}

// ==================================================================
//  MAIN – Configura e executa a simulação
// ==================================================================
int main() {
    srand(time(NULL));

    printf("=== TP1 - Produtores x Consumidores (Tema: Ranking de Jogo) ===\n");
    printf("Escolha a versao:\n");
    printf("  1) Varios jogadores e 1 servidor\n");
    printf("  2) Varios jogadores e varios servidores\n");
    printf("  3) Sem controle de concorrencia (para demonstrar problema)\n");
    printf("Versao: ");

    int versao;
    scanf("%d", &versao);
    printf("\nExecutando versao %d...\n\n", versao);

    // Inicializa semáforos
    sem_init(&sem_cheio, 0, 0);
    sem_init(&sem_vazio, 0, BUFFER_SIZE);
    sem_init(&sem_mutex, 0, 1);

    // Cria jogadores no ranking
    string nomes[5] = {"Ana", "Pedro", "Julia", "Lucas", "Sofia"};
    for (int i = 0; i < 5; i++)
        global_ranking.adicionar_jogador(nomes[i]);

    global_ranking.atualizarRanking();
    printf("\n=== RANKING INICIAL ===\n");
    global_ranking.exibir_validar_ranking();

    // Define número de threads conforme versão
    int nProd = (versao == 1 ? 3 : 4);
    int nCons = (versao == 1 ? 1 : 2);
    nProd_global = nProd;
    nCons_global = nCons;

    vector<pthread_t> threads;

    // Cria threads dos jogadores (produtores)
    for (int i = 0; i < nProd; i++) {
        argsThread* args = new argsThread();
        args->id = i;
        args->versao = versao;
        pthread_t t;
        pthread_create(&t, NULL, produtor, args);
        threads.push_back(t);
    }

    // Cria threads dos servidores (consumidores)
    for (int i = 0; i < nCons; i++) {
        argsThread* args = new argsThread();
        args->id = i;
        args->versao = versao;
        pthread_t t;
        pthread_create(&t, NULL, consumidor, args);
        threads.push_back(t);
    }

    // Aguarda término das threads
    for (pthread_t& t : threads)
        pthread_join(t, NULL);

    // Finaliza semáforos e mutex
    sem_destroy(&sem_cheio);
    sem_destroy(&sem_vazio);
    sem_destroy(&sem_mutex);
    pthread_mutex_destroy(&mutex_flag);

    printf("\n=== Execução finalizada ===\n");
    printf("Ranking final:\n");
    global_ranking.exibir_validar_ranking();
    printf("\n");

    return 0;
}
