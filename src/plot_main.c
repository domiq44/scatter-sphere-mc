#include "config.h"
#include "plot_csv.h"
#include "plot_gnuplot.h"
#include "plot_utils.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage : %s <fichier.csv>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *csv_filename = argv[1];

    Config cfg;
    if (!load_config("config.toml", &cfg)) {
        fprintf(stderr, "Erreur lors du chargement de config.toml\n");
        return EXIT_FAILURE;
    }

    Data data;
    if (!read_csv(csv_filename, &data)) {
        return EXIT_FAILURE;
    }
    data_free(&data);

    char png_filename[512];
    replace_extension(csv_filename, ".csv", ".png", png_filename, sizeof(png_filename));

    if (!plot_with_gnuplot(csv_filename, png_filename, &cfg)) {
        return EXIT_FAILURE;
    }

    printf("Graphique généré : %s\n", png_filename);
    return EXIT_SUCCESS;
}
