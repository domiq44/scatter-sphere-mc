#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include "geometry.h"
#include "vec3.h"
#include <math.h>

/* Test : distance_to_sphere ne doit jamais renvoyer NaN */
static void test_no_nan_distance(void **state)
{
    (void) state;

    double radius = 1.0;

    for (int i = 0; i < 10000; i++) {
        Vec3 pos = vec3_new(0.999 * cos(i), 0.999 * sin(i), 0.001 * (i % 10));

        Vec3 dir = vec3_normalize(vec3_new(cos(i * 0.1), sin(i * 0.1), 0.1));

        double d = distance_to_sphere(pos, dir, radius);

        assert_false(isnan(d));
    }
}

const struct CMUnitTest stability_tests[] = {
    cmocka_unit_test(test_no_nan_distance),
};

const int stability_tests_count = sizeof(stability_tests) / sizeof(stability_tests[0]);
