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
        return 1;
    }

    int N = cfg.n_photons;
    double sigma_e = cfg.sigma_e;
    double albedo = cfg.albedo;
    double g = cfg.g;
    double radius = cfg.radius;
    int seed = cfg.seed;
    double theta0 = cfg.theta_zero * M_PI / 180.0;

    if (argc > 1) {
        seed = atoi(argv[1]);
    }

    Rng rng;
    rng_seed(&rng, seed);

    Vec3 initial_dir = vec3_normalize(vec3_new(sin(theta0), 0.0, -cos(theta0)));

    mkdir("results", 0755);

    char filename[256];
    snprintf(filename, sizeof(filename), "results/theta%.0f_radius%.1f_albedo%.2f_g%.2f_N%d.csv",
             cfg.theta_zero, cfg.radius, cfg.albedo, cfg.g, cfg.n_photons);

    // === TABLEAUX POUR STOCKER LES PHOTONS SORTANTS ===
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

        Vec3 pos = vec3_new(0, 0, 0);
        Vec3 dir = initial_dir;

        double weight = 1.0;
        double local_absorbed = 0.0;

        while (1) {

            double xi = rng_next_f64(&rng);
            double free_path = -log(1.0 - xi) / sigma_e;

            double d_border = distance_to_sphere(pos, dir, radius);

            if (free_path >= d_border) {

                Vec3 exit_pos = vec3_add(pos, vec3_scale(dir, d_border));
                (void) exit_pos;

                // === ANGLE DE SORTIE PAR RAPPORT AU FAISCEAU INCIDENT ===
                double mu = vec3_dot(dir, initial_dir);
                if (mu > 1.0)
                    mu = 1.0;
                if (mu < -1.0)
                    mu = -1.0;

                double theta = acos(mu);
                double theta_deg = theta * 180.0 / M_PI;

                // === BIN POUR PDF ===
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

                break;
            }

            pos = vec3_add(pos, vec3_scale(dir, free_path));

            if (weight < 1e-3) {
                double p = 0.1;
                if (rng_next_f64(&rng) < p) {
                    weight /= p;
                } else {
                    n_rr_killed++;
                    E_rr += weight;
                    break;
                }
            }

            double old_weight = weight;
            weight *= albedo;
            local_absorbed += (old_weight - weight);

            dir = sample_henyey_greenstein(&rng, g, dir);
            n_scattered++;
        }

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
        fprintf(f, "%f,%f,%f,%f\n", theta_deg_arr[i], mu_arr[i],
                pdf_exit[bin_arr[i]], // <<< VRAIE PDF DE SORTIE
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
