//
// canVoltageCalibrate.cc
// (C) by Torsten Scherer (TeSche)
// <itschere@techfak.uni-bielefeld.de>
//

#include <stdio.h>
#include <unistd.h>

#include "mobile/canPlatform.h"


///////////////////////////////////////////////////////////////////////////////
//
// 53.94 40027
// 53.94 40033
// 53.94 40032 @ 11:36
//
// 50.64 38240
// 50.70 38268
// 50.73 38284 @ 11:40
//
// 50.67 38244
// 50.67 38258
// 50.67 38249 @ 13:04
//
// 
// 
// 
//
///////////////////////////////////////////////////////////////////////////////

int main (void)
{
  CCANPLATFORM platform;

  while (42)
    {
      int sum = 0;

      for (int i=0; i<20; i++)
	{
	  const int count = platform.GetBatteryVoltageRaw ();
	  printf ("%i ", count);
	  fflush (stdout);
	  sum += count;
	  sleep (1);
	}

      printf ("-> %i\n", sum / 20);
      fflush (stdout);

      sleep (40);
    }

  return 0;
}
