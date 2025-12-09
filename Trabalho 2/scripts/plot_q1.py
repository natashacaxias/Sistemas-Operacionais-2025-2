import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# ========= CONFIGURAÇÃO ==========
csv_path = "q1/data/resultados_speedup.csv"   # ajuste se necessário
output_dir = "q1/output/"                       # onde salvar os gráficos
plt.style.use("seaborn-v0_8")

# ========= CARREGAR CSV ==========
df = pd.read_csv(csv_path)

print("\n================ TABELA ORIGINAL ================\n")
print(df)

# ========= TABELA AGRUPADA: MELHOR POR TAMANHO ==========
print("\n================ TEMPO MÉDIO POR TAMANHO ================\n")
tabela_tam = df.groupby("n")[["tempo_seq", "tempo_par"]].mean()
print(tabela_tam)

# ========= TABELA LATeX ==========
print("\n================ TABELA PARA LATEX ================\n")
print(df.to_latex(index=False, float_format="%.6f"))

# ========= GRÁFICO TEMPO SEQ vs PAR ==========
plt.figure(figsize=(10,6))

# Criar coluna com rótulo correto da legenda
df["threads_label"] = df["threads"].astype(str) + " threads"

# Linha sequencial com cor fixa
sns.lineplot(
    data=df.drop_duplicates("n"),
    x="n", y="tempo_seq",
    marker="o",
    label="Sequencial",
    color="black",
    linewidth=2
)

# Linhas paralelas com rótulos adequados
sns.lineplot(
    data=df,
    x="n", y="tempo_par",
    hue="threads_label",         # <- LEGENDA PERSONALIZADA
    marker="o",
    palette="tab10",
    linewidth=2
)

plt.title("Comparação de Tempos: Sequencial vs Paralelo")
plt.xlabel("Tamanho da Vetor (n)")
plt.ylabel("Tempo (s)")
plt.grid(True)
plt.tight_layout()
plt.savefig(output_dir+"tempo_seq_vs_par.png")
plt.show()


# ========= GRÁFICO SPEEDUP x THREADS ==========
plt.figure(figsize=(10,6))
sns.lineplot(data=df, x="threads", y="speedup", hue="n", marker="o")

plt.title("Speedup em Função do Número de Threads")
plt.xlabel("Número de Threads")
plt.ylabel("Speedup (T_seq / T_par)")
plt.grid(True)
plt.tight_layout()
plt.savefig(output_dir+"speedup_vs_threads.png")
plt.show()

# ========= GRÁFICO SPEEDUP x TAMANHO ==========
plt.figure(figsize=(10,6))
sns.lineplot(data=df, x="n", y="speedup", hue="threads", marker="o")

plt.title("Speedup em Função do Tamanho do Vetor")
plt.xlabel("Tamanho do Vetor (n)")
plt.ylabel("Speedup")
plt.grid(True)
plt.tight_layout()
plt.savefig(output_dir+"speedup_vs_tamanho.png")
plt.show()

print("\nGráficos gerados na pasta:", output_dir)
