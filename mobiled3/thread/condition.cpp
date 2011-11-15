//
//
//

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "thread/condition.h"


#if HAS_THREADS

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class CCONDITIONAUTORUN
{
public:
  CCONDITIONAUTORUN (void)
  {
#if defined (_POSIX_SOURCE)

    int err = 0;

    if (sizeof (pthread_t) > 4)
      {
	fprintf (stderr,
		 "internal error, increment CCONDITION::_owner[] size to %i and recompile\n",
		 sizeof (pthread_t));
	err = 1;
      }

    if (sizeof (pthread_cond_t) > 48)
      {
	fprintf (stderr,
		 "internal error, increment CCONDITION::_cond[] size to %i and recompile\n",
		 sizeof (pthread_cond_t));
	err = 1;
      }

    if (sizeof (pthread_mutex_t) > 24)
      {
	fprintf (stderr,
		 "internal error, increment CCONDITION::_mutex[] size to %i and recompile\n",
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


static CCONDITIONAUTORUN autorun;


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CCONDITION::CCONDITION (void)
{
  _count = 0;

#if defined (_POSIX_SOURCE)
  
  pthread_cond_init ((pthread_cond_t *)_cond, 0x0);
  pthread_mutex_init ((pthread_mutex_t *)_mutex, 0x0);   // fast, non error checking mutex

#elif defined (_WIN32)

  *(HANDLE *)_cond = CreateSemaphore (NULL, 0, 0x7fffffff, NULL);
  *(HANDLE *)_mutex = CreateMutex (NULL, FALSE, NULL);

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CCONDITION::~CCONDITION (void)
{
  if (_count > 0)
    {
      // try to unlock the mutex. it really shouldn't be locked any more...

      fprintf (stderr, "CMUTEX::~CMUTEX() -- mutex still locked, unlocking\n");

      Unlock ();
    }

#if defined (_POSIX_SOURCE)
	
  ::pthread_mutex_destroy ((pthread_mutex_t *)_mutex);
  ::pthread_cond_destroy ((pthread_cond_t *)_cond);

#elif defined (_WIN32)

  CloseHandle (*(HANDLE *)_mutex);
  CloseHandle (*(HANDLE *)_cond);

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CCONDITION::Lock (void)
{
  static const char * const fxnName = "CCONDITION::Lock() -- ";

#if defined (_POSIX_SOURCE)
  
  if ((_count > 0) &&
      pthread_equal (*(pthread_t *)_owner, pthread_self ()))
    {
      fprintf (stderr, "%srecursive mutex lock\n", fxnName);

      fprintf (stderr, "count = %i\n", _count);
      fprintf (stderr, "%lu %lu\n", *(pthread_t *)_owner, pthread_self ());

      abort ();
    }

  int err;

  if ((err = pthread_mutex_lock ((pthread_mutex_t *)_mutex)))
    {
      errno = err;
      perror ("pthread_mutex_lock()");
      fprintf (stderr, "%serror locking mutex: %i\n", fxnName, err);
      abort ();
    }

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


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CCONDITION::Unlock (void)
{
  static const char * const fxnName = "CCONDITION::Unlock() -- ";

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

  _count--;

  ::pthread_mutex_unlock ((pthread_mutex_t *)_mutex);

#elif defined (_WIN32)
  
  _count--;

  ReleaseMutex (*(HANDLE *)_mutex);

#endif
  
  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCONDITION::Wait (void)
{
  static const char * const fxnName = "CCONDITION::Wait() -- ";

#if defined (_POSIX_SOURCE)

  if ((_count < 1) ||
      !::pthread_equal (*(pthread_t *)_owner, ::pthread_self ()))
    {
      ::fprintf (stderr, "%snot owner\n", fxnName);
      ::abort ();
    }

  _count--;

  ::pthread_cond_wait ((pthread_cond_t *)_cond, (pthread_mutex_t *)_mutex);

  *(pthread_t *)_owner = ::pthread_self ();

  _count++;

#elif defined (_WIN32)

  Unlock ();

  // ++itschere20010930: this isn't atomic as pthread_cond_wait(), but then it
  // doesn't have to be because the OS guarantees no Signal() or Broadcast() is
  // lost. the mutex still ensures atomicy on the user data. worst case
  // scenario is that someone calls Signal() n times, the first Wait() returns
  // and processes all the data and then n-1 Wait() return without data.  this
  // is explicitly allowed for pthread_cond_wait(), so that's why it's not a
  // problem here.

  WaitForSingleObject (*(HANDLE *)_cond, INFINITE);

  Lock ();

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#if defined (_POSIX_SOURCE)

bool CCONDITION::WaitTimeout (milliseconds_t timeout)
{
  static const char * const fxnName = "CCONDITION::WaitTimeout() -- ";

  if ((_count < 1) ||
      !pthread_equal (*(pthread_t *)_owner, pthread_self ()))
    {
      fprintf (stderr, "%snot owner\n", fxnName);
      abort ();
    }

  _count--;

  //

  struct timeval now;
  gettimeofday (&now, 0x0);

  if (timeout > 3294)
    {
      timeout = 3294;   // prevent arithmetic wrap on 32 bit unsigned ints...
    }

  unsigned int ns = 1000 * (now.tv_usec + timeout * 1000);   // ...here!

  struct timespec ts = {now.tv_sec + ns / 1000000000, ns % 1000000000};

  int ret = pthread_cond_timedwait ((pthread_cond_t *)_cond,
				    (pthread_mutex_t *)_mutex,
				    &ts);

  *(pthread_t *)_owner = ::pthread_self ();

  _count++;

  return ret == 0;
}

#endif


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCONDITION::Signal (void)
{
  static const char * const fxnName = "CCONDITION::Signal() -- ";

  if (_count < 0)
    {
      fprintf (stderr, "%snot locked\n", fxnName);
      abort ();
    }

#if defined (_POSIX_SOURCE)

  ::pthread_cond_signal ((pthread_cond_t *)_cond);

#else

  ReleaseSemaphore (*(HANDLE *)_cond, 1, 0x0);

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCONDITION::Broadcast (void)
{
  static const char * const fxnName = "CCONDITION::Signal() -- ";

  if (_count < 0)
    {
      fprintf (stderr, "%snot locked\n", fxnName);
      abort ();
    }

#if defined (_POSIX_SOURCE)

  ::pthread_cond_broadcast ((pthread_cond_t *)_cond);

#elif defined (_WIN32)

  ReleaseSemaphore (*(HANDLE *)_cond, 1, 0x0);

#endif
}

#endif // HAS_THREADS
