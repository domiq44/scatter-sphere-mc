#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include "rng.h"
#include <math.h>

/* observable : libre parcours exponentiel pur */
static double sample_free_path(Rng *rng)
{
    double xi = rng_next_f64(rng);
    return -log(1.0 - xi); /* sigma_e = 1 */
}

/* variance de la moyenne via méthode des blocs */
static double block_variance(int N, int B, uint64_t seed)
{
    Rng rng;
    rng_seed(&rng, seed);

    int block_size = N / B;
    double block_means[B];

    for (int b = 0; b < B; b++) {
        double sum = 0.0;
        for (int i = 0; i < block_size; i++)
            sum += sample_free_path(&rng);
        block_means[b] = sum / block_size;
    }

    double mean = 0.0;
    for (int b = 0; b < B; b++)
        mean += block_means[b];
    mean /= B;

    double var = 0.0;
    for (int b = 0; b < B; b++)
        var += (block_means[b] - mean) * (block_means[b] - mean);
    var /= B;

    return var;
}

/* Test : la variance de la moyenne doit décroître comme 1/N */
static void test_variance_mc(void **state)
{
    (void) state;

    int N1 = 200000; /* 2e5 */
    int N2 = 800000; /* 8e5 */
    int B = 20; /* 20 blocs */

    double var1 = block_variance(N1, B, 123);
    double var2 = block_variance(N2, B, 456);

    double ratio = var1 / var2;

    /* ratio attendu ≈ 4 */
    assert_true(ratio > 2.5 && ratio < 5.5);
}

const struct CMUnitTest variance_mc_tests[] = {
    cmocka_unit_test(test_variance_mc),
};

const int variance_mc_tests_count = sizeof(variance_mc_tests) / sizeof(variance_mc_tests[0]);
