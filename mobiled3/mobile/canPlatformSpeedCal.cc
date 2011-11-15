//
//
//

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include "mobile/canPlatform.h"

#include "thread/tracelog.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static inline double diffTimevalSeconds (const struct timeval &t1,
					 const struct timeval &t2)
{
  return (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec) / 1000000.0;
}


static inline double diffTimevalMucroSeconds (const struct timeval &t1,
					      const struct timeval &t2)
{
  return 1000000.0 * (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec);
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static void run (CCANPLATFORM &canPlatform)
{
  const int minspeed = 1;
  const int maxspeed = 7;

  double magic = 0.0; // magic speed value

  for (int speed=minspeed; speed<=maxspeed; speed++)
    {
      struct timeval start, stop;
      double deltaAngleLeft = 0.0, deltaAngleRight = 0.0;

      for (int cycle=0; cycle<250; cycle++)   // 5s
	{
	  canPlatform.SetWorldVelocity (speed, speed);   // rad/s

	  usleep (20000);

	  if (cycle == 50)   // allow 1s to ramp up
	    {
	      gettimeofday (&start, 0x0);

	      double dl, dr;
	      canPlatform.GetWorldState (dl, dr);
	    }

	  if (cycle > 50)
	    {
	      gettimeofday (&stop, 0x0);

	      double dl, dr;
	      canPlatform.GetWorldState (dl, dr);

	      deltaAngleLeft += dl;
	      deltaAngleRight += dr;
	    }
	}

      const double dt = diffTimevalSeconds (start, stop);

      const double velLeft = deltaAngleLeft / dt;
      const double velRight = deltaAngleRight / dt;

      const double magicLeft  = velLeft  / (double)speed;
      const double magicRight = velRight / (double)speed;

      printf ("%f : dt=%f  left=%f (%f)  right=%f (%f)\n",
	      (double)speed,
	      dt,
	      velLeft,
	      magicLeft,
	      velRight,
	      magicRight);

      magic += (magicLeft + magicRight) / 2.0;
    }

  magic /= (double)(maxspeed - minspeed + 1);

  printf ("\nTry something like %f for MAGIC.\n", magic) ;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int main (void)
{
  fprintf (stderr, "\n");
  fprintf (stderr, "BE SURE TO HAVE COMPILED `canPlatform.cc' WITH `#undef MAGIC'!!!\n");
  fprintf (stderr, "\n");

  CCANPLATFORM canPlatform;

  canPlatform.StartMotors ();

  run (canPlatform);

  return 0;
}
