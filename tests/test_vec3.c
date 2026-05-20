#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include "vec3.h"

static void test_vec3_add(void **state)
{
    Vec3 a = vec3_new(1, 2, 3);
    Vec3 b = vec3_new(4, 5, 6);
    Vec3 c = vec3_add(a, b);

    assert_float_equal(c.x, 5.0, 1e-12);
    assert_float_equal(c.y, 7.0, 1e-12);
    assert_float_equal(c.z, 9.0, 1e-12);
}

static void test_vec3_dot(void **state)
{
    Vec3 a = vec3_new(1, 0, 0);
    Vec3 b = vec3_new(0, 1, 0);
    assert_float_equal(vec3_dot(a, b), 0.0, 1e-12);
}

static void test_vec3_cross(void **state)
{
    Vec3 a = vec3_new(1, 0, 0);
    Vec3 b = vec3_new(0, 1, 0);
    Vec3 c = vec3_cross(a, b);

    assert_float_equal(c.x, 0.0, 1e-12);
    assert_float_equal(c.y, 0.0, 1e-12);
    assert_float_equal(c.z, 1.0, 1e-12);
}

static void test_vec3_normalize(void **state)
{
    Vec3 v = vec3_new(3, 0, 0);
    Vec3 n = vec3_normalize(v);

    assert_float_equal(n.x, 1.0, 1e-12);
    assert_float_equal(n.y, 0.0, 1e-12);
    assert_float_equal(n.z, 0.0, 1e-12);
}

const struct CMUnitTest vec3_tests[] = {
    cmocka_unit_test(test_vec3_add),
    cmocka_unit_test(test_vec3_dot),
    cmocka_unit_test(test_vec3_cross),
    cmocka_unit_test(test_vec3_normalize),
};

const int vec3_tests_count = sizeof(vec3_tests) / sizeof(vec3_tests[0]);
