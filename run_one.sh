#!/usr/bin/env bash
set -e

CONFIG="$1"
NAME="$2"

if [ -z "$CONFIG" ] || [ -z "$NAME" ]; then
    echo "Usage: $0 config.toml run_name"
    exit 1
fi

OUTDIR="results/$NAME"
mkdir -p "$OUTDIR"

echo "=== Simulation ($NAME) ==="

# Capture la sortie du simulateur
SIM_OUTPUT=$(docker run --rm \
    -v "$(pwd)/$OUTDIR:/app/results:Z" \
    -v "$(pwd)/$CONFIG:/app/config.toml:ro,Z" \
    scatter-sphere-simulate:local)

echo "$SIM_OUTPUT"

# Trouve le CSV généré
CSV=$(ls -t "$OUTDIR"/*.csv | head -n 1)
BASENAME=$(basename "$CSV")

# === Calcul du nombre de photons diffusés ===
N_PHOTONS_DIFFUSED=$(awk -F, 'NR>1 && $3 > 0 {count++} END {print count}' "$CSV")
echo "$N_PHOTONS_DIFFUSED" > "$OUTDIR/stats.txt"

echo "Photons diffusés (au moins une fois) : $N_PHOTONS_DIFFUSED"

echo "=== Tri du CSV ==="
docker run --rm \
    -v "$(pwd)/$OUTDIR:/app/results:Z" \
    alpine sh -c "sort -t, -k1,1nr /app/results/$BASENAME -o /app/results/$BASENAME"

echo "=== Tracé ==="
docker run --rm \
    -v "$(pwd)/$OUTDIR:/app/results:Z" \
    -v "$(pwd)/$CONFIG:/app/results/config.toml:ro,Z" \
    -w /app/results \
    scatter-sphere-plot:local "$BASENAME"

echo "Résultat : $OUTDIR"
