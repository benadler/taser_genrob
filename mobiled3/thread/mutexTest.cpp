//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#include "thread/thread.h"   // defined HAS_THREADS

#if HAS_THREADS

#include "thread/mutex.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static int count = 0;
static CMUTEX mutex;


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

  int _cnt;

  CDUMMY (int num)
  {
    _cnt = 0;
    _num = num;
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
      mutex.Lock ();

      _cnt++;

      if (++count != 1)
	{
	  ::fprintf (stderr, "mutex failed\n");
	  ::abort ();
	}

      --count;

      mutex.Unlock ();
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
#ifdef _WIN32
      Sleep (1000);
#else
      struct timeval tv = {2, 0};

      ::select (0, 0x0, 0x0, 0x0, &tv);
#endif

      printf ("%08x %08x %08x\n", thr1._cnt, thr2._cnt, thr3._cnt);
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
