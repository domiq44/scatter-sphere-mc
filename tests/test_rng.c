#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include "rng.h"

static void test_rng_values(void **state)
{
    Rng rng;
    rng_seed(&rng, 123);

    double a = rng_next_f64(&rng);
    double b = rng_next_f64(&rng);
    double c = rng_next_f64(&rng);

    assert_true(a >= 0.0 && a < 1.0);
    assert_true(b >= 0.0 && b < 1.0);
    assert_true(c >= 0.0 && c < 1.0);
}

static void test_rng_reproducibility(void **state)
{
    Rng r1, r2;
    rng_seed(&r1, 42);
    rng_seed(&r2, 42);

    for (int i = 0; i < 10; i++) {
        assert_float_equal(rng_next_f64(&r1), rng_next_f64(&r2), 0.0);
    }
}

const struct CMUnitTest rng_tests[] = {
    cmocka_unit_test(test_rng_values),
    cmocka_unit_test(test_rng_reproducibility),
};

const int rng_tests_count = sizeof(rng_tests) / sizeof(rng_tests[0]);
