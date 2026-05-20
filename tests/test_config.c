#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"

/* Utilitaire : crée un fichier temporaire avec un contenu donné */
static char *write_temp_config(const char *content)
{
    char template[] = "/tmp/scatter-config.XXXXXX";
    int fd = mkstemp(template);
    assert_true(fd >= 0);

    FILE *f = fdopen(fd, "w");
    assert_non_null(f);
    fputs(content, f);
    fclose(f);
    return strdup(template);
}

/* Test : lecture d’un fichier config valide */
static void test_config_valid(void **state)
{
    (void) state;

    const char *cfg_text = "n_photons = 100000\n"
                           "sigma_e = 5.0\n"
                           "albedo = 0.9\n"
                           "g = 0.85\n"
                           "radius = 1.0\n"
                           "n_mu_bins = 200\n"
                           "seed = 12345\n"
                           "theta_zero = 30.0\n";

    char *fname = write_temp_config(cfg_text);

    Config cfg;
    int ok = load_config(fname, &cfg);

    /* load_config() retourne 1 en cas de succès */
    assert_int_equal(ok, 1);

    assert_int_equal(cfg.n_photons, 100000);
    assert_float_equal(cfg.sigma_e, 5.0, 1e-12);
    assert_float_equal(cfg.albedo, 0.9, 1e-12);
    assert_float_equal(cfg.g, 0.85, 1e-12);
    assert_float_equal(cfg.radius, 1.0, 1e-12);
    assert_int_equal(cfg.n_mu_bins, 200);
    assert_int_equal(cfg.seed, 12345);
    assert_float_equal(cfg.theta_zero, 30.0, 1e-12);

    remove(fname);
    free(fname);
}

/* Test : fichier invalide */
static void test_config_invalid(void **state)
{
    (void) state;

    const char *cfg_text = "n_photons = abc\n" /* invalide */
                           "sigma_e = 5.0\n";

    char *fname = write_temp_config(cfg_text);

    Config cfg;
    int ok = load_config(fname, &cfg);

    /* load_config() retourne 0 en cas d’erreur */
    assert_int_equal(ok, 0);

    remove(fname);
    free(fname);
}

/* Tableau de tests */
const struct CMUnitTest config_tests[] = {
    cmocka_unit_test(test_config_valid),
    cmocka_unit_test(test_config_invalid),
};

const int config_tests_count = sizeof(config_tests) / sizeof(config_tests[0]);
