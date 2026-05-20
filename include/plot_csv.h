#ifndef PLOT_CSV_H
#define PLOT_CSV_H

#include <stddef.h>

typedef struct {
    double* mu;
    double* pdf;
    size_t len;
    size_t cap;
} Data;

void data_init(Data* d);
void data_free(Data* d);
int  data_push(Data* d, double mu, double pdf);
int  read_csv(const char* filename, Data* data);

#endif
