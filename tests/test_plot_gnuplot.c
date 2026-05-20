#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "plot_gnuplot.h"

/* ---------------------------------------------------------
   Buffer global pour capturer les écritures gnuplot
   --------------------------------------------------------- */
static char gp_buffer[4096];
static size_t gp_offset = 0;

/* ---------------------------------------------------------
   MOCK : popen()
   --------------------------------------------------------- */
FILE *__wrap_popen(const char *cmd, const char *mode)
{
    (void) cmd;
    (void) mode;

    gp_offset = 0;
    gp_buffer[0] = '\0';

    return stdout; /* pipe factice */
}

/* ---------------------------------------------------------
   MOCK : pclose()
   --------------------------------------------------------- */
int __wrap_pclose(FILE *f)
{
    (void) f;
    return 0;
}

/* ---------------------------------------------------------
   MOCK : fputs()
   --------------------------------------------------------- */
int fake_fputs(const char *s, FILE *stream)
{
    (void) stream;

    size_t n = strlen(s);
    if (gp_offset + n < sizeof(gp_buffer)) {
        memcpy(gp_buffer + gp_offset, s, n);
        gp_offset += n;
        gp_buffer[gp_offset] = '\0';
    }

    return 1; /* succès */
}

/* ---------------------------------------------------------
   Active fake_fputs dans plot_gnuplot.c
   --------------------------------------------------------- */
#define fputs fake_fputs

/* ---------------------------------------------------------
   INCLUSION APRÈS LES MOCKS — CRITIQUE !
   --------------------------------------------------------- */
#include "../src/plot_gnuplot.c"

/* ---------------------------------------------------------
   Test principal
   --------------------------------------------------------- */
static void test_plot_gnuplot_script(void **state)
{
    (void) state;

    Config cfg = {
        .theta_zero = 30.0, .sigma_e = 50.0, .radius = 10.0, .albedo = 0.9, .n_photons = 1000000
    };

    int ok = plot_with_gnuplot("input.csv", "output.png", &cfg);
    assert_true(ok);

    /* Vérifications */
    assert_non_null(strstr(gp_buffer, "set terminal pngcairo"));
    assert_non_null(strstr(gp_buffer, "set output 'output.png'"));
    assert_non_null(strstr(gp_buffer, "set datafile separator ','"));

    assert_non_null(strstr(gp_buffer, "set title"));
    assert_non_null(strstr(gp_buffer, "Distribution angulaire"));

    /* Vérification ASCII-safe des valeurs */
    assert_non_null(strstr(gp_buffer, ", 30"));
    assert_non_null(strstr(gp_buffer, ", 666.67"));
    assert_non_null(strstr(gp_buffer, ", 0.90"));
    assert_non_null(strstr(gp_buffer, ", 1000000"));

    assert_non_null(strstr(gp_buffer, "plot 'input.csv' using 1:3"));
}

/* ---------------------------------------------------------
   Tableau exporté
   --------------------------------------------------------- */
const struct CMUnitTest plot_gnuplot_tests[] = {
    cmocka_unit_test(test_plot_gnuplot_script),
};

const int plot_gnuplot_tests_count = sizeof(plot_gnuplot_tests) / sizeof(plot_gnuplot_tests[0]);
