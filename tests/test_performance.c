#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include <math.h>
#include <time.h>

#include "geometry.h"
#include "hg.h"
#include "rng.h"
#include "vec3.h"

/* Simulation minimale d'un photon */
static void simulate_one(Rng *rng)
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

        if (free_path >= d_border)
            return; /* sortie */

        pos = vec3_add(pos, vec3_scale(dir, free_path));
        dir = sample_henyey_greenstein(rng, g, dir);
    }
}

static void test_performance(void **state)
{
    (void) state;

    const int N = 20000;

    Rng rng;
    rng_seed(&rng, 123);

    clock_t t0 = clock();

    for (int i = 0; i < N; i++)
        simulate_one(&rng);

    clock_t t1 = clock();

    double elapsed = (double) (t1 - t0) / CLOCKS_PER_SEC;

    /* Le test doit passer sur n'importe quelle machine raisonnable */
    assert_true(elapsed < 0.5);
}

const struct CMUnitTest performance_tests[] = {
    cmocka_unit_test(test_performance),
};

const int performance_tests_count = sizeof(performance_tests) / sizeof(performance_tests[0]);
