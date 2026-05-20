#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include "geometry.h"
#include "vec3.h"
#include <math.h>

/* Test 1 : centre de la sphère */
static void test_dts_center(void **state)
{
    (void) state;

    Vec3 pos = vec3_new(0, 0, 0);
    Vec3 dir = vec3_new(1, 0, 0);
    double t = distance_to_sphere(pos, dir, 1.0);

    assert_float_equal(t, 1.0, 1e-12);
}

/* Test 2 : point intérieur */
static void test_dts_inside(void **state)
{
    (void) state;

    Vec3 pos = vec3_new(0.5, 0, 0);
    Vec3 dir = vec3_new(1, 0, 0);
    double t = distance_to_sphere(pos, dir, 1.0);

    assert_float_equal(t, 0.5, 1e-12);
}

/* Test 3 : direction oblique */
static void test_dts_oblique(void **state)
{
    (void) state;

    Vec3 pos = vec3_new(0, 0, 0);
    Vec3 dir = vec3_normalize(vec3_new(1, 1, 0));
    double t = distance_to_sphere(pos, dir, 1.0);

    assert_float_equal(t, 1.0, 1e-12);
}

/* Test 4 : point sur la surface */
static void test_dts_surface(void **state)
{
    (void) state;

    Vec3 pos = vec3_new(1, 0, 0);
    Vec3 dir = vec3_new(1, 0, 0);
    double t = distance_to_sphere(pos, dir, 1.0);

    assert_float_equal(t, 0.0, 1e-12);
}

/* Test 5 : point extérieur mais intersection */
static void test_dts_outside_hit(void **state)
{
    (void) state;

    Vec3 pos = vec3_new(2, 0, 0);
    Vec3 dir = vec3_new(-1, 0, 0);
    double t = distance_to_sphere(pos, dir, 1.0);

    /* La fonction renvoie la racine positive : t = 3 */
    assert_float_equal(t, 3.0, 1e-12);
}

/* Test 6 : pas d’intersection */
static void test_dts_no_hit(void **state)
{
    (void) state;

    Vec3 pos = vec3_new(2, 0, 0);
    Vec3 dir = vec3_new(1, 0, 0);
    double t = distance_to_sphere(pos, dir, 1.0);

    assert_float_equal(t, -1.0, 1e-12);
}

const struct CMUnitTest distance_tests[] = {
    cmocka_unit_test(test_dts_center),      cmocka_unit_test(test_dts_inside),
    cmocka_unit_test(test_dts_oblique),     cmocka_unit_test(test_dts_surface),
    cmocka_unit_test(test_dts_outside_hit), cmocka_unit_test(test_dts_no_hit),
};

const int distance_tests_count = sizeof(distance_tests) / sizeof(distance_tests[0]);
