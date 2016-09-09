/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_NAN
#define HE_NAN

#ifndef NAN
#include <limits>
#define NAN (std::numeric_limits<double>::quiet_NaN())
#endif

#if defined(__APPLE__) 
#include<cmath>
using std::isnan;
#endif

#if defined(_WIN32) && !defined(__MINGW32__)
static inline int isnan(double x){return x!=x;}
#endif

#endif

