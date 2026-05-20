#!/usr/bin/env bash
set -e

# Mode clean ?
if [[ "$1" == "clean" ]]; then
    BUILD_OPTS="--no-cache"
    TAG_SUFFIX="-debug"
    echo "=== Mode CLEAN activé ==="
else
    BUILD_OPTS=""
    TAG_SUFFIX=""
fi

echo "=== [1/4] Build simulate ==="
docker build $BUILD_OPTS -f Dockerfile.simulate -t scatter-sphere-simulate:local$TAG_SUFFIX .

echo "=== [2/4] Build plot ==="
docker build $BUILD_OPTS -f Dockerfile.plot -t scatter-sphere-plot:local$TAG_SUFFIX .

echo "=== [3/4] Build tests ==="
docker build $BUILD_OPTS -f Dockerfile.tests -t scatter-sphere-tests:local$TAG_SUFFIX .

echo "=== [4/4] Run tests ==="
docker run --rm scatter-sphere-tests:local$TAG_SUFFIX

echo "=== ✔ Build terminé avec succès ==="
