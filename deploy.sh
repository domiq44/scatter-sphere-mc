#!/usr/bin/env bash
set -euo pipefail

START_TIME=$(date +%s)

# ============================
# Configuration
# ============================
PROJECT_HOME="$(pwd)"
IMAGE_NAME="scatter-sphere:local"
IMAGE_TAR="scatter-sphere.tar"
K8S_TEMPLATE="k8s/job.yaml.template"
K8S_RENDERED="k8s/job.yaml"
RUN_LOG="runs.log"
JOB_NAME="scatter-sphere-job"
RESULTS_DIR="$PROJECT_HOME/results"

cleanup() {
    if [[ -n "${JOB_APPLIED:-}" ]]; then
        kubectl delete -f "$K8S_RENDERED" --ignore-not-found=true >/dev/null 2>&1 || true
    fi
}
trap cleanup EXIT

check_command() {
    if ! command -v "$1" >/dev/null 2>&1; then
        echo "Erreur : commande introuvable -> $1" >&2
        exit 1
    fi
}

check_command docker
check_command kubectl
check_command envsubst
check_command date

find_k0s_containerd_socket() {
    if [[ -S "/var/run/k0s/containerd.sock" ]]; then
        echo "/var/run/k0s/containerd.sock"
        return 0
    elif [[ -S "/var/lib/k0s/run/containerd.sock" ]]; then
        echo "/var/lib/k0s/run/containerd.sock"
        return 0
    fi
    return 1
}

if command -v k0s >/dev/null 2>&1; then
    if SOCKET=$(find_k0s_containerd_socket); then
        CTR_CMD=(k0s ctr --address "$SOCKET")
    else
        CTR_CMD=(k0s ctr)
    fi
elif command -v ctr >/dev/null 2>&1; then
    CTR_CMD=(ctr)
else
    echo "Erreur : ni k0s ni ctr n'est installé" >&2
    exit 1
fi

SUDO_CMD=()
if [[ -n "${SOCKET:-}" ]] && ([[ ! -r "$SOCKET" ]] || [[ ! -w "$SOCKET" ]]); then
    if command -v sudo >/dev/null 2>&1; then
        echo "⚠️  Accès au socket containerd restreint, utilisation de sudo pour ${CTR_CMD[*]}"
        SUDO_CMD=(sudo)
    else
        echo "Erreur : accès refusé à $SOCKET. Exécutez le script en root ou installez sudo." >&2
        exit 1
    fi
fi

mkdir -p "$RESULTS_DIR"

echo "📦 Build de l'image Docker..."
docker build -f Dockerfile.simulate -t "$IMAGE_NAME" .

echo "📤 Export de l'image en tar..."
docker save -o "$IMAGE_TAR" "$IMAGE_NAME"

echo "📥 Import dans containerd via ${SUDO_CMD[*]} ${CTR_CMD[*]}..."
"${SUDO_CMD[@]}" "${CTR_CMD[@]}" images import "$IMAGE_TAR"

echo "📝 Génération du manifest Kubernetes..."
export PROJECT_HOME
export IMAGE_NAME
envsubst < "$K8S_TEMPLATE" > "$K8S_RENDERED"

echo "☸️ Déploiement du Job..."
kubectl delete -f "$K8S_RENDERED" --ignore-not-found=true
kubectl apply -f "$K8S_RENDERED"
JOB_APPLIED=1

echo "⏳ Attente du Pod..."
while true; do
    POD=$(kubectl get pods -l job-name="$JOB_NAME" -o jsonpath='{.items[0].metadata.name}' 2>/dev/null || true)
    if [[ -n "$POD" ]]; then
        break
    fi
    echo "  → Pod pas encore créé, attente..."
    sleep 1
done

kubectl wait --for=condition=complete job/"$JOB_NAME" --timeout=300s

echo "📄 Logs du Pod :"
POD_LOGS=$(kubectl logs "$POD")
echo "$POD_LOGS"

GENERATED_FILE=$(echo "$POD_LOGS" | grep "Fichier généré" | sed 's/.*: //') || true

POD_STATUS=$(kubectl get pod "$POD" -o jsonpath='{.status.phase}')
POD_START=$(kubectl get pod "$POD" -o jsonpath='{.status.startTime}')
POD_END=$(kubectl get pod "$POD" -o jsonpath='{.status.containerStatuses[0].state.terminated.finishedAt}')

echo "📌 Statut final du Pod : $POD_STATUS"
echo "⏱ Durée du Pod : $POD_START → $POD_END"

echo -e "\n📜 Historique des exécutions $JOB_NAME :"
kubectl get events --sort-by=.metadata.creationTimestamp \
  | grep -E "$JOB_NAME.*(Created|Started|Completed)" || true

END_TIME=$(date +%s)
TOTAL_TIME=$((END_TIME - START_TIME))

echo -e "\n📘 Résumé du run"
echo "Pod : $POD"
echo "Durée du Pod : $POD_START → $POD_END"
echo "Durée totale : ${TOTAL_TIME}s"
echo "Statut : $POD_STATUS"
echo "Fichier généré : ${GENERATED_FILE:-<non trouvé>}"

printf '%s | Pod %s | %s | %ss | %s\n' "$(date '+%Y-%m-%d %H:%M:%S')" "$POD" "$POD_STATUS" "$TOTAL_TIME" "${GENERATED_FILE:-<non trouvé>}" >> "$RUN_LOG"

echo "🧹 Nettoyage..."
kubectl delete -f "$K8S_RENDERED" --ignore-not-found=true

echo "🎉 Déploiement terminé."
