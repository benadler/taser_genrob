//
// laserTest.cc
//

#include <unistd.h>

#include "./laser.h"

#include "thread/tracelog.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static CLASER laser0, laser1;


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int main (void)
{
#if 1
  _globGtlEnable = 1;
#endif

#if 1
  CSICKLASER sick0 (0, &laser0);

  sick0.Start ();
#else
  CSICKLASER sick1 (1, &laser1);

  sick1.Start ();
#endif

  while (42)
    {
      sleep (1);
    }

  return 0;
}
