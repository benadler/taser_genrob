//
//
//

#ifndef _CONDITION_H_
#define _CONDITION_H_


#include "thread/thread.h"

#if defined (_POSIX_SOURCE)
#include "iotypes.h"   // milliseconds_t
#endif


#if HAS_THREADS

class WINFLAGS CCONDITION
{
private:

  // not even implemented
  CCONDITION (const CCONDITION &);
  CCONDITION &operator= (const CCONDITION &);

protected:

  int _count;

  // opaque data types...
  char _owner[4];
  char _cond[48];   // linux=12, solaris=16, newer linux=48
  char _mutex[24];   // linux=24

public:

  CCONDITION (void);
  virtual ~CCONDITION (void);

  // pthread_cond_init()
  // pthread_cond_signal()
  // pthread_cond_broadcast()
  // pthread_cond_wait()
  // pthread_cond_timedwait()
  // pthread_cond_destroy()

  int Lock (void);
  int Unlock (void);

  void Wait (void);
  void Signal (void);
  void Broadcast (void);

#if defined (_POSIX_SOURCE)
  bool WaitTimeout (milliseconds_t timeout);
#endif
};

#endif // HAS_THREADS

#endif
