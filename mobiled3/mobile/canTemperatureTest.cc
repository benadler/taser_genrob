//
//
//

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include "mobile/canPlatform.h"

#include "thread/tracelog.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static void run (CCANPLATFORM &canPlatform)
{
  struct timeval start;
  gettimeofday (&start, 0x0);

  while (42)
    {
      canPlatform.SetWorldVelocity (2.0 * M_PI, 2.0 * M_PI);

      usleep (100000);
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int main (void)
{
#if 0
  _globGtlEnable = 1;
#endif

  CCANPLATFORM canPlatform;

  canPlatform.StartMotors ();

  run (canPlatform);

  return 0;
}
