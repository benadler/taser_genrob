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

double Det (const CMAT &m)
{
  const int size = m.GetCols ();

  if (size != m.GetRows ())
    {
      fprintf (stderr, "matrix not suqare\n");
      abort ();
    }

  CMAT l, u;

  m.DecompLU (l, u);

  double ret = 1.0;

  for (int i=0; i<size; i++)
    {
      ret *= u[i][i];
    }

  return ret;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int main (void)
{
  srandom (time (0));

  for (int size=3; size<=200; size++)
    {
      //

      CMAT m (size, size);

      for (int y=0; y<size; y++)
	{
	  for (int x=0; x<size; x++)
	    {
		  m[y][x] = frandom ();
	    }
	}

#if 0
      while (42)   // memory leak?
	{
	  CMAT mi2 = m.InvLU ();
	}
#endif

      CMAT mi1 = m.Inv ();
      CMAT mi2 = m.InvLU ();

      printf ("%i %f %f\n",
	      size,
	      Det (m * mi1),
	      Det (m * mi2));
      fflush (stdout);
    }

  return 0;
}
