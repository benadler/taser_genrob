//
// thread.h
//

#ifndef _THREAD_H_
#define _THREAD_H_


// include a standard header file to give the library the chance to define its
// usual defines.

#include <stdio.h>

#ifdef HAS_THREADS
  #undef HAS_THREADS
#endif

#if defined (__svr4__) && defined (sparc)   // solaris
  #ifndef _POSIX_SOURCE
    #define _POSIX_SOURCE
  #endif
#endif

#if defined (_POSIX_SOURCE)   // gcc@linux/solaris
  #define HAS_THREADS 1
  #include <pthread.h>
#elif defined (_WIN32)
  #define HAS_THREADS 1
  #include <process.h>
  #include <winsock2.h>// ++itschere20020111: winbase.h will load winsock.h
  #include <wtypes.h>
  #include <winbase.h>   // Sleep()
#elif defined (__WATCOMC__)
  #define HAS_THREADS 0
#else
  #error "operating system not supported"
#endif


#include "windefs.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

typedef struct
{
  unsigned long _num;
  int _indent;   
} THREADARGS;


THREADARGS *GetCurrentThreadArgs (void);


///////////////////////////////////////////////////////////////////////////////
//
// CTHREAD - thread supporter class
//
// implements architecture independent support for threads. note that the class
// is abstract due to the virtual method `Fxn()'. it's this method you're got
// to overload in a derived class to really implement the thread routine.
//
// starting a thread
//
// once created a thread can only be started once using `Start()'. any attempt
// to call `Start()' more than once results in an error. the first thing any
// `Fxn()' should do is call `ConfirmRunning()'. unless it does so the main
// thread will not continue (but stay in a busy poll loop (currently)). this is
// to get around troubles with the virtual table having disappeared again if
// the object is destroyed immediately after returning from `Start()' in the
// main thread before the spawned thread had a chance to call `Fxn()'. having
// an extra function to acknowledge the main thread isn't really nice but for
// now the easiest and most portable thing to do.
//
// stopping a thread
//
// once running a thread can either terminate by simply returning from `Fxn()'
// or be stopped by calling the destructor, but calling the destructor is NOT
// allowed from withing the thread itself. therefore if you just return from
// `Fxn()' and do not clean up the object from somewhere else you've got a
// memory leak. termination by calling the destructor is cooperative in that it
// only sets the flag `_stopRequested', which is to be obeyed by the thread. if
// the thread does not voluntarily return from `Fxn()' upon a stop request the
// destructor will block in an endless busy poll loop. this isn't a nice
// behaviour at all, but the most portable. I don't dare to implement posix
// thread cancellation in windows...
//
///////////////////////////////////////////////////////////////////////////////

class WINFLAGS CTHREAD
{
  //

public:

  THREADARGS _threadArgs;

#if defined (_REENTRANT) && HAS_THREADS

private:

#if defined (_POSIX_SOURCE)

  pthread_t _id;

  friend void *_wrapper (CTHREAD *thread);

#elif defined (_WIN32)

  unsigned int _id;

  friend unsigned int _wrapper (CTHREAD *thread);

#endif

protected:

  //

  volatile int _started;
  volatile int _running;
  volatile int _stopRequested;
  volatile int _stopped;

  //

  CTHREAD (void);

  virtual ~CTHREAD (void);

public:

  //
  //
  //

  int Start (void);

  //
  //
  //

  void ConfirmRunning (void)
  {
    _running = 1;
  }

  //
  //
  //

  int Stop (void);

  //
  //
  //

  virtual void Fxn (void) = 0x0;

  //
  //
  //

  void ScheduleRealtime (const int nice = 0) const;

#endif // _REENTRANT
};


///////////////////////////////////////////////////////////////////////////////
//
// detached threads (see posix manual)
//
// a "detached thread" is a thread which cleans up its data structures
// automatically upon termination, i.e., upon exit from the thread
// function. the pointer to the object is only valid as long as the thread
// runs, and becomes invalid upon thread termination. therefore the thread
// should NEVER terminate on its own will, but ONLY upon an explicit stop()
// command, so that the creating thread gets a chance to realize that its child
// has gone (is going).
//
///////////////////////////////////////////////////////////////////////////////

#if defined (_POSIX_SOURCE)

class CDETACHEDTHREAD
{
private:

  THREADARGS _threadArgs;

  pthread_t _id;

  friend void *_wrapperDetached (CDETACHEDTHREAD *thread);

protected:

  //

  int _started;
  bool _stopRequested;

  virtual ~CDETACHEDTHREAD (void);   // should only be deleted from wrapper fxn

public:

  //
  //
  //

  CDETACHEDTHREAD (void)
  {
    _started = 0;
    _stopRequested = false;
  }

  void Start (void);

  void Stop (void)
  {
    _stopRequested = true;
  }

  //
  //
  //

  virtual void Fxn (void) = 0x0;
};

#endif // _POSIX_SOURCE

#endif // _THREAD_H_
