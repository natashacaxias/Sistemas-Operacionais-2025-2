#include<bits/stdc++.h>
using namespace std;

struct usuario{
    string nome;
    int pontos = 0;

    void mostrar_usuario(){
        cout << setw(6) << right << pontos << " | " 
             << setw(nome.size()) << left << nome << "\n";
    }
};

struct ranking{

    int qtd_jogadores = 0;
    vector<usuario> jogador;

    random_device rd;
    mt19937 gen;
    ranking() : gen(rd()) {}

    // Ordena o vetor de jogador em ordem descrescentemente
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

    // Mostra o ranking ordenado descrescente
    void exibir_validar_ranking(){
        insertionSort();
        cout << "Ranking | Pontos | Nome\n";

        bool valido = true;
        int pos_inc;
        for(int i=0;i<qtd_jogadores;i++){
            if(i<qtd_jogadores-1 && jogador[i].pontos < jogador[i+1].pontos) {
                valido = false; 
                pos_inc = i;
            }
            cout << setw(7) << right << i+1 << " | ";
            jogador[i].mostrar_usuario();
        }
        if (valido) cout << "Ranking Correto\n\n";
        else cout << "Leitura Suja: posição " << pos_inc << "incorreta.";
    }

    // Soma ou subtrai pontos de um jogador aleatório
    void simular_evento(){
 if (qtd_jogadores == 0) return;

        // Gerar números aleatórios
        uniform_int_distribution<> dist_idx(0, qtd_jogadores - 1);
        uniform_int_distribution<> dist_pontos(0, 100);
        uniform_int_distribution<> dist_sinal(0, 1);

        int idx = dist_idx(gen);
        int sinal = dist_sinal(gen) ? 1 : -1;
        int pontos = dist_pontos(gen) * sinal;

        jogador[idx].pontos += pontos;
        
        cout << "Evento: " << jogador[idx].nome << ((pontos >= 0) ? " ganhou +":" perdeu ") << pontos << " pontos.\n\n";
    }

    void adicionar_jogador(usuario novo_jogador){
        jogador.push_back(novo_jogador);
        qtd_jogadores++;
    }
};

int main(){

    string nomes[10] = {"Ana", "João", "Maria", "Pedro", "Julia", "Lucas", "Carla", "Miguel", "Sofia", "Rafael"};

    ranking new_ranking;

    for(int i=0;i<3;i++){
        usuario novo_usuario;
        novo_usuario.nome = nomes[i];
        new_ranking.adicionar_jogador(novo_usuario);
        new_ranking.simular_evento();
    }

    new_ranking.exibir_validar_ranking();

    for(int i=4;i<10;i++){
        usuario novo_usuario;
        novo_usuario.nome = nomes[i];
        new_ranking.adicionar_jogador(novo_usuario);
        new_ranking.simular_evento();
    }

    for(int i=0;i<20;i++){
        new_ranking.simular_evento();
        if(i%5==0) new_ranking.exibir_validar_ranking();
    }

    new_ranking.exibir_validar_ranking();

    for(int i=0;i<5;i++){
        new_ranking.simular_evento();
        new_ranking.exibir_validar_ranking();
    }

}