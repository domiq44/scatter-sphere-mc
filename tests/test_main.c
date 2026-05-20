#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

/* Déclarations externes des tableaux de tests */
extern const struct CMUnitTest vec3_tests[];
extern const int vec3_tests_count;

extern const struct CMUnitTest rng_tests[];
extern const int rng_tests_count;

extern const struct CMUnitTest basis_tests[];
extern const int basis_tests_count;

extern const struct CMUnitTest hg_tests[];
extern const int hg_tests_count;

extern const struct CMUnitTest simulate_tests[];
extern const int simulate_tests_count;

extern const struct CMUnitTest config_tests[];
extern const int config_tests_count;

extern const struct CMUnitTest distance_tests[];
extern const int distance_tests_count;

extern const struct CMUnitTest conservation_tests[];
extern const int conservation_tests_count;

extern const struct CMUnitTest roulette_tests[];
extern const int roulette_tests_count;

extern const struct CMUnitTest hg_stats_tests[];
extern const int hg_stats_tests_count;

extern const struct CMUnitTest initial_dir_tests[];
extern const int initial_dir_tests_count;

extern const struct CMUnitTest stability_tests[];
extern const int stability_tests_count;

extern const struct CMUnitTest convergence_tests[];
extern const int convergence_tests_count;

extern const struct CMUnitTest flux_radial_tests[];
extern const int flux_radial_tests_count;

extern const struct CMUnitTest depth_tests[];
extern const int depth_tests_count;

extern const struct CMUnitTest performance_tests[];
extern const int performance_tests_count;

extern const struct CMUnitTest interaction_tests[];
extern const int interaction_tests_count;

extern const struct CMUnitTest flux_angular_tests[];
extern const int flux_angular_tests_count;

extern const struct CMUnitTest path_length_tests[];
extern const int path_length_tests_count;

extern const struct CMUnitTest angular_moment_tests[];
extern const int angular_moment_tests_count;

extern const struct CMUnitTest variance_mc_tests[];
extern const int variance_mc_tests_count;

extern const struct CMUnitTest plot_csv_tests[];
extern const int plot_csv_tests_count;

extern const struct CMUnitTest plot_utils_tests[];
extern const int plot_utils_tests_count;

extern const struct CMUnitTest plot_gnuplot_tests[];
extern const int plot_gnuplot_tests_count;

int main(void)
{

    const int total = vec3_tests_count + rng_tests_count + basis_tests_count + hg_tests_count +
        simulate_tests_count + config_tests_count + distance_tests_count +
        conservation_tests_count + roulette_tests_count + hg_stats_tests_count +
        initial_dir_tests_count + stability_tests_count + convergence_tests_count +
        flux_radial_tests_count + depth_tests_count + performance_tests_count +
        interaction_tests_count + flux_angular_tests_count + path_length_tests_count +
        angular_moment_tests_count + variance_mc_tests_count + plot_csv_tests_count +
        plot_utils_tests_count + plot_gnuplot_tests_count;

    struct CMUnitTest all_tests[total];
    int k = 0;

    for (int i = 0; i < vec3_tests_count; i++)
        all_tests[k++] = vec3_tests[i];
    for (int i = 0; i < rng_tests_count; i++)
        all_tests[k++] = rng_tests[i];
    for (int i = 0; i < basis_tests_count; i++)
        all_tests[k++] = basis_tests[i];
    for (int i = 0; i < hg_tests_count; i++)
        all_tests[k++] = hg_tests[i];
    for (int i = 0; i < simulate_tests_count; i++)
        all_tests[k++] = simulate_tests[i];
    for (int i = 0; i < config_tests_count; i++)
        all_tests[k++] = config_tests[i];
    for (int i = 0; i < distance_tests_count; i++)
        all_tests[k++] = distance_tests[i];
    for (int i = 0; i < conservation_tests_count; i++)
        all_tests[k++] = conservation_tests[i];
    for (int i = 0; i < roulette_tests_count; i++)
        all_tests[k++] = roulette_tests[i];
    for (int i = 0; i < hg_stats_tests_count; i++)
        all_tests[k++] = hg_stats_tests[i];
    for (int i = 0; i < initial_dir_tests_count; i++)
        all_tests[k++] = initial_dir_tests[i];
    for (int i = 0; i < stability_tests_count; i++)
        all_tests[k++] = stability_tests[i];
    for (int i = 0; i < convergence_tests_count; i++)
        all_tests[k++] = convergence_tests[i];
    for (int i = 0; i < flux_radial_tests_count; i++)
        all_tests[k++] = flux_radial_tests[i];
    for (int i = 0; i < depth_tests_count; i++)
        all_tests[k++] = depth_tests[i];
    for (int i = 0; i < performance_tests_count; i++)
        all_tests[k++] = performance_tests[i];
    for (int i = 0; i < interaction_tests_count; i++)
        all_tests[k++] = interaction_tests[i];
    for (int i = 0; i < flux_angular_tests_count; i++)
        all_tests[k++] = flux_angular_tests[i];
    for (int i = 0; i < path_length_tests_count; i++)
        all_tests[k++] = path_length_tests[i];
    for (int i = 0; i < angular_moment_tests_count; i++)
        all_tests[k++] = angular_moment_tests[i];
    for (int i = 0; i < variance_mc_tests_count; i++)
        all_tests[k++] = variance_mc_tests[i];
    for (int i = 0; i < plot_csv_tests_count; i++)
        all_tests[k++] = plot_csv_tests[i];
    for (int i = 0; i < plot_utils_tests_count; i++)
        all_tests[k++] = plot_utils_tests[i];
    for (int i = 0; i < plot_gnuplot_tests_count; i++)
        all_tests[k++] = plot_gnuplot_tests[i];

    return cmocka_run_group_tests(all_tests, NULL, NULL);
}
