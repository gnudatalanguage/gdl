#include <time.h>
// A conversion constant between epoch time and microsecs
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif
/*!
 \brief gtdhelper.hpp for WIN32 or MINGW32
 */
#ifndef _TIMEZONE_DEFINED /* also in sys/time.h */
#define _TIMEZONE_DEFINED
struct timezone {
  int tz_minuteswest;
  int tz_dsttime;
};
#endif /* _TIMEZONE_DEFINED */

int gettimeofday(struct timeval *tv, struct timezone *tz);

// Source from http://spaurh.egloos.com/4569632