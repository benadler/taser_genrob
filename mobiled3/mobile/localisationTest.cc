//
//
//

#include <stdio.h>

#include "localisation/localisation.h"




int main (void)
{
  printf ("4 steps a 0.25s\n\n");

  {
    Localisation loc (2000, 5500, M_PI/2.0);

    loc.setDesiredVelocities (1.0, 1.0);

    for (int i=0; i<4; i++)
      {
	CVEC state;

	loc.predict (0.25, state);

	printf ("# state = %f, %f, %f\n", state[8], state[9], state[10]);
      }
  }

  printf ("\n10 steps a 0.1a\n\n");

  {
    Localisation loc (2000, 5500, M_PI/2.0);

    loc.setDesiredVelocities (1.0, 1.0);

    for (int i=0; i<10; i++)
      {
	CVEC state;

	loc.predict (0.1, state);

	printf ("# state = %f, %f, %f\n", state[8], state[9], state[10]);
      }
  }

  return 0;
}
