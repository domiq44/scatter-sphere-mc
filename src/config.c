#include "config.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *trim(char *s)
{
    while (isspace((unsigned char) *s))
        s++;
    if (*s == 0)
        return s;
    char *end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char) *end))
        end--;
    end[1] = '\0';
    return s;
}

static int parse_int(const char *s, int *out)
{
    char *end;
    errno = 0;
    long value = strtol(s, &end, 10);
    if (end == s || *end != '\0' || errno == ERANGE || value < INT_MIN || value > INT_MAX)
        return 0;
    *out = (int)value;
    return 1;
}

static int parse_double(const char *s, double *out)
{
    char *end;
    errno = 0;
    double value = strtod(s, &end);
    if (end == s || *end != '\0' || errno == ERANGE)
        return 0;
    *out = value;
    return 1;
}

int load_config(const char *filename, Config *cfg)
{
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Erreur : impossible d'ouvrir %s\n", filename);
        return 0;
    }

    char line[256];
    int found = 0;

    while (fgets(line, sizeof(line), f)) {
        char *p = trim(line);
        if (*p == '\0' || *p == '#')
            continue;

        char *eq = strchr(p, '=');
        if (!eq)
            continue;

        *eq = '\0';
        char *key = trim(p);
        char *val = trim(eq + 1);

        if (strcmp(key, "n_photons") == 0) {
            if (!parse_int(val, &cfg->n_photons)) {
                fclose(f);
                fprintf(stderr, "Erreur : valeur invalide pour n_photons\n");
                return 0;
            }
            found++;
        } else if (strcmp(key, "sigma_e") == 0) {
            if (!parse_double(val, &cfg->sigma_e)) {
                fclose(f);
                fprintf(stderr, "Erreur : valeur invalide pour sigma_e\n");
                return 0;
            }
            found++;
        } else if (strcmp(key, "albedo") == 0) {
            if (!parse_double(val, &cfg->albedo)) {
                fclose(f);
                fprintf(stderr, "Erreur : valeur invalide pour albedo\n");
                return 0;
            }
            found++;
        } else if (strcmp(key, "g") == 0) {
            if (!parse_double(val, &cfg->g)) {
                fclose(f);
                fprintf(stderr, "Erreur : valeur invalide pour g\n");
                return 0;
            }
            found++;
        } else if (strcmp(key, "radius") == 0) {
            if (!parse_double(val, &cfg->radius)) {
                fclose(f);
                fprintf(stderr, "Erreur : valeur invalide pour radius\n");
                return 0;
            }
            found++;
        } else if (strcmp(key, "n_mu_bins") == 0) {
            if (!parse_int(val, &cfg->n_mu_bins)) {
                fclose(f);
                fprintf(stderr, "Erreur : valeur invalide pour n_mu_bins\n");
                return 0;
            }
            found++;
        } else if (strcmp(key, "seed") == 0) {
            if (!parse_int(val, &cfg->seed)) {
                fclose(f);
                fprintf(stderr, "Erreur : valeur invalide pour seed\n");
                return 0;
            }
            found++;
        } else if (strcmp(key, "theta_zero") == 0) {
            if (!parse_double(val, &cfg->theta_zero)) {
                fclose(f);
                fprintf(stderr, "Erreur : valeur invalide pour theta_zero\n");
                return 0;
            }
            found++;
        }
    }

    fclose(f);

    if (found < 8) {
        fprintf(stderr, "Erreur : paramètres manquants dans %s\n", filename);
        return 0;
    }

    return 1;
}
