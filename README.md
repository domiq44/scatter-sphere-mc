# ⚛️ ScatterSphereMC — Simulateur de Transport Radiatif Monte-Carlo

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Langage: C](https://img.shields.io/badge/language-C-blue.svg)](https://github.com/domiq44/scatter-sphere-mc/blob/main/README.md)
[![Plateforme](https://img.shields.io/badge/platform-Linux-green.svg)](https://docs.fedora.org/en/latest/user-guide/overview.html)
[![Testé sur Fedora](https://img.shields.io/badge/tested%20on-Fedora%2040+-blue)](https://docs.fedora.org/en/latest/)
[![Tests Unitaires](https://img.shields.io/badge/tests-46%20passed-brightgreen)](./tests/)
[![Docker Support](https://img.shields.io/badge/Docker-supported-blue)](./build.sh)
[![CI/CD](https://github.com/domiq44/scatter-sphere-mc/actions/workflows/ci.yaml/badge.svg)](https://github.com/domiq44/scatter-sphere-mc/actions/workflows/ci.yaml)
[![Kubernetes](https://img.shields.io/badge/Kubernetes-supported-blue)](./deploy.sh)
[![k0s Support](https://img.shields.io/badge/k0s-local%20cluster-green)](./deploy.sh)

**ScatterSphereMC** est un simulateur Monte‑Carlo robuste et scientifiquement validé pour le transport radiatif dans une **sphère diffusante homogène**. Conçu pour être **simple, lisible, physiquement correct, reproductible, et pédagogique**.

---

## 🧭 Vue d'Ensemble du Projet (La Carte Maîtresse)

Ce projet est structuré en plusieurs couches logiques. Chaque fichier a un rôle précis :
*   **Code C (`src/`, `include/`)** : Le cœur physique du simulateur.
*   **Outils de Construction (`Makefile`, `build.sh`, `Dockerfile.*`)** : Ils prennent le code C et le transforment en exécutables isolés (via Docker).
*   **Scripts d'Orchestration (`run.sh`, `run_one.sh`, `sweep.sh`, `run_many.sh`)** : Ils dirigent les exécutables pour réaliser une expérience complète (Simuler $\rightarrow$ Traiter $\rightarrow$ Tracer).
*   **Déploiement (`deploy.sh`)** : Il prend les artefacts construits et les déploie dans un environnement Kubernetes pour une utilisation en production.
*   **Documentation (`docs/`)** : Contient les guides détaillés pour chaque niveau d'usage (Construction, Déploiement, Physique).

---

## 🚀 Guide d'Utilisation Rapide (Comment faire ?)

Choisissez votre objectif pour savoir quel script lancer :

### 1. 🔬 Pour le Développement Local (Code & Tests Rapides)
Utilisez le `Makefile`. Il compile directement sur votre machine Fedora.
```bash
make all  # Compile simulateur et plot
make tests # Lance les tests unitaires avec cmocka
```
*(Pour plus de détails sur le rôle du Makefile, consultez `docs/BUILD_GUIDE.md`)*.

### 2. 🐳 Pour la Construction Standardisée (CI/CD)
Utilisez `./build.sh`. Ceci crée les images Docker pour les trois composants (Simulate, Plot, Tests) en garantissant une reproductibilité parfaite.

### 3. 🏃 Pour une Simulation Standard (Pipeline complet)
Utilisez `./run.sh`. Ce script exécute le flux de travail standard : **Simulate $\rightarrow$ Traiter (Tri) $\rightarrow$ Plot**. Le résultat PNG est affiché automatiquement.

### 4. 🔬 Pour l'Analyse Paramétrique (Recherche)
Utilisez `./sweep.sh`. Ce script est le plus avancé : il teste automatiquement **toutes les combinaisons** de paramètres définis dans `config.template.toml` (Angles vs $\tau$), exécutant `run_one.sh` pour chaque cas.

### 5. 🌐 Pour le Déploiement en Production (CD)
Utilisez `./deploy.sh`. Ce script automatise le déploiement sur votre cluster Kubernetes local (`k0s`), garantissant que l'application tourne dans un environnement isolé.
*(Pour plus de détails sur ce processus, consultez `docs/README_DEPLOYMENT_OVERVIEW.md`)*.

---

## 🧠 Fonctions Avancées et Validation Scientifique

### 📊 Analyse du Bruit Monte-Carlo
Chaque simulation produit un fichier `stats.txt` dans le dossier de résultats, contenant le **nombre de photons diffusés au moins une fois**. C'est la mesure essentielle du bruit.

**Visualisation Avancée :**
Pour analyser ces données de manière interactive, utilisez :
```bash
./generate_report_interactive.sh
```
Ceci génère `report_interactif.html`, un rapport web filtrable et zoomable.

### 🧪 Tests Unitaires
Le répertoire `tests/` contient 46 tests couvrant la géométrie, la physique (conservation de l'énergie), le RNG, et la stabilité numérique. Le succès de ces tests valide la robustesse de votre moteur.

### 📡 Déploiement Kubernetes
Le script `./deploy.sh` utilise les fichiers dans `k8s/` pour décrire à Kubernetes comment exécuter votre simulation de manière fiable en tant que Job.

---

## 📚 Références Documentaires

Pour comprendre le *pourquoi* derrière chaque commande, consultez les guides suivants dans le répertoire `docs/` :

*   **`docs/BUILD_GUIDE.md`** : Comment construire et tester.
*   **`docs/README_DEPLOYMENT_OVERVIEW.md`** : Comment déployer sur Kubernetes.
*   **`docs/RUN_ORCHESTRATION_GUIDE.md`** : Détail complet de ce que font `run.sh`, `sweep.sh`, etc.
*   **`docs/modele_impact_projection_inclinee.md`** : Documentation physique sur la méthode d'injection des photons.

---

## 🛣️ Feuille de Route (Roadmap)

*   Support multi‑couches.
*   Modèle Mie simplifié.
*   Support OpenMP pour parallélisation accrue.
*   Histogrammes 2D ($\theta$, $\phi$).
*   Interface Web minimaliste.
*   Packaging Podman Desktop.
