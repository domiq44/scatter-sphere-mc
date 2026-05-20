#include "geometry.h"
#include <math.h>

double distance_to_sphere(Vec3 pos, Vec3 dir, double radius)
{
    double b = vec3_dot(pos, dir);
    double c = vec3_dot(pos, pos) - radius * radius;

    double disc = b * b - c;
    if (disc < 0.0) {
        return -1.0;
    }

    return -b + sqrt(disc);
}
