#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include "geometry.h"
#include "hg.h"
#include "rng.h"
#include "vec3.h"
#include <math.h>

/* Simule un photon et renvoie le rayon de sortie */
static double simulate_exit_radius(Rng *rng)
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
            Vec3 exit_pos = vec3_add(pos, vec3_scale(dir, d_border));
            return sqrt(exit_pos.x * exit_pos.x + exit_pos.y * exit_pos.y);
        }

        pos = vec3_add(pos, vec3_scale(dir, free_path));
        dir = sample_henyey_greenstein(rng, g, dir);
    }
}

/* Compare deux histogrammes radiaux */
static double histogram_distance(const double *h1, const double *h2, int bins)
{
    double sum = 0.0;
    for (int i = 0; i < bins; i++)
        sum += fabs(h1[i] - h2[i]);
    return sum;
}

/* Test : le flux radial converge quand N augmente */
static void test_flux_radial_convergence(void **state)
{
    (void) state;

    const int bins = 10;
    double h_small[bins];
    double h_large[bins];

    for (int i = 0; i < bins; i++) {
        h_small[i] = 0.0;
        h_large[i] = 0.0;
    }

    /* Histogramme avec 5000 photons */
    {
        Rng rng;
        rng_seed(&rng, 111);

        int N = 5000;
        for (int i = 0; i < N; i++) {
            double r = simulate_exit_radius(&rng);
            int b = (int) (r * bins);
            if (b >= bins)
                b = bins - 1;
            h_small[b] += 1.0 / N;
        }
    }

    /* Histogramme avec 20000 photons */
    {
        Rng rng;
        rng_seed(&rng, 222);

        int N = 20000;
        for (int i = 0; i < N; i++) {
            double r = simulate_exit_radius(&rng);
            int b = (int) (r * bins);
            if (b >= bins)
                b = bins - 1;
            h_large[b] += 1.0 / N;
        }
    }

    /* Distance entre histogrammes */
    double dist = histogram_distance(h_small, h_large, bins);

    /* Les deux distributions doivent être proches */
    assert_true(dist < 0.15);
}

const struct CMUnitTest flux_radial_tests[] = {
    cmocka_unit_test(test_flux_radial_convergence),
};

const int flux_radial_tests_count = sizeof(flux_radial_tests) / sizeof(flux_radial_tests[0]);
