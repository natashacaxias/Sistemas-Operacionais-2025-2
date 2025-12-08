import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import sys
import os

plt.style.use("seaborn-v0_8")

# ------------------------
# LER NOMES DOS CSVs
# ------------------------
if len(sys.argv) < 3:
    print("Uso: python3 comparar_maquinas.py maquina1.csv maquina2.csv")
    sys.exit(1)

csv1 = sys.argv[1]
csv2 = sys.argv[2]

nome1 = os.path.splitext(os.path.basename(csv1))[0]
nome2 = os.path.splitext(os.path.basename(csv2))[0]

# ------------------------
# CARREGAR OS CSVs
# ------------------------
df1 = pd.read_csv(csv1)
df2 = pd.read_csv(csv2)

df1["maquina"] = nome1
df2["maquina"] = nome2

# Juntar tudo
df = pd.concat([df1, df2], ignore_index=True)

# Pasta de saída
output_dir = "../q2/data/comparacao/"
os.makedirs(output_dir, exist_ok=True)

# ------------------------
# TABELA COMPARATIVA
# ------------------------
tabela = df.pivot_table(
    index=["n", "threads"],
    columns="maquina",
    values=["tempo_seq", "tempo_par", "speedup"]
)

print("\n========== TABELA COMPARATIVA ==========\n")
print(tabela)

tabela.to_csv(output_dir + "tabela_comparativa.csv")

# ------------------------
# TABELA LATEX
# ------------------------
with open(output_dir + "tabela_comparativa.tex", "w") as f:
    f.write(tabela.to_latex(float_format="%.6f"))

print("\nTabela LaTeX salva em:", output_dir + "tabela_comparativa.tex")

# ------------------------
# GRÁFICO SPEEDUP (Máquina A vs Máquina B)
# ------------------------
plt.figure(figsize=(10,6))

sns.lineplot(data=df, x="n", y="speedup", hue="maquina", style="threads", marker="o")

plt.title("Speedup: Comparação entre Máquinas")
plt.xlabel("Tamanho da matriz (n)")
plt.ylabel("Speedup (Tseq / Tpar)")
plt.grid(True)
plt.tight_layout()

plt.savefig(output_dir + "speedup_comparacao.png")
plt.show()

# ------------------------
# GRÁFICO TEMPO PARALELO POR MÁQUINA
# ------------------------
plt.figure(figsize=(10,6))

sns.lineplot(data=df, x="n", y="tempo_par", hue="maquina", style="threads", marker="o")

plt.title("Tempo Paralelo: Comparação entre Máquinas")
plt.xlabel("Tamanho da matriz (n)")
plt.ylabel("Tempo (s)")
plt.grid(True)
plt.tight_layout()

plt.savefig(output_dir + "tempo_par_comparacao.png")
plt.show()

print("\nArquivos gerados na pasta:", output_dir)
