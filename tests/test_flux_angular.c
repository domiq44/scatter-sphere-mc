#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include "geometry.h"
#include "hg.h"
#include "rng.h"
#include "vec3.h"
#include <math.h>

/* Simule un photon et renvoie cos(theta) de la direction de sortie */
static double simulate_exit_mu(Rng *rng)
{
    double sigma_e = 1.0;
    double g = 0.0;
    double radius = 1.0;

    Vec3 pos = vec3_new(0, 0, 0);
    Vec3 dir = vec3_new(0, 0, -1);

    while (1) {

        double xi = rng_next_f64(rng);
        double free_path = -log(1.0 - xi) / sigma_e;

        double d_border = distance_to_sphere(pos, dir, radius);

        if (free_path >= d_border) {
            return dir.z; /* cos(theta) */
        }

        pos = vec3_add(pos, vec3_scale(dir, free_path));
        dir = sample_henyey_greenstein(rng, g, dir);
    }
}

/* Test : la distribution en cos(theta) doit être croissante (loi de Lambert) */
static void test_flux_angular_lambert(void **state)
{
    (void) state;

    const int bins = 10;
    double hist[bins];

    for (int i = 0; i < bins; i++)
        hist[i] = 0.0;

    Rng rng;
    rng_seed(&rng, 123);

    int N = 30000;

    for (int i = 0; i < N; i++) {
        double mu = simulate_exit_mu(&rng);
        if (mu < 0)
            continue; /* on ne garde que les photons sortant vers l'extérieur */
        int b = (int) (mu * bins);
        if (b >= bins)
            b = bins - 1;
        hist[b] += 1.0 / N;
    }

    /* Vérification : histogramme croissant */
    for (int i = 0; i < bins - 1; i++) {
        assert_true(hist[i] < hist[i + 1] + 0.01);
    }
}

const struct CMUnitTest flux_angular_tests[] = {
    cmocka_unit_test(test_flux_angular_lambert),
};

const int flux_angular_tests_count = sizeof(flux_angular_tests) / sizeof(flux_angular_tests[0]);
