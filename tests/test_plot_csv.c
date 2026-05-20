#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

#include "plot_csv.h"

/* ---------------------------------------------------------
   Test : data_init + data_push
   --------------------------------------------------------- */
static void test_data_push(void **state)
{
    (void) state;

    Data d;
    data_init(&d);

    assert_int_equal(d.len, 0);
    assert_int_equal(d.cap, 0);

    assert_true(data_push(&d, 0.1, 1.0));
    assert_true(data_push(&d, 0.2, 2.0));
    assert_true(data_push(&d, 0.3, 3.0));

    assert_int_equal(d.len, 3);
    assert_true(d.cap >= 3);

    assert_float_equal(d.mu[0], 0.1, 1e-12);
    assert_float_equal(d.pdf[0], 1.0, 1e-12);

    assert_float_equal(d.mu[2], 0.3, 1e-12);
    assert_float_equal(d.pdf[2], 3.0, 1e-12);

    data_free(&d);
}

/* ---------------------------------------------------------
   Test : read_csv sur un fichier temporaire
   --------------------------------------------------------- */
static void test_read_csv_valid(void **state)
{
    (void) state;

    const char *fname = "tmp_plotcsv_test.csv";
    FILE *f = fopen(fname, "w");
    assert_non_null(f);

    fprintf(f,
            "theta,mu,pdf\n"
            "0,0.1,1.0\n"
            "0,0.2,2.0\n"
            "0,0.3,3.0\n");
    fclose(f);

    Data d;
    assert_true(read_csv(fname, &d));

    assert_int_equal(d.len, 3);
    assert_float_equal(d.mu[1], 0.2, 1e-12);
    assert_float_equal(d.pdf[1], 2.0, 1e-12);

    data_free(&d);
    remove(fname);
}

/* ---------------------------------------------------------
   Test : read_csv sur fichier inexistant
   --------------------------------------------------------- */
static void test_read_csv_missing(void **state)
{
    (void) state;

    Data d;
    assert_false(read_csv("fichier_inexistant_123.csv", &d));
}

/* ---------------------------------------------------------
   Tableau de tests exporté
   --------------------------------------------------------- */
const struct CMUnitTest plot_csv_tests[] = {
    cmocka_unit_test(test_data_push),
    cmocka_unit_test(test_read_csv_valid),
    cmocka_unit_test(test_read_csv_missing),
};

const int plot_csv_tests_count = sizeof(plot_csv_tests) / sizeof(plot_csv_tests[0]);
