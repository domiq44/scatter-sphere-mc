# ---- STAGE 0 : Builder ----
FROM fedora:44 AS builder

### RUN dnf upgrade -y
### gnuplot
RUN dnf install -y \
        gcc \
        make \
        cmake \
        git \
        wget \
        tar \
    && dnf clean all

WORKDIR /app
COPY . .

RUN make clean && make plot

# ---- STAGE 1 : Runtime ----
FROM fedora:44 AS runtime

# 🔥 Installer gnuplot + locales UTF‑8
###RUN dnf upgrade -y
RUN dnf install -y gnuplot glibc-langpack-en && \
    dnf clean all

# 🔥 Activer UTF‑8 dans le conteneur
ENV LANG=en_US.UTF-8
ENV LANGUAGE=en_US.UTF-8
ENV LC_ALL=en_US.UTF-8
ENV LC_CTYPE=en_US.UTF-8

WORKDIR /app

# 🔥 Créer le dossier results dans l’image
RUN mkdir -p /app/results && chmod 777 /app/results

COPY --from=builder /app/plot /plot

ENTRYPOINT ["/plot"]
