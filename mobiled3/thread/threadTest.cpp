//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef _REENTRANT
#define _REENTRANT
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "thread/thread.h"   // defines HAS_THREADS

#if HAS_THREADS

#include "thread/tracelog.h"


static void foo (void)
{
  GTLC ("foo()");

  for (int i=0; i<10000000; i++) ;

  GTLFATAL (("done\n"));
}


#ifdef _REENTRANT

class THREAD : public CTHREAD
{
protected:

  int _num;

public:

  virtual void Fxn (void);

  THREAD (int num)
  {
    _num = num;

    Start ();
  }
};


void THREAD::Fxn (void)
{
  GTL ("THREAD::Fxn()");

  ConfirmRunning ();

  while (!_stopRequested)
    {
      GTL ("inner()");

      GTLPRINT (("calling foo()\n"));

      foo ();
    }

  GTLFATAL (("terminating\n"));
}

#endif


///////////////////////////////////////////////////////////////////////////////

int main (void)
{
  GTLC ("main()");

  int cnt = 0;

  while (42)
    {
      GTLPRINT (("starting threads\n"));

#ifdef _REENTRANT
      THREAD *thread1 = new THREAD (++cnt);
      THREAD *thread2 = new THREAD (++cnt);
      THREAD *thread3 = new THREAD (++cnt);

      //sleep (1);

      GTLPRINT (("stopping threads\n"));

      delete thread1;
      delete thread2;
      delete thread3;

      //char *c = new char;   // just so that `thread' doesn't stay the same
#else
      foo ();
#endif
    }

  return 0;
}


#else // !HAS_THREADS

int main (void)
{
  fprintf (stderr, "this system has no thread support\n");
  return -1;
}

#endif // !HAS_THREADS
