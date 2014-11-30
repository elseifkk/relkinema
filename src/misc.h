#ifndef MISC_H
#define MISC_H

#include <math.h>
#include <float.h>

static inline int BT(int var, int pos){ return var & (1<<pos);};
static inline int NIN(double x){return floor(0.5+x);};
static inline bool nearly_eq(double a, double b){return fabs(a-b)<DBL_EPSILON;};

#endif
