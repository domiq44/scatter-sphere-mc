#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include "geometry.h"
#include "hg.h"
#include "rng.h"
#include "vec3.h"
#include <math.h>

/* Simule un photon et renvoie le nombre d'interactions */
static int simulate_interaction_count(Rng *rng)
{
    double sigma_e = 1.0;
    double g = 0.0;
    double radius = 1.0;

    Vec3 pos = vec3_new(0, 0, 0);
    Vec3 dir = vec3_new(0, 0, -1);

    int count = 0;

    while (1) {

        double xi = rng_next_f64(rng);
        double free_path = -log(1.0 - xi) / sigma_e;

        double d_border = distance_to_sphere(pos, dir, radius);

        if (free_path >= d_border)
            return count; /* sortie */

        /* interaction */
        count++;

        pos = vec3_add(pos, vec3_scale(dir, free_path));
        dir = sample_henyey_greenstein(rng, g, dir);
    }
}

/* Test : P(N=0) ≈ e^-1 */
static void test_interaction_zero_probability(void **state)
{
    (void) state;

    Rng rng;
    rng_seed(&rng, 123);

    int N = 20000;
    int zero = 0;

    for (int i = 0; i < N; i++)
        if (simulate_interaction_count(&rng) == 0)
            zero++;

    double empirical = (double) zero / N;
    double theoretical = exp(-1.0);

    assert_true(fabs(empirical - theoretical) < 0.03);
}

/* Test : E[N] doit être raisonnable (pas de divergence) */
static void test_interaction_mean(void **state)
{
    (void) state;

    Rng rng;
    rng_seed(&rng, 456);

    int N = 20000;
    double sum = 0.0;

    for (int i = 0; i < N; i++)
        sum += simulate_interaction_count(&rng);

    double mean = sum / N;

    /* bornes larges mais réalistes */
    assert_true(mean > 0.4 && mean < 1.5);
}

const struct CMUnitTest interaction_tests[] = {
    cmocka_unit_test(test_interaction_zero_probability),
    cmocka_unit_test(test_interaction_mean),
};

const int interaction_tests_count = sizeof(interaction_tests) / sizeof(interaction_tests[0]);
