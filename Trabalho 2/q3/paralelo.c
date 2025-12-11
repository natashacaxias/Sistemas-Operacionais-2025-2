import threading
import time
import random


// VARIÁVEIS COMPARTILHADAS

lock = threading.Lock()
qtd_mortos = 0
qtd_tarefas = 0
qtd_trocas = 0
jogo_rodando = True

# Salas do jogo
salas = ["Cafeteria", "Elétrica", "Reator", "Admin", "Navegação"]

# Posição inicial do usuário
player_sala = "Cafeteria"

# Lista de NPCs
NPCS = []
IMPOSTOR = None


// THREAD DOS NPCs

def npc_thread(nome, impostor=False):
    global qtd_mortos, qtd_tarefas, salas, jogo_rodando

    sala_atual = random.choice(salas)

    while jogo_rodando:
        time.sleep(1)

        acao = random.choice(["mover", "tarefa", "nada"])

        with lock:
            if impostor:
                # 20% de chance de matar
                if random.random() < 0.2:
                    qtd_mortos += 1
                    print(f"[IMPOSTOR] {nome} matou alguém! Total mortos: {qtd_mortos}")
                else:
                    print(f"[IMPOSTOR] {nome} não matou ninguém desta vez.")
            else:
                if acao == "mover":
                    sala_atual = random.choice(salas)
                    print(f"[NPC] {nome} mudou para {sala_atual}")
                elif acao == "tarefa":
                    qtd_tarefas += 1
                    print(f"[NPC] {nome} fez uma tarefa. Tarefas concluídas: {qtd_tarefas}")
                else:
                    print(f"[NPC] {nome} está parado.")


// LÓGICA DO USUÁRIO
def menu_usuario():
    global qtd_tarefas, qtd_trocas, player_sala, jogo_rodando

    while jogo_rodando:
        print("\n====== MENU ======")
        print("1 - Trocar de sala")
        print("2 - Fazer tarefa")
        print("3 - Encerrar jogo")
        print("===================")
        escolha = input("Escolha sua ação: ")

        with lock:
            if escolha == "1":
                player_sala = random.choice(salas)
                qtd_trocas += 1
                print(f"[PLAYER] Você mudou para {player_sala}. Total trocas: {qtd_trocas}")

            elif escolha == "2":
                qtd_tarefas += 1
                print(f"[PLAYER] Você fez uma tarefa! Total: {qtd_tarefas}")

            elif escolha == "3":
                print("Encerrando jogo...")
                jogo_rodando = False
                break

            else:
                print("Escolha inválida!")


//INICIALIZAÇÃO DO SISTEMA

def main():
    global IMPOSTOR

    qtd_threads = int(input("Digite quantos NPCs deseja criar: "))

    # Criar NPCs
    for i in range(qtd_threads):
        nome = f"NPC{i+1}"
        NPCS.append(nome)

    # Escolher impostor
    IMPOSTOR = random.choice(NPCS)
    print(f"\n>>> O impostor será: {IMPOSTOR} <<<\n")

    # Criar threads
    for npc in NPCS:
        t = threading.Thread(target=npc_thread, args=(npc, npc == IMPOSTOR))
        t.daemon = True
        t.start()

    # Rodar menu do usuário
    menu_usuario()


if __name__ == "__main__":
    main()

