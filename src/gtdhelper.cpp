#include "gtdhelper.hpp"

// gettimeofday in windows
int gettimeofday(struct timeval *tv, struct timezone *tz)
{
  FILETIME ft;
  union {
  struct {
      unsigned long Low;
      unsigned long High;
      } u;
  unsigned __int64 tmpres;
  };
  static int tzflag;

  if (NULL != tv)
  {
    // Get system time
    GetSystemTimeAsFileTime(&ft);

    // Make it an unsigned 64 bit
    u.High = ft.dwHighDateTime;
    u.Low  = ft.dwLowDateTime;
    // 100nano -> 1micro,
    // and convert into epoch time
    tmpres = tmpres/10UL - DELTA_EPOCH_IN_MICROSECS; 

    // set sec, microsec
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