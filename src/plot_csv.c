#include "plot_csv.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void data_init(Data *d)
{
    d->mu = NULL;
    d->pdf = NULL;
    d->len = 0;
    d->cap = 0;
}

void data_free(Data *d)
{
    free(d->mu);
    free(d->pdf);
    d->mu = NULL;
    d->pdf = NULL;
    d->len = d->cap = 0;
}

int data_push(Data *d, double mu, double pdf)
{
    if (d->len == d->cap) {
        size_t new_cap = d->cap == 0 ? 128 : d->cap * 2;
        double *new_mu = realloc(d->mu, new_cap * sizeof(double));
        double *new_pdf = realloc(d->pdf, new_cap * sizeof(double));

        if (!new_mu || !new_pdf) {
            free(new_mu);
            free(new_pdf);
            return 0;
        }

        d->mu = new_mu;
        d->pdf = new_pdf;
        d->cap = new_cap;
    }

    d->mu[d->len] = mu;
    d->pdf[d->len] = pdf;
    d->len++;
    return 1;
}

int read_csv(const char *filename, Data *data)
{
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Erreur: impossible d'ouvrir %s\n", filename);
        return 0;
    }

    data_init(data);

    char line[512];
    size_t line_no = 0;

    while (fgets(line, sizeof(line), f)) {
        line_no++;

        if (line_no == 1)
            continue;

        line[strcspn(line, "\r\n")] = 0;

        char *theta_str = strtok(line, ",");
        char *mu_str = strtok(NULL, ",");
        char *pdf_str = strtok(NULL, ",");

        if (!theta_str || !mu_str || !pdf_str)
            continue;

        double mu = strtod(mu_str, NULL);
        double pdf = strtod(pdf_str, NULL);

        if (!data_push(data, mu, pdf)) {
            fprintf(stderr, "Erreur: allocation memoire\n");
            fclose(f);
            data_free(data);
            return 0;
        }
    }

    fclose(f);
    return 1;
}
