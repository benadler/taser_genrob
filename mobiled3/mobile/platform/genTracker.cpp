//
//
//

#error "++itschere won't work as from 20021113"

#include <math.h>
#include <unistd.h>
#include <sys/time.h>

#include "mobile/genbase.h"
#include "math/mat.h"


//
//
//

int main (void)
{
  //

  printf ("#\n");
  printf ("# output format is:\n");
  printf ("#\n");
  printf ("# zeit[s], genrob-x, genrob-y, genrob-sin(a), genrob-cos(a), genrob-error, tesche-x, tesche-y, tesche-sin(a), tesche-cos(a), tesche-error\n");
  printf ("#\n\n");

  //

  CGENBASE genbase ("localhost");

  //genbase.SetMode (CGENBASE::STANDBY);
  //genbase.SetFrame (-1.0, -1.0, -90.0);
  //genbase.SetFrame (-0.8, -0.8, -75.0);

  struct timeval start;
  gettimeofday (&start, 0x0);

  while (42)
    {
#if 1
      usleep (100000);
#endif

      //

      struct timeval now;

      gettimeofday (&now, 0x0);

      double t = (now.tv_sec - start.tv_sec) +
	(now.tv_usec - start.tv_usec) / 1000000.0;

      //

      double x, y, a;

      if (genbase.GetFrame (x, y, a) < 0)
	{
	  printf ("GetFrame() failed\n");
	  abort ();
	}

      //

      double xLSF, yLSF, aLSF;
      double eGenRob, eTeSche;

      if (genbase.GetFrameLSF (xLSF, yLSF, aLSF, &eGenRob, &eTeSche) < 0)
	{
	  printf ("# GetFrameLSF() failed\n");
	  continue;
	}

      //

      printf ("%.3f  %.3f %.3f %.4f %.4f %.4f  %.3f %.3f %.4f %.4f %.4f\n\n",
	      t,
	      x, y, sin (3.14159 * a / 180.0), cos (3.14159 * a / 180.0), eGenRob,
	      xLSF, yLSF, sin (3.14159 * aLSF / 180.0), cos (3.14159 * aLSF / 180.0), eTeSche);
    }

  return 0;
}
