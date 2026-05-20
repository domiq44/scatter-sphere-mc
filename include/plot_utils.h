#ifndef PLOT_UTILS_H
#define PLOT_UTILS_H

#include <stddef.h>

void replace_extension(const char* filename,
                       const char* old_ext,
                       const char* new_ext,
                       char* out,
                       size_t out_size);

#endif
