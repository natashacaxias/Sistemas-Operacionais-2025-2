#include<bits/stdc++.h>
using namespace std;
#include <unistd.h>

struct usuario{
    string nome;
    int pontos = 0;

    void mostrar_usuario(){
        printf("%6d | %s\n", pontos, nome.c_str());
    }
};

struct ranking{
    int qtd_jogadores = 0;
    vector<usuario> jogador;
    int qtd_leituras_sujas = 0;

    // construtor da semente para gerar números aleatórios
    random_device rd;
    mt19937 gen;
    ranking() : gen(rd()) {}

    // Ordena o vetor de jogadores em ordem descrescentemente
    void insertionSort() {
        int i, j;
        usuario pivot;
        for(i = 1; i < qtd_jogadores; i++) { 
            pivot = jogador[i];
            j = i -1;
            while((j>=0) && ( jogador[j].pontos<pivot.pontos) ) {
                jogador[j+1] = jogador[j];
                j--;
            }
            jogador[j+1] = pivot;
        }
    }

    void atualizarRanking(){
        insertionSort();
    }

    // Mostra o ranking ordenado descrescente
    void exibir_validar_ranking(){
        if(qtd_jogadores==0) return;

        printf("\nRanking | Pontos | Nome\n");
        bool valido = true;
        int pos_inc;
        for(int i=0;i<qtd_jogadores;i++){
            if(i<qtd_jogadores-1 && jogador[i].pontos < jogador[i+1].pontos) {
                valido = false; 
                pos_inc = i;
            }
            printf("%7d | ", i+1);
            jogador[i].mostrar_usuario();
        }
        if (valido) printf("Ranking Correto\n\n");
        else {
            qtd_leituras_sujas++;
            printf("Leitura Suja: posicao %d incorreta.\n\n", pos_inc+1);
        }
    }

    // Soma ou subtrai pontos de um jogador aleatório
    void simular_evento(){
        if (qtd_jogadores == 0) return;

        // Gerar números aleatórios
        uniform_int_distribution<> dist_idx(0, qtd_jogadores - 1);
        uniform_int_distribution<> dist_pontos(1, 100);
        uniform_int_distribution<> dist_sinal(0, 1);

        int idx = dist_idx(gen);
        int sinal = dist_sinal(gen) ? 1 : -1;
        int pontos = dist_pontos(gen) * sinal;

        jogador[idx].pontos += pontos;
        
        printf("%s %s%d | ", jogador[idx].nome.c_str(), 
               ((pontos >= 0) ? "+" : "-"), abs(pontos));

    }

    void adicionar_jogador(string nome){
        usuario novo_jogador;
        novo_jogador.nome = nome;
        novo_jogador.pontos = uniform_int_distribution<>(0,5)(gen);
        jogador.push_back(novo_jogador);
        qtd_jogadores++;
    }

    int tamanho(){
        return qtd_jogadores;
    }
};