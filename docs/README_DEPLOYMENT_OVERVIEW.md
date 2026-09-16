# 🚀 README_DEPLOYMENT_OVERVIEW.md

# 🚀 Guide de Déploiement du Projet (Utilisation de `deploy.sh`)

Ce document explique en détail le rôle, le fonctionnement et le contexte du script `./deploy.sh`. Ce script est la passerelle entre le développement local (où vous codez) et l'environnement de production (où le système est réellement utilisé, ici via Kubernetes).

---

## 🎯 1. À Quoi Sert `./deploy.sh` ? (Le Rôle Global)

Le script `./deploy.sh` est l'outil de **mise en production (Deployment)**. Son rôle principal est de prendre les composants de votre projet (notamment l'exécutable de simulation) qui ont été construits et testés, et de les injecter dans un environnement de cluster Kubernetes (K8s) afin qu'ils puissent être exécutés de manière stable et automatisée.

En résumé, si le `build.sh` est responsable de **fabriquer** le produit, le `deploy.sh` est responsable de **livrer et faire tourner** ce produit dans le monde réel.

## ⚙️ 2. Qu'est-ce qu'il Fait, Étape par Étape ? (Le Workflow Détaillé)

Le script exécute une séquence d'opérations très spécifiques, qui peuvent être regroupées en quatre grandes phases : **Préparation, Construction, Déploiement, et Validation.**

### Phase 1 : Vérifications Initiales (Pré-requis)
Avant de commencer, le script vérifie si tous les outils nécessaires sont présents sur votre machine hôte (Fedora 44) :
*   `docker` : Pour construire l'image Docker.
*   `kubectl` : Le client officiel pour interagir avec votre cluster Kubernetes.
*   `envsubst` : Un outil pour remplacer des variables dans des fichiers de configuration (comme les noms d'images).
*   Il détermine également comment communiquer avec le moteur de conteneur (Containerd), que ce soit directement ou via `k0s`.

### Phase 2 : Construction de l'Artefact (Le Paquet)
Le script s'assure que la version de l'application est prête pour K8s :
1.  **Build Docker (`docker build`)** : Il utilise `Dockerfile.simulate` pour créer une image Docker de votre simulateur, nommée `scatter-sphere:local`.
2.  **Exportation (`docker save`)** : Il "sauvegarde" cette image Docker dans un fichier `.tar` (`scatter-sphere.tar`). Ceci est une méthode standard pour transférer une image Docker entre machines sans passer par un registre distant (comme Docker Hub).
3.  **Importation dans Containerd (`ctr images import`)** : Il utilise les commandes `ctr` (Containerd CLI) pour injecter cette image `.tar` directement dans le moteur de conteneur qui gère votre cluster Kubernetes.

### Phase 3 : Déploiement Kubernetes (La Mise en Ligne)
C'est le cœur de l'automatisation :
1.  **Génération du Manifeste (`envsubst`)** : Il prend un modèle de configuration Kubernetes (`k8s/job.yaml.template`) et remplace les placeholders (comme `<IMAGE_NAME>`) par les valeurs réelles (ex: `scatter-sphere:local`). Le résultat est le fichier `k8s/job.yaml`.
2.  **Application (`kubectl apply`)** : Il envoie ce fichier de configuration au cluster Kubernetes. Il demande au cluster de créer un **Job** (une tâche) qui va lancer le conteneur avec l'image que vous venez d'importer.

### Phase 4 : Validation et Rapport (Le Bilan)
Un déploiement n'est réussi que si le travail a été fait. Le script attend et rapporte :
1.  **Attente du Pod (`kubectl wait`)** : Il surveille le cluster jusqu'à ce que le Job soit marqué comme `Complete`.
2.  **Extraction des Logs (`kubectl logs`)** : Il récupère les journaux de l'exécution pour vérifier si le simulateur s'est exécuté sans erreur. Il tente même d'extraire un fichier de sortie spécifique ("Fichier généré").
3.  **Journalisation (`RUN_LOG`)** : Il enregistre toutes les métriques importantes (statut final, durée totale, nom du Pod, fichier généré) dans un fichier `runs.log`.
4.  **Nettoyage (`kubectl delete`)** : Une fois le rapport généré, il supprime la tâche (Job) pour éviter qu'elle ne tourne indéfiniment et pour garder votre cluster propre.

---

## 💡 3. Son Rôle dans le Workflow Global (La Place dans le Cycle)

Imaginez un cycle de vie :

1.  **Développement** $\rightarrow$ (Vous modifiez le code)
2.  **Test Local** $\rightarrow$ (`make tests`) : *Est-ce que ça marche sur ma machine ?*
3.  **Validation CI/CD** $\rightarrow$ (`./build.sh`) : *Est-ce que ça marche dans un environnement Docker parfait et est-ce que le code est propre ?*
4.  **Production** $\rightarrow$ **`./deploy.sh`** : *Maintenant que c'est validé, faisons-le tourner pour que les utilisateurs puissent l'utiliser.*

**Conclusion :** `./deploy.sh` est l'étape finale qui prend les artefacts stables de la phase de construction (`./build.sh`) et les déploie dans l'infrastructure cible (Kubernetes) pour une utilisation réelle, tout en assurant une traçabilité complète grâce au journal de bord.

---
*Ce fichier est destiné aux développeurs et aux ingénieurs DevOps qui gèrent le passage de la phase de test à la phase de production.*
