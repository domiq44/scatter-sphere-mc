#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include <math.h>

#include "geometry.h"
#include "hg.h"
#include "rng.h"
#include "vec3.h"

/* Simulation minimale d’un photon pour tester la conservation */
static double run_single_photon(Rng *rng)
{
    double sigma_e = 1.0;
    double albedo = 1.0; /* aucune absorption */
    double g = 0.0; /* diffusion isotrope */
    double radius = 1.0;

    Vec3 pos = vec3_new(0, 0, 0);
    Vec3 dir = vec3_new(0, 0, -1);

    double weight = 1.0;

    while (1) {

        /* libre parcours */
        double xi = rng_next_f64(rng);
        double free_path = -log(1.0 - xi) / sigma_e;

        /* distance au bord */
        double d_border = distance_to_sphere(pos, dir, radius);

        /* sortie */
        if (free_path >= d_border) {
            return weight;
        }

        /* interaction */
        pos = vec3_add(pos, vec3_scale(dir, free_path));

        /* pas d’absorption car albedo = 1 */

        /* diffusion isotrope */
        dir = sample_henyey_greenstein(rng, g, dir);
    }
}

/* Test : conservation de l’énergie */
static void test_energy_conservation(void **state)
{
    (void) state;

    Rng rng;
    rng_seed(&rng, 12345);

    int N = 2000;
    double sum_w = 0.0;

    for (int i = 0; i < N; i++) {
        sum_w += run_single_photon(&rng);
    }

    double mean_w = sum_w / N;

    /* tolérance ±5% */
    assert_true(mean_w > 0.95 && mean_w < 1.05);
}

/* Tableau de tests */
const struct CMUnitTest conservation_tests[] = {
    cmocka_unit_test(test_energy_conservation),
};

const int conservation_tests_count = sizeof(conservation_tests) / sizeof(conservation_tests[0]);
