//
//
//


#ifndef _REENTRANT
  #error "have to compile this with _REENTRANT"
#endif

#include "thread/thread.h"

#if HAS_THREADS

#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#if defined (_POSIX_SOURCE)
  #include <sched.h>
  #include <sys/mman.h>
#endif

//
//
//

#if defined (_WIN32)

extern "C"
{
  WINFLAGS void init (void)
  {
  }
}

#endif


///////////////////////////////////////////////////////////////////////////////
//
// CTHREAD -- multi thread version
//
///////////////////////////////////////////////////////////////////////////////

static int _globThreadListInitialized = 0;
static unsigned long globThreadNum = 0;


//

struct threaddata
{
#if defined (_POSIX_SOURCE)
  pthread_t id;
#elif defined (_WIN32)
  unsigned int id;
#else
  #error "operating system not supported"
#endif

  THREADARGS *args;
};


//

class _CTHREADLIST
{
private:

#if defined (_POSIX_SOURCE)
  pthread_mutex_t _mutex;
#elif defined (_WIN32)
  HANDLE _mutex;
#endif

  int _maxThreadData;
  struct threaddata *_threadData;

  //
  //
  //

  void Lock (void)
  {
#if defined (_POSIX_SOURCE)
    pthread_mutex_lock (&_mutex);
#elif defined (_WIN32)
    WaitForSingleObject (_mutex, INFINITE);
#endif
  }

  //
  //
  //

  void Unlock (void)
  {
#if defined (_POSIX_SOURCE)
    pthread_mutex_unlock (&_mutex);
#elif defined (_WIN32)
    ReleaseMutex (_mutex);
#endif
  }

public:

  //
  //
  //

  _CTHREADLIST (void)
  {
    static THREADARGS mainThreadArgs = {0, 0};

    if (_globThreadListInitialized)
      {
	fprintf (stderr, "FATAL SYSTEM ERROR:\n");
	fprintf (stderr, "you've somehow managed to create a second instance of_CTHREADLIST, what was\n");
	fprintf (stderr, "meant to be a class completely private to CTHREAD. there's no way how the\n");
	fprintf (stderr, "program could safely continue now that something is utterly wrong and so we\n");
	fprintf (stderr, "will abort() here.\n");

	abort ();
      }

#if defined (_POSIX_SOURCE)
    pthread_mutex_init (&_mutex, 0x0);
#elif defined (_WIN32)
    _mutex = CreateMutex (NULL, FALSE, NULL);
#endif

    int size = (_maxThreadData = 1) * sizeof (struct threaddata);

    if (!(_threadData = (struct threaddata *)malloc (size)))
      {
	fprintf (stderr, "out of memory\n");
	abort ();
      }

#if defined (_POSIX_SOURCE)
    _threadData->id = pthread_self ();
#elif defined (_WIN32)
    _threadData->id = GetCurrentThreadId ();
#endif
    _threadData->args = &mainThreadArgs;

    _globThreadListInitialized = 1;
  }

  //
  //
  //

  ~_CTHREADLIST (void)
  {
#if defined (_WIN32)
    CloseHandle (_mutex);
#endif
  }

  //
  //
  //

#if defined (_POSIX_SOURCE)
  void Add (THREADARGS *threadArgs, pthread_t id)
#elif defined (_WIN32)
  void Add (THREADARGS *threadArgs, unsigned int id)
#endif
  {
    Lock ();

    int i;

    // search a free slot

    for (i=0; i<_maxThreadData; i++)
      {
	if (!_threadData[i].args)
	  {
	    break;
	  }
      }

    // if no free slot was found add a new one

    if (i == _maxThreadData)
      {
	int size = ++_maxThreadData * sizeof (struct threaddata);

#ifdef __GNUC__
#warning "++itschere20011228: possible mt problem?"
#endif

	// ++itschere20011027: tracelog will save the pointer to the thread
	// data, which may change here and so we've got a problem...

	if (!(_threadData = (struct threaddata *)realloc (_threadData, size)))
	  {
	    fprintf (stderr, "out of memory\n");
	    abort ();
	  }
      }

    //printf ("%i/%i\r", i, _maxThreadData);

    _threadData[i].id = id;
    _threadData[i].args = threadArgs;

    Unlock ();
  }

  //
  //
  //

#if defined (_POSIX_SOURCE)
  void Delete (pthread_t id)
#else
  void Delete (unsigned int id)
#endif
  {
    Lock ();

    int i;

    for (i=0; i<_maxThreadData; i++)
      {
#if defined (_POSIX_SOURCE)
	if (pthread_equal (id, pthread_self ()))
#elif defined (_WIN32)
	if (id == GetCurrentThreadId ())
#endif
	  {
	    break;
	  }
      }

    _threadData[i].id = 0;
    _threadData[i].args = 0x0;

    Unlock ();
  }

  //

#if defined (_POSIX_SOURCE)
  THREADARGS *FindThreadArgs (pthread_t id)
#elif defined (_WIN32)
  THREADARGS *FindThreadArgs (unsigned int id)
#endif
  {
    Lock ();

    int i;

    for (i=0; i<_maxThreadData; i++)
      {
#if defined (_POSIX_SOURCE)
	if (pthread_equal (_threadData[i].id, id))
#elif defined (_WIN32)
	if (_threadData[i].id == id)
#endif
	  {
	    break;
	  }
      }

    // if we can't find the thread it has not been started by us. in this case
    // we can't provide any reasonable thread arguments.

    if (i == _maxThreadData)
      {

	Unlock ();

	return 0x0;
      }

    //

    THREADARGS *args = _threadData[i].args;

    Unlock ();

    return args;
  }
};


//

static _CTHREADLIST _globThreadList;


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CTHREAD::CTHREAD (void)
{
  if (!_globThreadListInitialized)
    {
      fprintf (stderr, "FATAL SYSTEM ERROR:\n");
      fprintf (stderr, "you've created a global CTHREAD instance to be initialized even before\n");
      fprintf (stderr, "main(). there is no way to determine the order of initialization of these\n");
      fprintf (stderr, "kind of objects. however, the implementation of CTHREAD relies on a particular\n");
      fprintf (stderr, "order, which has failed on this system. the only thing to avoid this problem\n");
      fprintf (stderr, "is to get rid of the global CTHREAD instance.\n");

      abort ();
    }

  _started = _running = _stopped = _stopRequested = 0;

  _threadArgs._num = ++globThreadNum;
  _threadArgs._indent = 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CTHREAD::~CTHREAD (void)
{
#if 0 // ++itschere20021212

  Stop ();

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

THREADARGS *GetCurrentThreadArgs (void)
{
#if defined (_POSIX_SOURCE)
  return _globThreadList.FindThreadArgs (pthread_self ());
#elif defined (_WIN32)
  return _globThreadList.FindThreadArgs (GetCurrentThreadId ());
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
// need this for posix threads
//
///////////////////////////////////////////////////////////////////////////////

#if defined (_POSIX_SOURCE)

void *_wrapper (CTHREAD *thread)
{
  _globThreadList.Add (&thread->_threadArgs, pthread_self ());

  thread->Fxn ();

  _globThreadList.Delete (pthread_self ());

  thread->_stopped = 1;

  return 0x0;
}

#elif defined (_WIN32)

unsigned int _wrapper (CTHREAD *thread)
{
  _globThreadList.Add (&thread->_threadArgs, GetCurrentThreadId ());

  thread->Fxn ();

  _globThreadList.Delete (GetCurrentThreadId ());

  thread->_stopped = 1;

  return 0;
}

#endif

//
//
//

int CTHREAD::Start (void)
{
  //printf ("CTHREAD::Start()\n");

  if (_started++)
    {
      // sorry, may only be started once
      return -1;
    }

  // start it

#if defined (_POSIX_SOURCE)

  pthread_attr_t attr;

  pthread_attr_init (&attr);
  pthread_attr_setscope (&attr, PTHREAD_SCOPE_SYSTEM);
  pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);

  pthread_create (&_id,
		  &attr,
		  (void * (*) (void *))_wrapper,
		  this);

  pthread_attr_destroy (&attr);

#elif defined (_WIN32)

  unsigned long err = _beginthreadex (0x0,
				      0,
				      (unsigned int (__stdcall *) (void *))_wrapper,
				      this,
				      0,
				      &_id);

#endif

  time_t start = time (0x0);

  while (!_running)   // wait until thread is up and running
    {
#if defined (_POSIX_SOURCE) && defined (linux)
      sched_yield ();
#elif defined (_WIN32)
      Sleep (0);
#endif

      if (time (0x0) - start >= 5)
	{
	  fprintf (stderr, "thread still not running after 5 seconds???\n");
	  fflush (stderr);
	  abort ();
	}
    }

  return 0;
}


//
//
//

int CTHREAD::Stop (void)
{
#if defined (_POSIX_SOURCE)

  if (pthread_equal (_id, pthread_self ()))
    {
      fprintf (stderr, "thread Stop()ed from within itself\n");
      abort ();
    }

#elif defined (_WIN32)

  if (_id == GetCurrentThreadId ())
    {
      fprintf (stderr, "thread Stop()ed from within itself\n");
      abort ();
    }

#endif

  if (!_started)
    {
      // didn't even start
      return 1;
    }

  // stop thread

  _stopRequested = 1;

  while (!_stopped)
    {
#if defined (_POSIX_SOURCE) && defined (linux)
      sched_yield ();
#elif defined (_WIN32)
      Sleep (0);
#endif
    }

  return 0;
}


//
//
//

void CTHREAD::ScheduleRealtime (const int nice) const
{
#if defined (_POSIX_SOURCE) && defined (linux)

  //

  if (mlockall (MCL_CURRENT | MCL_FUTURE))
    {
      fprintf (stderr, "mlockall() failed\n");
    }

  //

  struct sched_param p;

#define SCHED_TESCHE SCHED_FIFO

  int maxPrio = sched_get_priority_max (SCHED_TESCHE);

  p.sched_priority = maxPrio >= nice ? maxPrio - nice : 0;

  if (sched_setscheduler (0, SCHED_TESCHE, &p))
    {
      fprintf (stderr, "sched_setscheduler() failed\n");
    }

#endif
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
// CDETACHEDTHREAD
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void *_wrapperDetached (CDETACHEDTHREAD *thread)
{
  _globThreadList.Add (&thread->_threadArgs, pthread_self ());

  thread->Fxn ();

  delete thread;

  return 0x0;
}


//
//
//

void CDETACHEDTHREAD::Start (void)
{
  if (++_started > 1)
    {
      fprintf (stderr,
	       "%p->CDETACHEDTHREAD::Start() -- called more than once\n",
	       (void*)this);
      abort ();
    }

  _threadArgs._num = ++globThreadNum;
  _threadArgs._indent = 0;

  pthread_attr_t attr;

  pthread_attr_init (&attr);
  pthread_attr_setscope (&attr, PTHREAD_SCOPE_SYSTEM);
  pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);

  pthread_create (&_id,
		  &attr,
		  (void * (*) (void *))_wrapperDetached,
		  this);

  pthread_attr_destroy (&attr);
}


//
//
//

CDETACHEDTHREAD::~CDETACHEDTHREAD (void)
{
  if (!pthread_equal (_id, pthread_self ()))
    {
      fprintf (stderr,
	       "%p->CDETACHEDTHREAD::~CDETACHEDTHREAD() -- called from extern\n",
	       (void*)this);
      abort ();
    }

  //

  if (_started > 0)
    {
      _globThreadList.Delete (pthread_self ());
    }
}


#else // !HAS_THREADS

///////////////////////////////////////////////////////////////////////////////
//
// CTHREAD -- single thread version
//
// our version of QNX (4.25) seems to have no thread support, though some
// online docs say QNX has posix threads. no idea what's going on, but I can't
// find any header file with the definitions, so I can't do thread support for
// our qnx (probably you need a newer version than 4.25). but I have to supply
// a dummy CTHREAD class so that tracelog works...
//
///////////////////////////////////////////////////////////////////////////////

THREADARGS *GetCurrentThreadArgs (void)
{
  static THREADARGS args = {0, 0};

  return &args;
}


#endif // !HAS_THREADS
