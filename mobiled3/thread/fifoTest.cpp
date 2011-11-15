//
//
//

#include <time.h>
#include <stdio.h>
#include <unistd.h>

#include "thread/thread.h"   // defines HAS_THREADS

#if HAS_THREADS

#include "thread/fifo.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static CFIFO fifo;


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class CTESTMSG : public CFIFOMSG
{
public:

  int _num;

  CTESTMSG (int num)
  {
    _num = num;
  }
};


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
private:

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
      CTESTMSG *msg = (CTESTMSG *)fifo.Pop ();

      _cnt++;

      //::printf ("thread %i got message %i\n", _num, msg->_num);

      delete msg;
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int main (void)
{
  CDUMMY thr1 (1), thr2 (2), thr3 (3), thr4 (4), thr5 (5), thr6 (6);

  thr1.Start ();
  thr2.Start ();
  thr3.Start ();
  thr4.Start ();
  thr5.Start ();
  thr6.Start ();

  int cnt = 0;

  while (42)
    {
      for (int i=0; i<100000; i++)
	{
	  CTESTMSG *msg = new CTESTMSG (++cnt);

	  fifo.Push (msg);
	}

      printf ("%08x %08x %08x %08x %08x %08x\n",
	      thr1._cnt, thr2._cnt, thr3._cnt,
	      thr4._cnt, thr5._cnt, thr6._cnt);
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
