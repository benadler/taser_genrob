//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include "math/mat.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static double frandom (void)
{
  return 2.0 * ((random () / (double)RAND_MAX) - 0.5);
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int main (void)
{
  srandom (time (0));

  for (int size=3; size<=20; size++)
    {
      //

      CMAT m (size, size);

      struct timeval start;
      gettimeofday (&start, 0x0);

      for (int i=0; i<1000; i++)
	{
	  for (int y=0; y<size; y++)
	    {
	      for (int x=0; x<size; x++)
		{
		  m[y][x] = frandom ();
		}
	    }

	  CMAT mi = m.Inv ();
	}


      struct timeval stop;
      gettimeofday (&stop, 0x0);

      double t =
	(stop.tv_sec - start.tv_sec) +
	(stop.tv_usec - start.tv_usec) / 1000000.0;

      printf ("%i %f ", size, t / 1000.0);
      fflush (stdout);

      //

      start = stop;

      for (int i=0; i<1000; i++)
	{
	  for (int y=0; y<size; y++)
	    {
	      for (int x=0; x<size; x++)
		{
		  m[y][x] = frandom ();
		}
	    }

	  CMAT mi = m.InvLU ();
	}

      gettimeofday (&stop, 0x0);

      t =
	(stop.tv_sec - start.tv_sec) +
	(stop.tv_usec - start.tv_usec) / 1000000.0;

      printf ("%f\n", t / 1000.0);

      //
    }

  return 0;
}
