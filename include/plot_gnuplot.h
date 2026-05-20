#ifndef PLOT_GNUPLOT_H
#define PLOT_GNUPLOT_H

#include <stddef.h>
#include "config.h"

void build_gnuplot_script(char* out, size_t max,
                          const Config* cfg,
                          const char* csv_filename,
                          const char* png_filename);

int plot_with_gnuplot(const char* csv_filename,
                      const char* png_filename,
                      const Config* cfg);

#endif
