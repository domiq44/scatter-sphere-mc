#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include "geometry.h"
#include "hg.h"
#include "rng.h"
#include "vec3.h"
#include <math.h>

/* Simulation minimale : renvoie 1 si le photon sort par z > 0 */
static int simulate_exit_top(Rng *rng)
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

        if (free_path >= d_border) {
            /* sortie */
            Vec3 exit_pos = vec3_add(pos, vec3_scale(dir, d_border));
            return exit_pos.z > 0 ? 1 : 0;
        }

        pos = vec3_add(pos, vec3_scale(dir, free_path));
        dir = sample_henyey_greenstein(rng, g, dir);
    }
}

/* Test : la convergence doit suivre 1/sqrt(N) */
static void test_convergence(void **state)
{
    (void) state;

    int Ns[3] = { 1000, 5000, 20000 };
    double results[3];

    for (int k = 0; k < 3; k++) {

        Rng rng;
        rng_seed(&rng, 1234 + k);

        int N = Ns[k];
        int count = 0;

        for (int i = 0; i < N; i++)
            count += simulate_exit_top(&rng);

        results[k] = (double) count / N;
    }

    /* On vérifie que les estimations se rapprochent */
    double diff1 = fabs(results[0] - results[2]);
    double diff2 = fabs(results[1] - results[2]);

    /* L'estimation avec 5000 photons doit être plus proche que celle avec 1000 */
    assert_true(diff2 < diff1);
}

const struct CMUnitTest convergence_tests[] = {
    cmocka_unit_test(test_convergence),
};

const int convergence_tests_count = sizeof(convergence_tests) / sizeof(convergence_tests[0]);
