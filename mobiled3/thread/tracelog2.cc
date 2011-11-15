//
//
//

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#if defined (_WIN32)
  #include <io.h>
#endif

#if defined (__WATCOMC__)

  // ++itschere20011030: f*ing qnx doesn't have gettimeofday()??? :-(

  static int gettimeofday (struct timeval *tv, struct timezone *dummy)
  {
    struct timespec now;
    clock_gettime (CLOCK_REALTIME, &now);
    tv->tv_sec = now.tv_sec;
    tv->tv_usec = now.tv_nsec;
    return 0;
  }

#endif

#include "thread/tracelog.h"
#include "thread/mutex.h"


//
//
//

void GTLCOUNT (unsigned char channel, const char *comment)
{
  //

  static CMUTEX mutex;

  mutex.Lock ();

  //

  static int count[256], first = 1;
  static FILE *fp = 0x0;

  if (first)
    {
      first = 0;

      memset (count, 0, sizeof (count));

      if (!(fp = fopen ("count.out", "w")))
	{
	  perror ("fopen()");
	  abort ();
	}
    }

  //

  count[channel]++;

  struct timeval now;

  gettimeofday (&now, 0x0);

  struct tm *tm = localtime (&now.tv_sec);

  fprintf (fp, "%02i:%02i:%02i.%06i %i %i # %s\n",
	   tm->tm_hour,
	   tm->tm_min,
	   tm->tm_sec,
	   (int)now.tv_usec,
	   channel,
	   count[channel],
	   comment);

  fflush (fp);

  //

  mutex.Unlock ();
}
