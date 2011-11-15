//
//
//

#include <stdio.h>
#include <unistd.h>

#include "mobile/mobile.h"
#include "infobase/infobase.h"

#include "thread/tracelog.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static void rotate (CMOBILE &mobile, const degree_t angle)
{
  GTLC ("rotate()");

  printf ("rotating %f°\n", angle);

  if (mobile.RotateAngle (angle))
    {
      printf ("failed \n");
      exit (-1);
    }

  if (mobile.WaitForCompleted ())
    {
      printf ("failed \n");
      exit (-1);
    }

  write (fileno (stdout), "\007", 1);

  sleep (1);
}



///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static void move (CMOBILE &mobile, const meter_t x, const meter_t y, const degree_t theta)
{
  GTLC ("move()");

  printf ("going to: %f, %f, %f\n", x, y, theta);

  double xErr, yErr, aErr;

  if (mobile.MoveDirect (x, y, theta,
			 xErr, yErr, aErr) < 0)
    {
      printf ("failed\n");
      exit (-1);
    }

  printf ("error: %f, %f, %f\n", xErr, yErr, aErr);
  write (fileno (stdout), "\007", 1);

  sleep (1);
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static void usage (void)
{
  fprintf (stderr, "usage: mobileMove [-d] [-m <hostname>] [-h] {<devname>}\n");
  fprintf (stderr, "\twhere\n");
  fprintf (stderr, "\t-d : enable debugging\n");
  fprintf (stderr, "\t-m <hostname> : connect to mobile <hostname>\n");
  fprintf (stderr, "\t-h : print this help\n");

  exit (-1);
}


int main (int argc, char **argv)
{
  GTLC ("MobileRotationTest::main()");

  int c;
  char *host = 0x0;

  while ((c = getopt (argc, argv, "adm:")) >= 0)
    {
      switch (c)
	{
	case 'd':
	  _globGtlEnable = 1;
	  break;

	case 'm':
	  host = optarg;
	  break;

	case 'h':
	default:
	  usage ();
	}
    }

  //

  CMOBILE mobile (host);

  FILE *log;

  int i = 0;

  for (int a=1; a < 180; ++a)
    {
      move (mobile, 11.0, 13.0, 0.0);  // somewhere
      move (mobile, 12.0, 13.0, 0.0);

      double x, y, theta,
             x_avg1 = 0, y_avg1 = 0, theta_avg1 = 0,
             x_avg2 = 0, y_avg2 = 0, theta_avg2 = 0;

      for (i=0; i<100; ++i)
      {
        y = 0, x = 0, theta = 0;

        mobile.GetPosition (x, y, theta);

        x_avg1 += x;
        y_avg1 += y;
        theta_avg1 += theta;

        usleep (30000);
      }

      x_avg1 /= 100.0;
      y_avg1 /= 100.0;
      theta_avg1 /= 100.0;

      // ROTATE

      rotate (mobile, (float) a);

      for (i=0; i<100; ++i)
      {
        y = 0, x = 0, theta = 0;

        mobile.GetPosition (x, y, theta);

        x_avg2 += x;
        y_avg2 += y;
        theta_avg2 += theta;

        usleep (30000);
      }

      x_avg2 /= 100.0;
      y_avg2 /= 100.0;
      theta_avg2 /= 100.0;

      printf ("%i   %f %f %f   %f %f %f\n",
              a,
              x_avg1,
              y_avg1,
              theta_avg1,
              x_avg2,
              y_avg2,
              theta_avg2);

      log = fopen ("rotation_avg.out", "a");

      fprintf (log,
               "%i   %f %f %f   %f %f %f\n",
               a,
               x_avg1,
               y_avg1,
               theta_avg1,
               x_avg2,
               y_avg2,
               theta_avg2);

      fflush (log);

      fclose(log);

      move (mobile, 11.0, 13.0, 0.0);  // somewhere
      move (mobile, 12.0, 13.0, 0.0);

      x_avg1 = 0, y_avg1 = 0, theta_avg1 = 0;
      x_avg2 = 0, y_avg2 = 0, theta_avg2 = 0;

      for (i=0; i<100; ++i)
      {
        y = 0, x = 0, theta = 0;

        mobile.GetPosition (x, y, theta);

        x_avg1 += x;
        y_avg1 += y;
        theta_avg1 += theta;

        usleep (30000);
      }

      x_avg1 /= 100.0;
      y_avg1 /= 100.0;
      theta_avg1 /= 100.0;

      // ROTATE

      rotate (mobile, (float) -a);

      for (i=0; i<100; ++i)
      {
        y = 0, x = 0, theta = 0;

        mobile.GetPosition (x, y, theta);

        x_avg2 += x;
        y_avg2 += y;
        theta_avg2 += theta;

        usleep (30000);
      }

      x_avg2 /= 100.0;
      y_avg2 /= 100.0;
      theta_avg2 /= 100.0;

      printf ("%i   %f %f %f   %f %f %f\n",
              -a,
              x_avg1,
              y_avg1,
              theta_avg1,
              x_avg2,
              y_avg2,
              theta_avg2);

      log = fopen ("rotation_avg.out", "a");

      fprintf (log,
               "%i   %f %f %f   %f %f %f\n",
               -a,
               x_avg1,
               y_avg1,
               theta_avg1,
               x_avg2,
               y_avg2,
               theta_avg2);

      fflush (log);

      fclose (log);
    }

  printf ("test of rotation finished");

  return 0;
}
