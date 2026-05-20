#include "plot_gnuplot.h"
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef UNIT_TESTING
#define fprintf fake_fprintf
#endif

void build_gnuplot_script(char *out, size_t max, const Config *cfg, const char *csv_filename,
                          const char *png_filename)
{
    double tau = 4.0 / 3.0 * cfg->sigma_e * cfg->radius;
    char *old_locale = setlocale(LC_NUMERIC, NULL);
    char *saved_locale = NULL;

    if (old_locale) {
        saved_locale = strdup(old_locale);
    }
    setlocale(LC_NUMERIC, "C");

    snprintf(out, max,
             "set terminal pngcairo size 800,600 enhanced\n"
             "set output '%s'\n"
             "set datafile separator ','\n"
             "set title sprintf('Distribution angulaire (θ₀ = %.0f°, τ = %.2f, albedo = %.2f, N = "
             "%d)', "
             "%.0f, %.2f, %.2f, %d)\n"
             "set xlabel 'θ (degrés)'\n"
             "set ylabel 'pdf(θ)'\n"
             "set logscale y 10\n"
             "set format y \"10^{%%T}\"\n"
             "set grid xtics ytics\n"
             "plot '%s' using 1:3 with lines lw 2 lc rgb 'blue' title 'pdf(θ)'\n",
             png_filename, cfg->theta_zero, tau, cfg->albedo, cfg->n_photons, cfg->theta_zero, tau,
             cfg->albedo, cfg->n_photons, csv_filename);

    if (saved_locale) {
        setlocale(LC_NUMERIC, saved_locale);
        free(saved_locale);
    }
}

int plot_with_gnuplot(const char *csv_filename, const char *png_filename, const Config *cfg)
{
    char script[4096];
    build_gnuplot_script(script, sizeof(script), cfg, csv_filename, png_filename);

    FILE *gp = popen("gnuplot -persistent", "w");
    if (!gp) {
        fprintf(stderr, "Erreur: impossible de lancer gnuplot\n");
        return 0;
    }

    fputs(script, gp);
    fflush(gp);
    pclose(gp);
    return 1;
}
