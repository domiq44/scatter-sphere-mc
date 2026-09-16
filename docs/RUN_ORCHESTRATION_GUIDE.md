# 🎬 Guide d'Orchestration des Simulations (Scripts d'Exécution)

# 🌟 Introduction
Ce document est le guide opérationnel pour exécuter les simulations Monte-Carlo. Contrairement aux Dockerfiles qui *construisent* les outils, ces scripts (`run.sh`, `run_one.sh`, `run_many.sh`, `sweep.sh`) sont les **chefs d'orchestre** qui *utilisent* les outils construits pour faire tourner le processus physique complet : **Simulation $\rightarrow$ Traitement des données $\rightarrow$ Visualisation.**

Pour un novice, considérez ces scripts comme les **boutons de contrôle** de votre laboratoire virtuel.

---

## ⚙️ Vue d'Ensemble du Workflow de Simulation

Le processus standard dans ce projet est toujours le même, mais il est exécuté avec des niveaux de granularité différents :

$$\text{Configuration} \rightarrow \text{Simulation (Docker)} \rightarrow \text{Traitement (Alpine)} \rightarrow \text{Plotting (Docker)} \rightarrow \text{Résultat}$$

### 1. Le Cœur de l'Opération : `run_one.sh` (L'Expérience Unique)
C'est le bloc de construction atomique. Il exécute *une seule* simulation complète pour un jeu de paramètres donné.

**🎯 Ce qu'il fait :**
1.  **Préparation** : Il prend un fichier de configuration (`$CONFIG`) et un nom d'expérience (`$NAME`). Il crée un dossier de résultats dédié (`results/$NAME`).
2.  **Simulation** : Il lance le conteneur `scatter-sphere-simulate:local`. Le simulateur utilise le fichier de configuration passé en argument pour générer les données brutes (`.csv`) dans le dossier de résultats.
3.  **Analyse Physique** : Il exécute un script `alpine` (un système minimaliste) pour lire le CSV et **calculer le nombre de photons diffusés au moins une fois** (la métrique clé du bruit MC). Cette valeur est sauvegardée dans `stats.txt`.
4.  **Tri des Données** : Il utilise l'image `alpine` pour trier le CSV généré, en ordonnant les données par la première colonne (probablement l'angle ou un paramètre de mesure), de manière décroissante (`nr`).
5.  **Visualisation** : Il lance le conteneur `scatter-sphere-plot:local`, lui passant le fichier CSV trié. Ce conteneur utilise `gnuplot` pour générer le graphique PNG.
6.  **Notification** : Il affiche le chemin du résultat final.

**🧑‍🏫 Pour le Novice :** Si vous voulez tester *une seule* configuration spécifique (ex: Angle=30, Tau=0.1), c'est ce script que vous devez utiliser.

### 2. L'Automatisation Séquentielle : `run.sh` (Le Pipeline Standard)
Ce script est conçu pour exécuter le pipeline de base sans la complexité de la gestion des paramètres multiples.

**🎯 Ce qu'il fait :**
1.  **Simulation** : Il lance `scatter-sphere-simulate:local` pour générer le CSV.
2.  **Tri** : Il lance un conteneur `alpine` pour trier le CSV.
3.  **Tracé** : Il lance `scatter-sphere-plot:local` pour créer le PNG.
4.  **Affichage** : Il utilise `xdg-open` pour ouvrir automatiquement le PNG généré avec votre visionneuse d'images par défaut sur Fedora.

**🧑‍🏫 Pour le Novice :** Utilisez ce script quand vous voulez simplement voir le résultat d'une configuration par défaut sans vous soucier des détails de la configuration ou du nom du répertoire.

### 3. Le Test de Robustesse : `run_many.sh` (La Vérification de Stabilité)
Ce script est utilisé pour vérifier si la simulation est **statistiquement stable**.

**🎯 Ce qu'il fait :**
1.  Il initialise un fichier de log (`results/diffs.txt`).
2.  Il boucle un nombre prédéfini de fois (`N_RUNS=20`).
3.  À chaque itération, il appelle un sous-processus (probablement une fonction ou un script interne) pour lancer la simulation en utilisant un *seed* (graine aléatoire) différent.
4.  Il capture le résultat critique (le "différence MC") et enregistre : `[Numéro de la tentative] [Seed utilisé] [Résultat MC]`.

**🧑‍🏫 Pour le Novice :** Si vous exécutez ce script, vous vérifiez que, même en changeant la "graine" aléatoire (le point de départ du hasard), les résultats restent cohérents. Cela prouve la **reproductibilité** de votre moteur.

### 4. L'Exploration Paramétrique : `sweep.sh` (La Cartographie du Paramètre)
C'est l'outil le plus puissant pour la recherche scientifique. Il permet d'explorer rapidement une **grande plage de conditions expérimentales** sans avoir à lancer les scripts manuellement.

**🎯 Ce qu'il fait :**
1.  **Lecture des Paramètres** : Il lit les valeurs de référence (ex: `sigma_e`) depuis `config.template.toml`.
2.  **Définition des Grilles** : Il définit des listes de valeurs pour deux variables critiques : `ANGLES` (ex: 0°, 30°, 60°) et `TAUS` (ex: 0.01, 0.1, 1.0).
3.  **Calcul Dynamique** : Pour chaque combinaison d'Angle et de Tau, il calcule une valeur physique dérivée : le `RADIUS`.
4.  **Génération de Configurations** : Il utilise `sed` pour modifier le modèle de configuration (`config.template.toml`) en créant un fichier temporaire spécifique pour chaque combinaison (`config.angleXX_tauYY.toml`).
5.  **Exécution Séquentielle** : Il appelle ensuite **`run_one.sh`** pour exécuter l'expérience complète avec cette configuration spécifique.

**🧑‍🏫 Pour le Novice :** Si vous voulez savoir comment le résultat change lorsque vous modifiez l'angle **ET** le paramètre $\tau$, utilisez ce script. Il fait le travail répétitif de création de fichiers et d'appel de `run_one.sh` pour vous.

---

## 🗺️ Récapitulatif du Choix du Script

| Si vous voulez... | Utilisez ce script | Résultat attendu |
| :--- | :--- | :--- |
| **Voir un résultat simple et rapide** | `./run.sh` | Un fichier PNG affiché immédiatement. |
| **Tester une seule configuration précise** | `./run_one.sh config.toml nom` | Un dossier `results/nom` contenant le CSV, les stats, et le PNG. |
| **Vérifier la stabilité du hasard** | `./run_many.sh` | Un fichier `results/diffs.txt` montrant la cohérence des résultats. |
| **Explorer toutes les combinaisons** | `./sweep.sh` | De multiples dossiers dans `results/` (ex: `angle30_tau0.1`), chacun avec son propre graphique. |
