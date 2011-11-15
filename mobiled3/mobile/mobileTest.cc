//
// mobileTest.cc
// (C) 12/2002 by TeSche (Torsten Scherer)
// <itschere@techfak.uni-bielefeld.de>
//

#include <stdlib.h>
#include <unistd.h>

#include "thread/tracelog.h"

#include "mobile/mobile.h"


int main (void)
{
#if 1
  _globGtlEnable = 1;
#endif

  CMOBILE mobile ("localhost");

  double x0, y0, a0;

  mobile.GetPosition (x0, y0, a0);

  mobile.SetScale (0.8, -1.0);

  while (42)
    {
#if 1
      mobile.SetPosition (0.0, 0.0, 0.0);
      mobile.RotateAngle (170.0);
      mobile.WaitForCompleted ();
      abort ();
#endif

#if 0

      mobile.SetPosition (0, 0, 0);
      mobile.Forward (-10);
      mobile.WaitForCompleted ();

#else

      mobile.Move (2, 2, -90);
      mobile.Move (6, 2, 0);
      mobile.Move (6, 4, 90);
      mobile.Move (2, 4, 180);

#endif

      //

      mobile.WaitForCompleted ();

      double x, y, a;

      mobile.GetPosition (x, y, a);

      printf ("%f %f %f\n", x, y, a);
    }

  return 0;
}
