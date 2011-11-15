//
// canGyroCalibrate.cc
// (C) by Torsten Scherer (TeSche)
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

  while (42)
    {
      int gyro = platform.GetGyroVal ();

      printf ("%i\n", gyro);

      usleep (100000);
    }

  return 0;
}
