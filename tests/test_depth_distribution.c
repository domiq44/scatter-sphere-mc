#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include "geometry.h"
#include "hg.h"
#include "rng.h"
#include "vec3.h"
#include <math.h>

/* Simule un photon et renvoie la profondeur du premier événement */
static double simulate_first_depth(Rng *rng)
{
    double sigma_e = 50.0;
    double radius = 1.0;

    Vec3 pos = vec3_new(0, 0, 0);
    Vec3 dir = vec3_new(0, 0, -1);

    double xi = rng_next_f64(rng);
    double free_path = -log(1.0 - xi) / sigma_e;

    double d_border = distance_to_sphere(pos, dir, radius);

    if (free_path >= d_border)
        return d_border; /* sortie sans interaction */

    return free_path;
}

/* Test : la moyenne doit être proche de 1/sigma_e = 0.02 */
static void test_depth_mean(void **state)
{
    (void) state;

    Rng rng;
    rng_seed(&rng, 123);

    int N = 20000;
    double sum = 0.0;

    for (int i = 0; i < N; i++)
        sum += simulate_first_depth(&rng);

    double mean = sum / N;

    /* valeur théorique : 1/sigma_e = 0.02 */
    assert_true(mean > 0.015 && mean < 0.03);
}

/* Test : la distribution cumulée doit être proche de 1 - exp(-sigma_e * z) */
static void test_depth_cdf(void **state)
{
    (void) state;

    Rng rng;
    rng_seed(&rng, 456);

    int N = 20000;
    int count = 0;

    double sigma_e = 50.0;
    double z0 = 0.5; /* point de test */

    for (int i = 0; i < N; i++) {
        double d = simulate_first_depth(&rng);
        if (d < z0)
            count++;
    }

    double empirical = (double) count / N;
    double theoretical = 1.0 - exp(-sigma_e * z0);

    /* tolérance ±5% */
    assert_true(fabs(empirical - theoretical) < 0.05);
}

const struct CMUnitTest depth_tests[] = {
    cmocka_unit_test(test_depth_mean),
    cmocka_unit_test(test_depth_cdf),
};

const int depth_tests_count = sizeof(depth_tests) / sizeof(depth_tests[0]);
