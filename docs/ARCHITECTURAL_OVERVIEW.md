# 📂 Architecture du Projet (Vue d'Ensemble)

Ce document sert de **carte maîtresse** de votre projet. Il est conçu pour qu'un nouvel utilisateur puisse comprendre instantanément où se trouve chaque pièce du puzzle : le code source, les outils de construction, les scripts d'exécution et les fichiers de configuration. Il relie tous les autres guides (Build Guide, Deployment Overview, etc.).

## 🌳 Vue d'Ensemble de l'Arborescence

L'architecture est divisée en couches logiques, chacune ayant un rôle spécifique dans le cycle de vie du projet.

### 1. 🧠 La Logique Physique (Le Noyau)
Ce sont les fichiers qui contiennent l'intelligence du simulateur.
*   **`src/`** : Contient les fichiers `.c` qui implémentent les algorithmes physiques (Monte-Carlo, diffusion HG, géométrie, RNG).
*   **`include/`** : Fournit les fichiers d'en-tête (`.h`) qui définissent les interfaces (les "contrats") entre les différents modules de `src/`.
*   **`config.toml` / `config.template.toml`** : Définissent les hyperparamètres d'entrée de la simulation (nombre de photons, $\sigma_e$, $\theta_0$, etc.).

### 🛠️ 2. Les Outils de Construction (Build Tools)
Ces éléments sont responsables de transformer le code source en exécutables utilisables.
*   **`Makefile`** : L'outil de construction local. Il compile le code C directement sur votre machine Fedora pour un développement rapide (`make tests`, `make all`).
*   **`Dockerfile.*`** : Les recettes pour créer des environnements isolés. Ils garantissent que le code fonctionne *exactement* de la même manière sur n'importe quel système (CI/CD, Production).
*   **`build.sh`** : Le script qui orchestre l'appel séquentiel de ces Dockerfiles pour créer tous les artefacts nécessaires.

### 🏃 3. Les Scripts d'Exécution (Orchestration Runtime)
Ces scripts utilisent les artefacts construits pour faire tourner les expériences.
*   **`run.sh`** : Exécute le pipeline *standard* : Simulation $\rightarrow$ Tri $\rightarrow$ Plotting, pour un jeu de paramètres par défaut.
*   **`run_one.sh`** : Le bloc atomique. Il exécute la simulation, calcule les métriques clés (nombre de photons diffusés), trie, puis trace **pour un seul jeu de paramètres donné**.
*   **`run_many.sh`** : Utilisé pour tester la stabilité. Il lance de multiples simulations avec des graines aléatoires (`seeds`) différentes pour vérifier la robustesse statistique.
*   **`sweep.sh`** : L'outil d'exploration. Il génère automatiquement des dizaines, voire des centaines, de fichiers de configuration temporaires, puis appelle `run_one.sh` pour tester systématiquement toutes les combinaisons de paramètres définies (ex: tous les angles contre tous les $\tau$).

### 🌐 4. Le Déploiement (Production / CD)
*   **`deploy.sh`** : Le pont vers l'infrastructure externe. Il construit l'image, l'importe dans un moteur de conteneur (via `k0s`/`ctr`), génère un manifest K8s, et lance le Job dans le cluster.
*   **`k8s/`** : Contient les schémas (`.template`) et les manifestes (`.yaml`) pour décrire l'exécution du simulateur dans Kubernetes.
*   **`docs/README_DEPLOYMENT_OVERVIEW.md`** : Documente en détail le fonctionnement de ce script.

### 📊 5. L'Analyse et la Visualisation (Post-Traitement)
Ces outils prennent les données brutes (`.csv`) et les transforment en connaissances exploitables.
*   **`generate_report_interactive.sh`** : Transforme les CSV en un rapport HTML dynamique, permettant de filtrer, zoomer et observer le bruit MC.
*   **`analyse_diff.py`** : Scripts Python pour des analyses statistiques avancées sur les résultats.
*   **`report_interactif.*`** : Les fichiers générés qui constituent le livrable final de l'analyse.

### 🗺️ Récapitulatif Visuel du Flux (Le Chemin de la Donnée)

$$\text{Config.toml} \xrightarrow{\text{run\_one.sh}} \text{simulate (Docker)} \rightarrow \text{CSV} \xrightarrow{\text{alpine}} \text{Tri} \rightarrow \text{plot (Docker)} \rightarrow \text{PNG}$$

$$\text{Config.template.toml} \xrightarrow{\text{sweep.sh}} \text{Configs multiples} \xrightarrow{\text{run\_one.sh}} \text{Résultats} \xrightarrow{\text{generate\_report\_interactive.sh}} \text{Rapport HTML}$$
