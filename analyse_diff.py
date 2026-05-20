import numpy as np

# Lire les valeurs depuis run_many.sh
vals = []

with open("results/diffs.txt") as f:
    for line in f:
        if line.startswith("#"):
            continue
        _, _, diff = line.split()
        vals.append(float(diff))

vals = np.array(vals)

print("Nombre d'échantillons :", len(vals))
print("Moyenne :", np.mean(vals))
print("Écart-type :", np.std(vals))
print("Erreur relative moyenne :", np.mean(vals) / 1e6)
print("Erreur relative std :", np.std(vals) / 1e6)
