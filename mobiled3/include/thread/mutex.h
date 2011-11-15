//
//
//

#ifndef _MUTEX_H_
#define _MUTEX_H_

#include "thread/thread.h"


#if HAS_THREADS

class WINFLAGS CMUTEX
{
private:

  // not even implemented
  CMUTEX (const CMUTEX &);
  CMUTEX &operator= (const CMUTEX &);

protected:

  // opaque data types...
  int _count;
  char _owner[4];
  char _mutex[24];

public:

  CMUTEX (void);
  virtual ~CMUTEX (void);

  int Lock (void);
#ifndef _WIN32
  int TryLock (void);
#endif
  int Unlock (void);
};

#endif // HAS_THREADS

#endif
