#include "gtdhelper.hpp"

// gettimeofday in windows
int gettimeofday(struct timeval *tv, struct timezone *tz)
{
  FILETIME ft;
  unsigned __int64 tmpres = 0;
  static int tzflag;

  if (NULL != tv)
  {
    // Get system time
    GetSystemTimeAsFileTime(&ft);

    // Make it an unsigned 64 bit
    tmpres |= ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;
    // 100nano -> 1micro,
    // and convert into epoch time
    tmpres -= DELTA_EPOCH_IN_MICROSECS;    

    // set sec, micorsec
    tv->tv_sec = (tmpres / 1000000UL);
    tv->tv_usec = (tmpres % 1000000UL);
  }

  // timezone
  if (NULL != tz)
  {
    if (!tzflag)
    {
      _tzset();
      tzflag++;
    }
    tz->tz_minuteswest = _timezone / 60;
    tz->tz_dsttime = _daylight;
  }

  return 0;
}

// source from http://spaurh.egloos.com/4569632