#ifndef HG_H
#define HG_H

#include "vec3.h"
#include "rng.h"

Vec3 sample_henyey_greenstein(Rng *rng, double g, Vec3 dir);

#endif
