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

  #include <unix.h>

  // ++itschere20011030: f*ing qnx doesn't have gettimeofday()??? :-(

  int gettimeofday (struct timeval *tv, struct timezone *dummy)
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


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#define INDENTING


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int _globGtlEnable = 0;


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

_CGTL1::_CGTL1 (const void *addr, const char *fxnName)
{
  _addr = addr;
  _fxn = fxnName;
  _threadArgs = GetCurrentThreadArgs ();

  if (_globGtlEnable)
    {
      Print ("0x%08x->%s\n", _addr, _fxn);
    }

  if (_threadArgs)
    {
      _threadArgs->_indent++;
    }

  // qnx bug (watcom c16 v10.6): if the inline destructor (any inline
  // function?) contains only code conditionalized by means of preprocessor
  // defines (only this special define?) then NO code is generated AT ALL,
  // regardless of whether the condition is true or false. there has to be at
  // least some dummy code around (here: cleaning a variable) to force
  // generation of the code inside the condition.

  // no, it's more like it sucks on inline functions in general... :-(
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

_CGTL1::~_CGTL1 (void)
{
  if (_threadArgs)
    {
      _threadArgs->_indent--;
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

// ++itschere20021125: SIGSEGV in RCCL???

void _CGTL1::Print (const char *fmt, ...)
{
  char buf[256], *ptr = buf;
  int size = 0, remaining = sizeof (buf) - 7;   // strlen(" (...)")+1

#if 1
  struct timeval now;
  gettimeofday (&now, 0x0);
  struct tm *tm = localtime (&now.tv_sec);
#else
  time_t now = time (0x0);
  struct tm *tm = localtime (&now);
#endif

  if (_globGtlEnable)
    {
      size = sprintf (buf,
		      "# %02lx@%02i:%02i:%02i.%06i : ",
		      _threadArgs ? _threadArgs->_num : 0xffffffff,
		      tm->tm_hour,
		      tm->tm_min,
		      tm->tm_sec,
		      (int)now.tv_usec);

      ptr += size;
      remaining -= size;

      if (_threadArgs)
	{
	  int cnt = _threadArgs->_indent;

	  while ((cnt-- > 0) && (remaining >= 2))
	    {
	      size += 2;
	      remaining -= 2;
	      *ptr++ = ' ';
	      *ptr++ = ' ';
	    }

	  *ptr = 0;
	}
    }
  else
    {
      size = sprintf (buf, "# %02lx@%02i:%02i:%02i.%06i (0x%08x->%s) : ",
		      _threadArgs ? _threadArgs->_num : 0xffffffff,
		      tm->tm_hour,
		      tm->tm_min,
		      tm->tm_sec,
		      (int)now.tv_usec,
		      (unsigned int)_addr,
		      _fxn);

      ptr += size;
      remaining -= size;
    }

  // now the variable part

  va_list args;

  va_start (args, fmt);
  int inc = vsnprintf (ptr, remaining, fmt, args);
  va_end (args);

  if (inc > remaining)   // `man vsnprintf'
    {
      inc = remaining;
    }

  ptr += inc;
  size += inc;
  remaining -= inc;

  //

  if (remaining < 1)
    {
      size += sprintf (ptr, " (...)");
    }

#if defined (_WIN32)
  _write (fileno (stderr), buf, size);
#else
  write (fileno (stderr), buf, size);
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

_CGTL2::_CGTL2 (const void *addr, const char *fxnName)
{
  _addr = addr;
  _fxn = fxnName;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

_CGTL2::~_CGTL2 (void)
{
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void _CGTL2::Print (const char *fmt, ...)
{
  THREADARGS *_threadArgs = GetCurrentThreadArgs ();
  char buf[256];
  int remaining = sizeof (buf) - 7;

#if 1
  struct timeval now;
  gettimeofday (&now, 0x0);
  struct tm *tm = localtime (&now.tv_sec);
#else
  time_t now = time (0x0);
  struct tm *tm = localtime (&now);
#endif

  int size = sprintf (buf, "# %02lx@%02i:%02i:%02i.%06i : *** FEHLER IN 0x%08x->%s : ",
		      _threadArgs ? _threadArgs->_num : 0xffffffff,
		      tm->tm_hour,
		      tm->tm_min,
		      tm->tm_sec,
		      (int)now.tv_usec,
		      (unsigned int)_addr,
		      _fxn);

  remaining -= size;
  char *ptr = buf + size;

  //

  va_list args;

  va_start (args, fmt);
  int inc = vsnprintf (ptr, remaining, fmt, args);
  va_end (args);

  if (inc > remaining)   // `man vsnprintf'
    {
      inc = remaining;
    }

  ptr += inc;
  size += inc;
  remaining -= inc;

  //

  if (remaining < 1)
    {
      size += sprintf (ptr, " (...)");
    }

#if defined (_WIN32)
  _write (fileno (stderr), buf, size);
#else
  write (fileno (stderr), buf, size);
#endif
}
