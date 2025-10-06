#include<bits/stdc++.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "ranking.cpp"

using namespace std;

struct argsThread {
    int id;
    int ver;
};

ranking global_ranking;
sem_t sem_mutex, sem_dados;
int qtd_leitores, qtd_escritores;

void *leitor(void *arg){
    argsThread *args = (argsThread*)arg;
    int id = args->id;
    int ver = args->ver;
    
    if (ver == 3 || ver==1){     //sem controle de concorrência
        //printf("Thread %d (leitura) começou...\n", id);
        usleep(5000);
        global_ranking.exibir_validar_ranking();
        //printf("Thread %d (leitura) terminou\n", id);
    }

    else if(ver == 2){           // escritores com prioridade sobre leitores

        sem_wait(&sem_mutex);
        qtd_leitores++;
        if (qtd_leitores == 1) {
            sem_wait(&sem_dados); // primeiro leitor bloqueia escritores
        }
        sem_post(&sem_mutex);
        
        // Seção Crítica - LEITURA
        //printf("Thread %d (leitura) começou...\n", id);
        global_ranking.exibir_validar_ranking();
        //printf("Thread %d (leitura) terminou\n", id);
        // -----------------------

        sem_wait(&sem_mutex);
        qtd_leitores--;
        if (qtd_leitores == 0) {
            sem_post(&sem_dados); // Último leitor libera escritores
        }
        sem_post(&sem_mutex);
    }

    delete args;
    return NULL;
}

void *escritor(void *arg){
    argsThread *args = (argsThread*)arg;
    int id = args->id;
    int ver = args->ver;

    if(ver==1 || ver == 2){
        //printf("Thread %d (escrita) começou...\n", id);
        sem_wait(&sem_dados);
        global_ranking.simular_evento();
        usleep(10500);
        global_ranking.atualizarRanking();
        sem_post(&sem_dados);
        //printf("Thread %d (escrita) terminou\n", id);

    }

    else {
        //printf("Thread %d (escrita) começou...\n", id);
        global_ranking.simular_evento();
        usleep(20000);
        global_ranking.atualizarRanking();
        //printf("Thread %d (escrita) terminou\n", id);
    }
    
    delete args;
    return NULL;
}

int main(){
    string nome[10] = {"Ana", "Joca", "Maria", "Pedro", "Julia", "Lucas", "Carla", "Miguel", "Sofia", "Rafael"};
    int versao = 2;

    printf("Simulação de ranking de jogo:\n");
    printf("1. Sem controle de prioridade\n2. Escritores com prioridade sobre leitores\n3. Sem controle de concorrencia\nEscolha uma versao: ");
    scanf("%d", &versao);

    qtd_escritores = 0;
    qtd_leitores = 0;

    vector<pthread_t> threads;
    argsThread* args;
    pthread_t thread;
    pthread_attr_t attr;

    sem_init(&sem_mutex, 0, 1);
    sem_init(&sem_dados, 0, 1);

    for(int i=0;i<10;i++){
        global_ranking.adicionar_jogador(nome[i]);
    }

    printf("\n=== RANKING APOS INSERCOES == \n");
    global_ranking.atualizarRanking();
    global_ranking.exibir_validar_ranking();

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    printf("\n=== SIMULACOEES LEITURA/ESCRITA == \n\n");

    int threadId = 0;
    for(int i=0;i<20;i++){
        vector<pthread_t> ciclo_threads; 
        for(int j=0;j<10;j++){
            args = new argsThread();
            args->id = threadId++;
            args->ver = versao;
            pthread_create(&thread, &attr, escritor, args);
            ciclo_threads.push_back(thread);
        }

        args = new argsThread();
        args->id = threadId++;
        args->ver = versao;
        pthread_create(&thread, &attr, leitor, args);
        ciclo_threads.push_back(thread);

        for(size_t k=0; k<ciclo_threads.size(); k++) {
            pthread_join(ciclo_threads[k], NULL);
        }
    }
    
    for(int i=0;i<threads.size();i++) pthread_join(threads[i], NULL);

    printf("\n=== RANKING FINAL ===\n");
    global_ranking.exibir_validar_ranking();
    printf("\nTotal de leituras sujas: %d\n\n", global_ranking.qtd_leituras_sujas);

    // Desativas semáforos
    sem_destroy(&sem_mutex);
    sem_destroy(&sem_dados);

    return 0;
}