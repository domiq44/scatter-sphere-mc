#define _GNU_SOURCE
#include "hg.h"
#include "basis.h"
#include "rng.h"
#include "vec3.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Vec3 sample_henyey_greenstein(Rng *rng, double g, Vec3 dir)
{
    double xi = rng_next_f64(rng);
    double cos_theta;

    if (fabs(g) < 1e-3) {
        cos_theta = 2.0 * xi - 1.0;
    } else {
        double s = (1.0 - g * g) / (1.0 - g + 2.0 * g * xi);
        s = s * s;
        cos_theta = (1.0 + g * g - s) / (2.0 * g);
    }

    if (cos_theta > 1.0)
        cos_theta = 1.0;
    if (cos_theta < -1.0)
        cos_theta = -1.0;

    double sin_theta = sqrt(fmax(0.0, 1.0 - cos_theta * cos_theta));
    double phi = 2.0 * M_PI * rng_next_f64(rng);

    double cos_phi = cos(phi);
    double sin_phi = sin(phi);

    Vec3 u, v, w;
    orthonormal_basis(dir, &u, &v, &w);

    Vec3 new_dir =
        vec3_add(vec3_add(vec3_scale(u, sin_theta * cos_phi), vec3_scale(v, sin_theta * sin_phi)),
                 vec3_scale(w, cos_theta));

    return vec3_normalize(new_dir);
}
