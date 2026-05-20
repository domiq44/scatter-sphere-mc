#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>

#include "plot_utils.h"

/* ---------------------------------------------------------
   Test : remplacement simple .csv -> .png
   --------------------------------------------------------- */
static void test_replace_extension_basic(void **state)
{
    (void) state;

    char out[256];
    replace_extension("data.csv", ".csv", ".png", out, sizeof(out));

    assert_string_equal(out, "data.png");
}

/* ---------------------------------------------------------
   Test : fichier sans extension -> ajout de .png
   --------------------------------------------------------- */
static void test_replace_extension_no_ext(void **state)
{
    (void) state;

    char out[256];
    replace_extension("result", ".csv", ".png", out, sizeof(out));

    assert_string_equal(out, "result.png");
}

/* ---------------------------------------------------------
   Test : extension différente -> ajout de .png
   --------------------------------------------------------- */
static void test_replace_extension_wrong_ext(void **state)
{
    (void) state;

    char out[256];
    replace_extension("image.txt", ".csv", ".png", out, sizeof(out));

    assert_string_equal(out, "image.txt.png");
}

/* ---------------------------------------------------------
   Test : buffer trop petit -> sortie tronquée mais valide
   --------------------------------------------------------- */
static void test_replace_extension_small_buffer(void **state)
{
    (void) state;

    char out[8];
    replace_extension("verylongfilename.csv", ".csv", ".png", out, sizeof(out));

    /* On ne teste pas la valeur exacte, juste que c'est bien terminé */
    assert_int_equal(out[sizeof(out) - 1], '\0');
}

/* ---------------------------------------------------------
   Tableau exporté
   --------------------------------------------------------- */
const struct CMUnitTest plot_utils_tests[] = {
    cmocka_unit_test(test_replace_extension_basic),
    cmocka_unit_test(test_replace_extension_no_ext),
    cmocka_unit_test(test_replace_extension_wrong_ext),
    cmocka_unit_test(test_replace_extension_small_buffer),
};

const int plot_utils_tests_count = sizeof(plot_utils_tests) / sizeof(plot_utils_tests[0]);
