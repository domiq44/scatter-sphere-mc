#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include "vec3.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static void test_initial_dir_zero(void **state)
{
    (void) state;

    double theta0 = 0.0;
    Vec3 d = vec3_normalize(vec3_new(sin(theta0), 0, -cos(theta0)));

    assert_float_equal(d.x, 0.0, 1e-12);
    assert_float_equal(d.y, 0.0, 1e-12);
    assert_float_equal(d.z, -1.0, 1e-12);
}

static void test_initial_dir_30deg(void **state)
{
    (void) state;

    double theta0 = M_PI / 6.0; /* 30° */
    Vec3 d = vec3_normalize(vec3_new(sin(theta0), 0, -cos(theta0)));

    assert_true(fabs(d.x - 0.5) < 1e-12);
    assert_true(fabs(d.z + sqrt(3) / 2) < 1e-12);
}

const struct CMUnitTest initial_dir_tests[] = {
    cmocka_unit_test(test_initial_dir_zero),
    cmocka_unit_test(test_initial_dir_30deg),
};

const int initial_dir_tests_count = sizeof(initial_dir_tests) / sizeof(initial_dir_tests[0]);
