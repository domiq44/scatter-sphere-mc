#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include "hg.h"
#include "rng.h"
#include "vec3.h"

static void test_hg_normalization(void **state)
{
    Rng rng;
    rng_seed(&rng, 123);

    Vec3 dir = vec3_new(0, 0, 1);

    for (int i = 0; i < 100; i++) {
        Vec3 d = sample_henyey_greenstein(&rng, 0.5, dir);
        assert_float_equal(vec3_norm(d), 1.0, 1e-6);
    }
}

static void test_hg_mean_mu(void **state)
{
    Rng rng;
    rng_seed(&rng, 42);

    Vec3 dir = vec3_new(0, 0, 1);
    double g = 0.85;

    double sum = 0.0;
    int N = 20000;

    for (int i = 0; i < N; i++) {
        Vec3 d = sample_henyey_greenstein(&rng, g, dir);
        sum += vec3_dot(dir, d);
    }

    double mu_mean = sum / N;

    assert_true(mu_mean > g - 0.05);
    assert_true(mu_mean < g + 0.05);
}

const struct CMUnitTest hg_tests[] = {
    cmocka_unit_test(test_hg_normalization),
    cmocka_unit_test(test_hg_mean_mu),
};

const int hg_tests_count = sizeof(hg_tests) / sizeof(hg_tests[0]);
