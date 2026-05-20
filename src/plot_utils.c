#include "plot_utils.h"
#include <string.h>

void replace_extension(const char *filename, const char *old_ext, const char *new_ext, char *out,
                       size_t out_size)
{
    size_t len = strlen(filename);
    size_t old_len = strlen(old_ext);

    if (len >= old_len && strcmp(filename + len - old_len, old_ext) == 0) {
        size_t base_len = len - old_len;
        if (base_len + strlen(new_ext) + 1 > out_size) {
            strncpy(out, filename, out_size - 1);
            out[out_size - 1] = '\0';
            return;
        }
        memcpy(out, filename, base_len);
        strcpy(out + base_len, new_ext);
    } else {
        if (len + strlen(new_ext) + 1 > out_size) {
            strncpy(out, filename, out_size - 1);
            out[out_size - 1] = '\0';
            return;
        }
        strcpy(out, filename);
        strcat(out, new_ext);
    }
}
