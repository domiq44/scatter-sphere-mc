#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include "rng.h"

/* Test : la roulette russe est non biaisée */
static void test_roulette_unbiased(void **state)
{
    (void) state;

    Rng rng;
    rng_seed(&rng, 12345);

    const double w0 = 0.001; /* poids initial faible */
    const double p = 0.1; /* probabilité de survie */

    int N = 50000;
    double sum = 0.0;

    for (int i = 0; i < N; i++) {

        double w = w0;

        if (w < 1e-3) {
            double xi = rng_next_f64(&rng);
            if (xi < p)
                w /= p; /* survit */
            else
                w = 0.0; /* détruit */
        }

        sum += w;
    }

    double mean = sum / N;

    /* L'espérance doit être proche de w0 (±10%) */
    assert_true(mean > 0.9 * w0 && mean < 1.1 * w0);
}

/* Tableau de tests */
const struct CMUnitTest roulette_tests[] = {
    cmocka_unit_test(test_roulette_unbiased),
};

const int roulette_tests_count = sizeof(roulette_tests) / sizeof(roulette_tests[0]);
