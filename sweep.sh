#!/usr/bin/env bash
set -e

ANGLES=(0 30 60 90)
TAUS=(0.01 0.02 0.05 0.1 0.2 0.5 1 2 5 10 20 50 100)

SIGMA_E=$(awk -F= '/^[[:space:]]*sigma_e[[:space:]]*=/{gsub(/[ \t]/, "", $2); print $2; exit}' config.template.toml)
if [[ -z "$SIGMA_E" ]]; then
  echo "Erreur : impossible de lire sigma_e depuis config.template.toml" >&2
  exit 1
fi

for ANG in "${ANGLES[@]}"; do
  for TAU in "${TAUS[@]}"; do

    NAME="angle${ANG}_tau${TAU}"
    CONFIG_TMP="config.${NAME}.toml"

    # radius = tau / (4/3 * sigma_e) = tau * 3 / (4 * sigma_e)
    RADIUS=$(awk -v tau="$TAU" -v sigma="$SIGMA_E" 'BEGIN {printf "%.8f", tau * 3 / (4 * sigma)}')

    echo "=== Génération config pour $NAME (radius=$RADIUS) ==="

    sed \
      -e "s/{{ANGLE}}/$ANG/g" \
      -e "s/{{TAU}}/$TAU/g" \
      -e "s/{{RADIUS}}/$RADIUS/g" \
      config.template.toml > "$CONFIG_TMP"

    ./run_one.sh "$CONFIG_TMP" "$NAME"
  done
done
