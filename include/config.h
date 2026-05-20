#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
    int    n_photons;
    double sigma_e;
    double albedo;
    double g;
    double radius;
    int    n_mu_bins;
    int    seed;
    double theta_zero;
} Config;

int load_config(const char *filename, Config *cfg);

#endif
