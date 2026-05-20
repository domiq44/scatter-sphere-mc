#!/usr/bin/env bash
set -e

mkdir -p results

echo "=== Simulation ==="
docker run --rm \
    -v "$(pwd)/results:/app/results:Z" \
    scatter-sphere-simulate:local

CSV=$(ls -t results/*.csv | head -n 1)
BASENAME=$(basename "$CSV")
echo "CSV : $CSV"

echo "=== Tri du CSV ==="
docker run --rm \
    -v "$(pwd)/results:/app/results:Z" \
    alpine sh -c "sort -t, -k1,1nr /app/results/$BASENAME -o /app/results/$BASENAME"

echo "=== Tracé ==="
docker run --rm \
    -v "$(pwd)/results:/app/results:Z" \
    -v "$(pwd)/config.toml:/app/results/config.toml:ro,Z" \
    -w /app/results \
    scatter-sphere-plot:local "$BASENAME"

PNG="${CSV%.csv}.png"
echo "Graphique : $PNG"

if command -v xdg-open >/dev/null; then
    xdg-open "$PNG" &
fi
