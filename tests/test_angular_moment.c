#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include "hg.h"
#include "rng.h"
#include "vec3.h"
#include <math.h>

/* applique n_scatter diffusions isotropes et renvoie la direction finale */
static Vec3 random_walk_direction(Rng *rng, int n_scatter)
{
    double g = 0.0; /* isotrope */

    Vec3 dir = vec3_new(0, 0, -1);

    for (int i = 0; i < n_scatter; i++)
        dir = sample_henyey_greenstein(rng, g, dir);

    return dir;
}

/* Test : le moment angulaire moyen doit être nul (⟨dir⟩ ≈ 0) */
static void test_angular_moment_zero(void **state)
{
    (void) state;

    Rng rng;
    rng_seed(&rng, 123);

    int N = 50000;
    int n_scatter = 5;

    double sx = 0.0, sy = 0.0, sz = 0.0;

    for (int i = 0; i < N; i++) {
        Vec3 d = random_walk_direction(&rng, n_scatter);
        sx += d.x;
        sy += d.y;
        sz += d.z;
    }

    sx /= N;
    sy /= N;
    sz /= N;

    /* tolérance raisonnable autour de 0 */
    assert_true(fabs(sx) < 0.02);
    assert_true(fabs(sy) < 0.02);
    assert_true(fabs(sz) < 0.02);
}

const struct CMUnitTest angular_moment_tests[] = {
    cmocka_unit_test(test_angular_moment_zero),
};

const int angular_moment_tests_count =
    sizeof(angular_moment_tests) / sizeof(angular_moment_tests[0]);
