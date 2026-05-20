#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include "hg.h"
#include "rng.h"
#include "vec3.h"
#include <math.h>

/* Test : la moyenne de μ doit être ≈ g */
static void test_hg_mean_matches_g(void **state)
{
    (void) state;

    Rng rng;
    rng_seed(&rng, 12345);

    double g = 0.7;
    int N = 50000;

    double sum = 0.0;

    Vec3 dir0 = vec3_new(0, 0, 1);

    for (int i = 0; i < N; i++) {
        Vec3 d = sample_henyey_greenstein(&rng, g, dir0);
        sum += vec3_dot(d, dir0);
    }

    double mean = sum / N;

    /* tolérance ±0.03 */
    assert_true(mean > g - 0.03 && mean < g + 0.03);
}

/* Test : g = 0 → isotrope → moyenne ≈ 0 */
static void test_hg_isotropic(void **state)
{
    (void) state;

    Rng rng;
    rng_seed(&rng, 98765);

    double g = 0.0;
    int N = 50000;

    double sum = 0.0;
    Vec3 dir0 = vec3_new(0, 0, 1);

    for (int i = 0; i < N; i++) {
        Vec3 d = sample_henyey_greenstein(&rng, g, dir0);
        sum += vec3_dot(d, dir0);
    }

    double mean = sum / N;

    assert_true(mean > -0.03 && mean < 0.03);
}

const struct CMUnitTest hg_stats_tests[] = {
    cmocka_unit_test(test_hg_mean_matches_g),
    cmocka_unit_test(test_hg_isotropic),
};

const int hg_stats_tests_count = sizeof(hg_stats_tests) / sizeof(hg_stats_tests[0]);
