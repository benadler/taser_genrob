//
//
//

#include <math.h>
#include <stdio.h>
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
  while (42)
    {
      canPlatform.SetWorldVelocity (2.0, 0.0);

      canPlatform.GetStatus ();

      usleep (10000);

      double dl, dr, dg;

      //canPlatform.GetWorldState (dl, dr, dg);
    }
}


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

  CCANPLATFORM canPlatform;

  //

  run (canPlatform);

  //

  return 0;
}
