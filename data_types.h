/**
 * ================================================
 * =        Harvard University | CS s165          =
 * ================================================
 * ==========     A1 DATA TYPES     ===============
 * forked from the CS165/265 generator by 
 * Kostas Zoumpatianos, Michael Kester
 * ================================================
 * Contact:
 * ========
 * - Wilson Qin <wilsonqin@seas.harvard.edu>
 */

#include <stdint.h>

#ifndef DATA_TYPES_H
#define DATA_TYPES_H


// KEY
typedef int32_t KEY_t;
typedef int32_t VAL_t;

#define KEY_MAX 2147483647
#define KEY_MIN -2147483647

#define GEN_RANDOM_KEY_GAUSS(r) gsl_ran_gaussian(r, 2147483647/3);
#define GEN_RANDOM_VAL_GAUSS(r) gsl_ran_gaussian(r, 2147483647/3);

#define GEN_RANDOM_KEY_UNIFORM(r) gsl_rng_get(r);
#define GEN_RANDOM_VAL_UNIFORM(r) gsl_rng_get(r);

// PRINT PATTERNS
#define PUT_PATTERN "p %d %d\n"
#define GET_PATTERN "g %d\n"
#define RANGE_PATTERN "r %d %d\n"
#define LOAD_PATTERN "l %s\n"


// SCAN PATTERNS
#define PUT_PATTERN_SCAN "%d %d"
#define GET_PATTERN_SCAN "%d"
#define RANGE_PATTERN_SCAN "%d %d"
#define LOAD_PATTERN_SCAN "%s"

#endif