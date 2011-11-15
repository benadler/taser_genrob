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

  CMAT m (10, 10);

  for (int y=0; y<10; y++)
    {
      for (int x=0; x<10; x++)
	{
	  m[y][x] = frandom ();
	}
    }

  while (42)   // memory leak?
    {
      CMAT mi2 = m.InvLU ();
    }

  return 0;
}