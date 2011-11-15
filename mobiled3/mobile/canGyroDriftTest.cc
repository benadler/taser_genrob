//
// canGyroDriftTest.cc
// (C) 12/2003 by Torsten Scherer (TeSche)
// <itschere@techfak.uni-bielefeld.de>
//

#include <stdio.h>
#include <unistd.h>

#include "thread/tracelog.h"

#include "mobile/canPlatform.h"


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

  CCANPLATFORM platform;

  time_t start = time (0);

  while (42)
    {
      time_t now = time (0);

      int gyro = platform.GetGyroValAbs ();

      printf ("%f %i\n", (now - start) / 3600.0, gyro);
      fflush (stdout);

      sleep (10);
    }

  return 0;
}
