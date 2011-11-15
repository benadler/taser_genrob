//
//
//

#include <unistd.h>

#include "thread/thread.h"


class CTEST : public CDETACHEDTHREAD
{
public:

  CTEST (void);

  virtual ~CTEST (void);

  virtual void Fxn (void);
};


CTEST::CTEST (void)
{
}


CTEST::~CTEST (void)
{
}


void CTEST::Fxn (void)
{
  while (!_stopRequested)
    {
      write (fileno (stdout), "o", 1);
    }
}


int main (void)
{
  while (42)
    {
      CTEST *test = new CTEST;

      test->Start ();

      for (int i=0; i<100; i++)
	{
	  write (fileno (stdout), "*", 1);
	}

      test->Stop ();
    }
}
