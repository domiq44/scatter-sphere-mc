#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include "geometry.h"
#include "hg.h"
#include "rng.h"
#include "vec3.h"
#include <math.h>

/* Simule un photon et renvoie la longueur totale parcourue */
static double simulate_path_length(Rng *rng)
{
    double sigma_e = 1.0;
    double g = 0.0;
    double radius = 1.0;

    Vec3 pos = vec3_new(0, 0, 0);
    Vec3 dir = vec3_new(0, 0, -1);

    double total = 0.0;

    while (1) {

        double xi = rng_next_f64(rng);
        double free_path = -log(1.0 - xi) / sigma_e;

        double d_border = distance_to_sphere(pos, dir, radius);

        if (free_path >= d_border) {
            total += d_border;
            return total;
        }

        total += free_path;

        pos = vec3_add(pos, vec3_scale(dir, free_path));
        dir = sample_henyey_greenstein(rng, g, dir);
    }
}

/* Test 1 : la moyenne doit être proche de 1.5 */
static void test_path_length_mean(void **state)
{
    (void) state;

    Rng rng;
    rng_seed(&rng, 123);

    int N = 20000;
    double sum = 0.0;

    for (int i = 0; i < N; i++)
        sum += simulate_path_length(&rng);

    double mean = sum / N;

    /* tolérance large mais réaliste */
    assert_true(mean > 1.2 && mean < 1.8);
}

/* Test 2 : pas de valeurs aberrantes */
static void test_path_length_no_outliers(void **state)
{
    (void) state;

    Rng rng;
    rng_seed(&rng, 456);

    int N = 20000;

    for (int i = 0; i < N; i++) {
        double L = simulate_path_length(&rng);
        /* un photon ne peut pas parcourir 10 unités dans une sphère de rayon 1 */
        assert_true(L < 10.0);
    }
}

const struct CMUnitTest path_length_tests[] = {
    cmocka_unit_test(test_path_length_mean),
    cmocka_unit_test(test_path_length_no_outliers),
};

const int path_length_tests_count = sizeof(path_length_tests) / sizeof(path_length_tests[0]);
