//
//
//

#include <stdio.h>
#include <stdlib.h>

#include "thread/mutex.h"


#if HAS_THREADS


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class CMUTEXAUTORUN
{
public:
  CMUTEXAUTORUN (void)
  {
#if defined (_POSIX_SOURCE)

    int err = 0;

    if (sizeof (pthread_t) > 4)
      {
	fprintf (stderr,
		 "internal error, increment CMUTEX::_owner[] size to %i and recompile\n",
		 sizeof (pthread_t));
	err = 1;
      }

    if (sizeof (pthread_mutex_t) > 24)
      {
	fprintf (stderr,
		 "internal error, increment CMUTEX::_mutex[] size to %i and recompile\n",
		 sizeof (pthread_mutex_t));
	err = 1;
      }

    if (err)
      {
	abort ();
      }

#endif
  }
};


static CMUTEXAUTORUN autorun;


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CMUTEX::CMUTEX (void)
{
  _count = 0;

#if defined (_POSIX_SOURCE)

  pthread_mutex_init ((pthread_mutex_t *)_mutex, 0x0);

#elif defined (_WIN32)

  *(HANDLE *)_mutex = CreateMutex (NULL, FALSE, NULL);

#endif
}


//
//
//

CMUTEX::~CMUTEX (void)
{
  if (_count > 0)
    {
      // try to unlock the mutex. it really should be locked any more...

      fprintf (stderr, "CMUTEX::~CMUTEX() -- mutex still locked, unlocking\n");

      Unlock ();
    }

#if defined (_POSIX_SOURCE)

  pthread_mutex_destroy ((pthread_mutex_t *)_mutex);

#elif defined (_WIN32)

  CloseHandle (*(HANDLE *)_mutex);

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
// acquire a lock
//
///////////////////////////////////////////////////////////////////////////////

int CMUTEX::Lock (void)
{
  static const char * const fxnName = "CMUTEX::Lock() -- ";

#if defined (_POSIX_SOURCE)

  if ((_count > 0) &&
      pthread_equal (*(pthread_t *)_owner, ::pthread_self ()))
    {
      fprintf (stderr, "%srecursive mutex lock\n", fxnName);

      fprintf (stderr, "count = %i\n", _count);
      fprintf (stderr, "%lu %lu\n", *(pthread_t *)_owner, pthread_self ());

      abort ();
    }

  if (pthread_mutex_lock ((pthread_mutex_t *)_mutex))
    {
      fprintf (stderr, "%serror locking mutex\n", fxnName);
      abort ();
    }

  // mutex is locked

  *(pthread_t *)_owner = pthread_self ();

#elif defined (_WIN32)

  WaitForSingleObject (*(HANDLE *)_mutex, INFINITE);

#endif

  if (++_count != 1)   // this must be last
    {
      fprintf (stderr, "%ssyncronization problem\n", fxnName);
      abort ();
    }

  return 0;
}


#ifndef _WIN32

///////////////////////////////////////////////////////////////////////////////
//
// try to acquire a lock
//
///////////////////////////////////////////////////////////////////////////////

int CMUTEX::TryLock (void)
{
#if defined (_POSIX_SOURCE)

  int ret = pthread_mutex_trylock ((pthread_mutex_t *)_mutex);

  if (!ret)
    {
      // mutex is locked

      *(pthread_t *)_owner = pthread_self ();

      _count++;
    }

  return ret;

#else

  return 0;

#endif
}

#endif


///////////////////////////////////////////////////////////////////////////////
//
// release a lock
//
///////////////////////////////////////////////////////////////////////////////

int CMUTEX::Unlock (void)
{
  static const char * const fxnName = "CMUTEX::Unlock() -- ";

  if (_count < 1)
    {
      fprintf (stderr, "%snot locked\n", fxnName);
      abort ();
    }

#if defined (_POSIX_SOURCE)

  if (!pthread_equal (*(pthread_t *)_owner, pthread_self ()))
    {
      fprintf (stderr, "%snot owner\n", fxnName);
      abort ();
    }

  // from here on I (current thread) know I have locked the mutex

  _count--;

  if (pthread_mutex_unlock ((pthread_mutex_t *)_mutex))
    {
      fprintf (stderr, "CMUTEX::Unlock() -- error unlocking mutex\n");
      abort ();
    }

#elif defined (_WIN32)

  _count--;

  ReleaseMutex (*(HANDLE *)_mutex);

#endif

  return 0;
}

#endif // HAS_THREADS
