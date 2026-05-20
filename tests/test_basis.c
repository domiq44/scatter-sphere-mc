#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include "basis.h"
#include "vec3.h"

static void test_basis_orthonormal(void **state)
{
    Vec3 dir = vec3_normalize(vec3_new(0.3, 0.4, -0.866));

    Vec3 u, v, w;
    orthonormal_basis(dir, &u, &v, &w);

    assert_float_equal(vec3_dot(u, v), 0.0, 1e-6);
    assert_float_equal(vec3_dot(u, w), 0.0, 1e-6);
    assert_float_equal(vec3_dot(v, w), 0.0, 1e-6);

    assert_float_equal(vec3_norm(u), 1.0, 1e-6);
    assert_float_equal(vec3_norm(v), 1.0, 1e-6);
    assert_float_equal(vec3_norm(w), 1.0, 1e-6);
}

static void test_basis_special_case(void **state)
{
    Vec3 dir = vec3_new(0, 0, 1);

    Vec3 u, v, w;
    orthonormal_basis(dir, &u, &v, &w);

    assert_float_equal(vec3_norm(u), 1.0, 1e-6);
    assert_float_equal(vec3_norm(v), 1.0, 1e-6);
    assert_float_equal(vec3_norm(w), 1.0, 1e-6);
}

const struct CMUnitTest basis_tests[] = {
    cmocka_unit_test(test_basis_orthonormal),
    cmocka_unit_test(test_basis_special_case),
};

const int basis_tests_count = sizeof(basis_tests) / sizeof(basis_tests[0]);
