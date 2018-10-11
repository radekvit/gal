#ifndef GAL_RAND_H
#define GAL_RAND_H
#include <cstddef>

size_t gal_rand(size_t upTo);
size_t gal_rand(size_t from, size_t upTo);

double gal_rand(double upTo);
double gal_rand(double from, double upTo);

#endif