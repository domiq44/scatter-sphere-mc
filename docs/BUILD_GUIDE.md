# 🏗️ Guide de Construction du Projet (Build Guide)

Ce document explique comment compiler et tester les différents composants de notre projet (Simulateur, Plot, Tests) en utilisant les outils fournis.

Le projet utilise une approche hybride : le **`Makefile`** permet une construction locale rapide, tandis que les **`Dockerfile`** garantissent une construction parfaite et reproductible dans n'importe quel environnement (comme un serveur CI/CD).

---

## 🛑 Prérequis Systèmes : Le Choix de votre Environnement

**Ceci est la distinction la plus importante :** Selon la méthode que vous choisissez pour construire, les prérequis sur votre machine hôte sont différents.

### 💻 Méthode Locale (Utilisation du `Makefile`)
Si vous utilisez `make tests`, vous construisez directement sur votre machine. Vous avez besoin de :
*   **Compilateurs/Outils** : `gcc`, `make`, `cmake`, `pkg-config`, `clang-format`.
*   **Bibliothèques** : `libcmocka-devel` doit être installé sur votre système.

**Pour installer les dépendances locales critiques, exécutez (si nécessaire) :**
```bash
sudo dnf install gcc make cmake git pkg-config libcmocka-devel
```

### 🐳 Méthode Containerisée (Utilisation de `./build.sh`)
Si vous utilisez `./build.sh`, vous déléguez toute la gestion des dépendances à Docker. Vous avez besoin de :
*   **Outil Principal** : **Docker** (et Docker Compose, si vous l'utilisez ailleurs).
*   **Avantage** : Toutes les dépendances système (GCC, Gnuplot, etc.) sont installées *à l'intérieur* des conteneurs, garantissant une reproductibilité parfaite, indépendamment de ce qui est installé sur votre machine hôte.

---

## 🧱 Partie 1 : Les Composants de Construction (Définition des outils)

Cette section présente les outils que nous utilisons pour assembler le projet : le `Makefile` pour le développement local, et les Dockerfiles pour l'isolation.

### A. Le `Makefile` (Développement Rapide)
Le `Makefile` est votre outil principal pour le développement quotidien. Il orchestre la compilation en utilisant votre machine locale.
* **Rôle** : Compiler les sources en utilisant les flags optimisés (`-O3`, `-std=c11`), gérer les dépendances externes (`cmocka` via `pkg-config`), et fournir des commandes de nettoyage rapides (`make clean`).
* **Commandes Clés** : `make all` (pour compiler), `make tests` (pour lancer les tests locaux).

### B. Les Dockerfiles (Environnement Contrôlé)
Les Dockerfiles garantissent que l'environnement de compilation est *toujours* le même. Ils sont structurés en étapes de *Build* puis de *Runtime* pour minimiser la taille finale.
* **`Dockerfile.simulate`** : Construit et prépare l'exécutable de simulation.
* **`Dockerfile.plot`** : Construit et prépare l'exécutable de traçage, incluant `gnuplot`.
* **`Dockerfile.tests`** : Le plus sophistiqué. Il orchestre l'analyse de qualité :
    1. **Builder** : Compile le code et génère `compile_commands.json` (nécessaire pour l'analyse).
    2. **Lint** : Exécute `clang-tidy` et `cppcheck` pour vérifier la qualité du code.
    3. **Test Runner** : Exécute les tests unitaires dans un conteneur isolé.

---

## ⚙️ Partie 2 : L'Action de Construction (Comment assembler le tout)

Ceci est la procédure standard pour créer toutes les versions de votre projet.

### ➡️ Procédure Standard (Recommandée pour CI/CD)
Utilisez le script `./build.sh`. Il exécute séquentiellement :
1. `Build simulate`
2. `Build plot`
3. `Build tests` (C'est ici que toute la logique de Linting/Testing se déroule)
4. `Run tests` (Exécution finale dans le conteneur de test)

### 🏃 Procédure Locale (Pour le Développeur)
Utilisez le `Makefile` pour un retour d'information immédiat :
* Pour compiler et tester rapidement : `make tests`.

---

## ✅ Partie 3 : Vérification et Validation (Le Cœur de la Qualité)

Cette section explique ce que signifie le succès dans chaque environnement.

### A. Validation Locale (Niveau 1 - `make tests`)
L'exécution locale montre que l'application fonctionne correctement sur votre machine hôte.
* **Résultat attendu** : Tous les tests passent avec succès.
* **Interprétation des erreurs capturées** : Si des erreurs sont capturées mais que le test passe (`[ OK ]`), cela confirme que **votre code gère correctement cette erreur** (le test valide le comportement de l'erreur).

### B. Validation Docker (Niveau 2 & 3 - `./build.sh`)
Le succès de `./build.sh` prouve que l'environnement Docker est parfaitement fonctionnel.
* **Rapidité** : Si vous relancez `./build.sh` et que les étapes sont marquées `CACHED`, cela signifie que votre système de cache Docker fonctionne efficacement.
* **Validation de la Qualité** : Le fait que le build se termine sans interruption signifie que les outils d'analyse statique (`clang-tidy` et `cppcheck`) ont validé la qualité du code.

---

## 🚀 Optimisation et Mise à Jour (Prochaines Étapes)

Pour passer à un niveau de production maximal, l'objectif suivant est d'optimiser les images de runtime.

* **Minimiser les Images** : Dans les étapes `runtime` des Dockerfiles, envisagez de remplacer `fedora:44` par une distribution plus légère (comme `debian:slim` ou `alpine`) après avoir copié uniquement les binaires finaux. Ceci réduit la taille et la surface d'attaque de votre déploiement final.
