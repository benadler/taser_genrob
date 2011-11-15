//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "thread/thread.h"   // defines HAS_THREADS

#if HAS_THREADS

#include "thread/condition.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CCONDITION condition;
volatile int count = 0;


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

//
//
//

class CDUMMY : public CTHREAD
{
  int _num;

  virtual void Fxn (void);

public:

  CDUMMY (int num)
  {
    _num = num;

    Start ();
  }
};


//
//
//

void CDUMMY::Fxn (void)
{
  ConfirmRunning ();

  while (42)
    {
      condition.Lock ();

      condition.Wait ();

      if (--count < 0)
	{
	  fprintf (stderr, "synchronization problem\n");
	  abort ();
	}

      // ::printf ("bingo %i\n", _num);

      condition.Unlock ();
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int main (void)
{
  CDUMMY thr1 (1), thr2 (2), thr3 (3);

  thr1.Start ();
  thr2.Start ();
  thr3.Start ();

  while (42)
    {
      condition.Lock ();
      count++;
      condition.Signal ();
      condition.Unlock ();
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
