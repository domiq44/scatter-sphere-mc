#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "basis.h"
#include "config.h"
#include "geometry.h"
#include "hg.h"
#include "rng.h"
#include "vec3.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define NBINS 200

int main(int argc, char **argv)
{
    // Charger la configuration
    Config cfg;
    if (!load_config("config.toml", &cfg)) {
        fprintf(stderr, "Erreur lors du chargement de config.toml\n");
        return 1;
    }

    int N = cfg.n_photons;
    double sigma_e = cfg.sigma_e;
    double albedo = cfg.albedo;
    double g = cfg.g;
    double radius = cfg.radius;
    int seed = cfg.seed;
    double theta0_deg = cfg.theta_zero;

    // Conversion de l'angle d'incidence en radians
    double theta0_rad = theta0_deg * M_PI / 180.0;
    double cos_theta0 = cos(theta0_rad);
    double sin_theta0 = sin(theta0_rad);

    if (argc > 1) {
        seed = atoi(argv[1]);
    }

    Rng rng;
    rng_seed(&rng, seed);

    // --- 1. DÉFINIR LA DIRECTION FIXE DU FAISCEAU PARALLÈLE (D) ---
    // Le faisceau arrive à un angle theta0_rad par rapport à l'axe Oz (vertical).
    // Nous fixons l'incidence dans le plan XZ (phi=0 dans le système de projection).
    Vec3 fixed_direction = vec3_normalize(vec3_new(sin_theta0, 0.0, -cos_theta0));

    // --- 2. PRÉPARATION DES SORTIES ---
    mkdir("results", 0755);

    char filename[256];
    snprintf(filename, sizeof(filename), "results/theta%.0f_radius%.1f_albedo%.2f_g%.2f_N%d.csv",
             cfg.theta_zero, cfg.radius, cfg.albedo, cfg.g, cfg.n_photons);

    // === TABLEAUX POUR STOCKER LES PHOTONS SORTANTS ===
    // N_detected peut être inférieur à N, donc on utilise N comme taille maximale
    double *theta_deg_arr = malloc(N * sizeof(double));
    double *mu_arr = malloc(N * sizeof(double));
    double *weight_arr = malloc(N * sizeof(double));
    int *bin_arr = malloc(N * sizeof(int));

    if (!theta_deg_arr || !mu_arr || !weight_arr || !bin_arr) {
        fprintf(stderr, "Erreur : allocation mémoire échouée\n");
        free(theta_deg_arr);
        free(mu_arr);
        free(weight_arr);
        free(bin_arr);
        return 1;
    }

    int n_detected = 0;

    // === HISTOGRAMME POUR PDF DE SORTIE ===
    double hist[NBINS] = { 0 };

    // === Compteurs énergie ===
    int n_rr_killed = 0;
    long long n_scattered = 0;
    double E_detected = 0.0;
    double E_rr = 0.0;
    double E_absorbed_direct = 0.0;
    double E_absorbed_corr = 0.0;

    // === SIMULATION ===
    for (int i = 0; i < N; i++) {

        // --- 1. GÉNÉRATION DU POINT D'ENTRÉE P_in (Uniformité sur la projection inclinée) ---

        // Phase 1: Génération dans le plan de projection (X'Y'Z')
        // rho uniforme dans [0, R]
        double rho = radius * rng_next_f64(&rng);
        // phi uniforme dans [0, 2pi]
        double phi = 2.0 * M_PI * rng_next_f64(&rng);

        double x_prime = rho * cos(phi);
        double y_prime = rho * sin(phi);
        double z_prime = 0.0;

        // Phase 2: Transformation vers le repère monde (XYZ)
        // x = x' cos(theta0) + z' sin(theta0)
        // y = y'
        // z = -x' sin(theta0) + z' cos(theta0)

        Vec3 pos = vec3_new(x_prime * cos_theta0, y_prime, -x_prime * sin_theta0);

        // La direction de propagation est fixe (le faisceau parallèle)
        Vec3 dir = fixed_direction;

        // --- 2. SIMULATION DE TRAJECTOIRE À PARTIR DE P_in ---

        double weight = 1.0;
        double local_absorbed = 0.0;

        while (1) {

            double xi = rng_next_f64(&rng);
            // Calcul de la distance libre (Free Path)
            double free_path = -log(1.0 - xi) / sigma_e;

            // Calcul de la distance jusqu'au bord de la sphère depuis le point actuel 'pos' dans la direction 'dir'
            double d_border = distance_to_sphere(pos, dir, radius);

            if (free_path >= d_border) {
                // --- PHOTON SORTANT ---
                Vec3 exit_pos = vec3_add(pos, vec3_scale(dir, d_border));
                (void) exit_pos;

                // Angle de sortie par rapport au faisceau incident (D)
                // ATTENTION: Si le faisceau est parallèle, l'angle de sortie est l'angle entre 'dir' et 'fixed_direction'.
                // Cependant, dans un simulateur de diffusion, on compare souvent l'angle de sortie avec l'axe Z (ou D)
                // Si vous voulez l'angle par rapport à la direction d'incidence fixe (fixed_direction):
                double mu = vec3_dot(dir, fixed_direction);
                if (mu > 1.0)
                    mu = 1.0;
                if (mu < -1.0)
                    mu = -1.0;

                // Si vous voulez l'angle par rapport à l'axe Z (convention standard) :
                // Vec3 z_axis = vec3_new(0, 0, 1);
                // double mu = vec3_dot(dir, z_axis); // Ceci est plus complexe car 'dir' n'est pas forcément dans le plan XZ

                // Nous conservons l'utilisation de 'mu' calculé par rapport à la direction incidente fixe,
                // car c'est la convention la plus cohérente avec l'injection.
                double theta = acos(mu);
                double theta_deg = theta * 180.0 / M_PI;

                // === BIN POUR PDF ===
                // La PDF est basée sur l'angle de sortie par rapport à la direction incidente fixe
                int bin = (int) ((mu + 1.0) * 0.5 * NBINS);
                if (bin < 0)
                    bin = 0;
                if (bin >= NBINS)
                    bin = NBINS - 1;

                hist[bin] += weight;

                // === STOCKAGE POUR PASS 2 ===
                theta_deg_arr[n_detected] = theta_deg;
                mu_arr[n_detected] = mu;
                weight_arr[n_detected] = weight;
                bin_arr[n_detected] = bin;

                n_detected++;
                E_detected += weight;

                break; // Photon sorti, passer au photon suivant
            }

            // --- PHOTON EN TRAJET ---
            pos = vec3_add(pos, vec3_scale(dir, free_path));

            if (weight < 1e-3) {
                double p = 0.1;
                if (rng_next_f64(&rng) < p) {
                    weight /= p;
                } else {
                    n_rr_killed++;
                    E_rr += weight;
                    break; // Photon tué par RR
                }
            }

            double old_weight = weight;
            weight *= albedo;
            local_absorbed += (old_weight - weight);

            // Diffusion
            dir = sample_henyey_greenstein(&rng, g, dir);
            n_scattered++;
        }

        // Gestion de l'absorption (Méthode de comptage)
        if (local_absorbed != 0.0) {
            double y = local_absorbed - E_absorbed_corr;
            double t = E_absorbed_direct + y;
            E_absorbed_corr = (t - E_absorbed_direct) - y;
            E_absorbed_direct = t;
        }
    }

    // === PASS 2 : NORMALISATION DE LA PDF ===
    double total = 0.0;
    for (int i = 0; i < NBINS; i++)
        total += hist[i];

    double pdf_exit[NBINS];
    for (int i = 0; i < NBINS; i++)
        pdf_exit[i] = (total > 0 ? hist[i] / total : 0.0);

    // === ÉCRITURE DU CSV FINAL ===
    FILE *f = fopen(filename, "w");
    if (!f) {
        fprintf(stderr, "Erreur : impossible d'ouvrir %s\n", filename);
        free(theta_deg_arr);
        free(mu_arr);
        free(weight_arr);
        free(bin_arr);
        return 1;
    }

    fprintf(f, "theta_deg,mu,pdf,weight\n");

    for (int i = 0; i < n_detected; i++) {
        // NOTE: Nous utilisons la PDF normalisée (pdf_exit[bin_arr[i]]) pour le graphique
        fprintf(f, "%f,%f,%f,%f\n", theta_deg_arr[i], mu_arr[i], pdf_exit[bin_arr[i]],
                weight_arr[i]);
    }

    fclose(f);

    printf("Fichier généré : %s\n", filename);

    free(theta_deg_arr);
    free(mu_arr);
    free(weight_arr);
    free(bin_arr);

    // === Bilan énergétique ===
    printf("\n=== Bilan énergétique ===\n");
    printf("Photons injectés : %d\n", N);

    printf("Photons sortis : %d (%.3f %%)\n", n_detected, 100.0 * n_detected / N);

    printf("Photons tués RR : %d (%.3f %%)\n", n_rr_killed, 100.0 * n_rr_killed / N);

    printf("Photons diffusés : %lld (%.3f %% par photon en moyenne)\n", n_scattered,
           100.0 * (double) n_scattered / N);

    // Bilan déduit
    double E_absorbed_deduced = (double) N - (E_detected + E_rr);

    printf("\n--- Énergies ---\n");
    printf("E_detected           = %.6f (%.3f %%)\n", E_detected, 100.0 * E_detected / N);

    printf("E_rr                 = %.6f (%.3f %%)\n", E_rr, 100.0 * E_rr / N);

    printf("E_absorbed (direct)  = %.12f (%.3f %%)\n", E_absorbed_direct,
           100.0 * E_absorbed_direct / N);

    printf("E_absorbed (déduit)  = %.12f (%.3f %%)\n", E_absorbed_deduced,
           100.0 * E_absorbed_deduced / N);

    // Total direct
    double E_total_direct = E_absorbed_direct + E_detected + E_rr;

    printf("\nE_total (direct)     = %.12f (doit être ≈ %d)\n", E_total_direct, N);
    printf("E_total (déduit)     = %.12f (doit être ≈ %d)\n",
           E_absorbed_deduced + E_detected + E_rr, N);

    // Écart direct - N
    double diff = E_total_direct - (double) N;
    printf("\nÉcart direct - N     = %.12f\n", diff);

    // === Résumé final (énergies) ===
    double pct_E_out = 100.0 * E_detected / N;
    double pct_E_abs = 100.0 * E_absorbed_direct / N;
    double pct_E_rr = 100.0 * E_rr / N;

    printf("\n=== Résumé ===\n");
    printf("Sortie %.3f %% | Absorption %.3f %% | RR %.3f %%\n", pct_E_out, pct_E_abs, pct_E_rr);

    // Ligne pour scripts
    printf("\nMC_DIFF %.12f\n", diff);

    if (n_detected == 0) {
        printf("\n⚠️  Aucun photon n'a atteint la surface.\n");
        printf("   La sphère est probablement trop opaque (tau = 4 * sigma_e * R / 3).\n");
    }

    return 0;
}
