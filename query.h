#ifndef QUERY_H
#define QUERY_H

#include "data_types.h"

/* Query Helper Data types */

// CONTEXT for results returned from a range query
typedef struct rangeResult {
    KEY_t *keys; // holds the qualifying keys 
    VAL_t *vals; // holds the qualifying values
} RANGE_RESULT_t;

#endif