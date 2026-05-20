#!/usr/bin/env bash
set -e

OUTFILE="report_interactif.html"

echo "=== Génération du rapport HTML interactif : $OUTFILE ==="

SIGMA_E=$(awk -F= '/^[[:space:]]*sigma_e[[:space:]]*=/{gsub(/[ \t]/, "", $2); print $2; exit}' config.template.toml 2>/dev/null || true)
if [[ -z "$SIGMA_E" ]]; then
  SIGMA_E=$(awk -F= '/^[[:space:]]*sigma_e[[:space:]]*=/{gsub(/[ \t]/, "", $2); print $2; exit}' config.toml 2>/dev/null || true)
fi
if [[ -z "$SIGMA_E" ]]; then
  SIGMA_E=50.0
fi

cat > "$OUTFILE" << 'EOF'
<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="UTF-8">
  <title>Rapport interactif – scatter-sphere-mc</title>

  <link rel="stylesheet" href="report_interactif.css">
</head>

<body>

  <h1>Rapport interactif – scatter-sphere-mc</h1>

  <div class="controls">
    <label>Angle :
      <select id="angleFilter"><option value="all">Tous</option></select>
    </label>

    <label>Tau :
      <select id="tauFilter"><option value="all">Tous</option></select>
    </label>
  </div>

  <div class="grid" id="grid">
EOF

# Boucle sur tous les PNG
while IFS= read -r PNG; do
  DIR=$(dirname "$PNG")
  BASEDIR=$(basename "$DIR")

  ANGLE=${BASEDIR#angle}
  ANGLE=${ANGLE%%_tau*}

  TAU=${BASEDIR#*_tau}

  # radius = tau * 3 / (4 * sigma_e)
  RADIUS=$(awk -v tau="$TAU" -v sigma="$SIGMA_E" 'BEGIN {printf "%.4f", tau * 3 / (4 * sigma)}')

  STATS_FILE="$DIR/stats.txt"
  if [[ -f "$STATS_FILE" ]]; then
      NDIFF=$(cat "$STATS_FILE")
  else
      NDIFF="?"
  fi

  CAPTION="angle = ${ANGLE}°, tau = ${TAU}, Ndiff = ${NDIFF}"

  cat >> "$OUTFILE" << EOF
    <div class="card" data-angle="$ANGLE" data-tau="$TAU">
      <img src="$PNG" alt="$CAPTION">
      <div class="caption">$CAPTION<br><code>$PNG</code></div>
    </div>
EOF
done < <(find results -type f -name "*.png" | sort)

cat >> "$OUTFILE" << 'EOF'
  </div>

  <div id="lightbox">
    <img id="lightbox-img" src="">
  </div>

  <script src="report_interactif.js"></script>
</body>
</html>
EOF

echo "Fini. Ouvre le fichier avec ton navigateur :"
echo "  file://$(pwd)/$OUTFILE"
