#include <math.h>
#include <stdio.h>

#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include "basis.h"
#include "hg.h"
#include "rng.h"
#include "vec3.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static void test_simulate_basic(void **state)
{

    Rng rng;
    rng_seed(&rng, 12345);

    double g = 0.85;
    double sigma_e = 5.0;
    double radius = 1.0;
    double theta_zero_deg = 30.0;

    double theta = theta_zero_deg * M_PI / 180.0;
    Vec3 initial_dir = vec3_normalize(vec3_new(sin(theta), 0.0, -cos(theta)));

    /* On vérifie que la direction initiale est bien normalisée */
    assert_float_equal(vec3_norm(initial_dir), 1.0, 1e-12);

    for (int i = 0; i < 100; i++) {

        Vec3 pos = vec3_new(0, 0, 0);
        (void) pos; /* évite un warning */

        Vec3 dir = initial_dir;

        /* Une seule diffusion */
        dir = sample_henyey_greenstein(&rng, g, dir);

        /* µ = cos(angle entre dir et initial_dir) */
        double mu = vec3_dot(dir, initial_dir);

        /* µ doit être dans [-1, 1] */
        assert_true(mu >= -1.0 && mu <= 1.0);
    }
}

/* Tableau de tests */
const struct CMUnitTest simulate_tests[] = {
    cmocka_unit_test(test_simulate_basic),
};

const int simulate_tests_count = sizeof(simulate_tests) / sizeof(simulate_tests[0]);
