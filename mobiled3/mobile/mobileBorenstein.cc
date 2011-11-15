//
// mobileBorenstein.cc
// (C) 07-12/2003 by TeSche (Torsten Scherer)
// <itschere@techfak.uni-bielefeld.de>
//

#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#include "thread/tracelog.h"

#include "mobile/mobile.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static void borenstein (CMOBILE &mobile, const double scale)
{
  static double globX[4] = {2, 4, 4, 2};
  static double globY[4] = {2, 2, 4, 4};
  static double globA[4] = {0, 90, 180, -90};

  mobile.SetScale (scale, scale);

  mobile.Move (globX[0], globY[0], globA[0]);

  mobile.WaitForCompleted ();

  mobile.SetPosition (globX[0], globY[0], globA[0]);

  static int cnt = 1;
  char buf[128];
  snprintf (buf, 128, "borenstein-%02i.log", cnt++);

  FILE *fp = fopen (buf, "w");

  //

  mobile.Move (globX[1], globY[1], globA[1]);

  while (!mobile.IsCompleted ())
    {
      double xp, yp, ap, xpOdo, ypOdo, apOdo;

      mobile.GetPosition (xp, yp, ap);
      mobile.GetPositionOdo (xpOdo, ypOdo, apOdo);

      fprintf (fp,
	       "%.3f %.3f %.2f  %.3f %.3f %.2f\n",
	       xp, yp, ap,
	       xpOdo, ypOdo, apOdo);
    }

  //

  mobile.Move (globX[2], globY[2], globA[2]);

  while (!mobile.IsCompleted ())
    {
      double xp, yp, ap, xpOdo, ypOdo, apOdo;

      mobile.GetPosition (xp, yp, ap);
      mobile.GetPositionOdo (xpOdo, ypOdo, apOdo);

      fprintf (fp,
	       "%.3f %.3f %.2f  %.3f %.3f %.2f\n",
	       xp, yp, ap,
	       xpOdo, ypOdo, apOdo);
    }

  //

  mobile.Move (globX[3], globY[3], globA[3]);

  while (!mobile.IsCompleted ())
    {
      double xp, yp, ap, xpOdo, ypOdo, apOdo;

      mobile.GetPosition (xp, yp, ap);
      mobile.GetPositionOdo (xpOdo, ypOdo, apOdo);

      fprintf (fp,
	       "%.3f %.3f %.2f  %.3f %.3f %.2f\n",
	       xp, yp, ap,
	       xpOdo, ypOdo, apOdo);
    }

  //

  mobile.Move (globX[0], globY[0], globA[0]);

  while (!mobile.IsCompleted ())
    {
      double xp, yp, ap, xpOdo, ypOdo, apOdo;

      mobile.GetPosition (xp, yp, ap);
      mobile.GetPositionOdo (xpOdo, ypOdo, apOdo);

      fprintf (fp,
	       "%.3f %.3f %.2f  %.3f %.3f %.2f\n",
	       xp, yp, ap,
	       xpOdo, ypOdo, apOdo);
    }

  fclose (fp);
}


///////////////////////////////////////////////////////////////////////////////
//
// old
//
///////////////////////////////////////////////////////////////////////////////

#if 0

static void borenstein0 (CMOBILE &mobile)
{
  const double x0 = 2.0;
  const double y0 = 5.0;
  const double a0 = 90.0;

#define LAENGE 3.0

  while (42)
    {
      mobile.Move (x0, y0, a0);
      mobile.WaitForCompleted ();

      double xp, yp, ap;
      mobile.GetPosition (xp, yp, ap);
      printf ("%f %f %f\n", xp, yp, ap);

      mobile.Move (x0 + LAENGE, y0, a0 - 90.0);
      mobile.WaitForCompleted ();

      mobile.Move (x0 + LAENGE, y0 - LAENGE, a0 - 180.0);
      mobile.WaitForCompleted ();

      mobile.Move (x0, y0 - LAENGE, a0 + 90.0);
      mobile.WaitForCompleted ();
    }
}


static void borenstein1 (CMOBILE &mobile)
{
  while (42)
    {
      mobile.Move (2, 2, -90);
      mobile.WaitForCompleted ();

      mobile.Move (6, 2, 0);
      mobile.WaitForCompleted ();

      mobile.Move (6, 4, 90);
      mobile.WaitForCompleted ();

      mobile.Move (2, 4, 180);
      mobile.WaitForCompleted ();
    }
}


static double frandom (void)   // 0..1
{
  static int first = 1;

  if (first)
    {
      first = 0;
      srandom (time (0));
    }

  return random () / (double)RAND_MAX;
}


static void borenstein2 (CMOBILE &mobile)
{
  const double x0 = 2.0;
  const double y0 = 2.0;
  const double xLength = 4.0;
  const double yLength = 2.0;

  double xp, yp, ap;
  mobile.GetPosition (xp, yp, ap);

  while (42)
    {
      const double xe = x0 + xLength * frandom ();
      const double ye = y0 + yLength * frandom ();

      const double ae = atan2 (ye - yp, xe - xp) * 180.0 / M_PI;

      mobile.Move (xe, ye, ae);
      mobile.WaitForCompleted ();

      mobile.GetPosition (xp, yp, ap);
      printf ("%f,%f,%f -> %f,%f,%f\n", xe, ye, ae, xp, yp, ap);
    }
}

#endif


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int main (void)
{
  printf ("#\n");
  printf ("# actually, this isn't the real borenstein test...\n");
  printf ("#\n");

#if 1
  _globGtlEnable = 1;
#endif

  CMOBILE mobile ("localhost");

  borenstein (mobile, 1.0);
  borenstein (mobile, 1.0);
  borenstein (mobile, 1.0);

  borenstein (mobile, 0.5);
  borenstein (mobile, 0.5);
  borenstein (mobile, 0.5);

  borenstein (mobile, 0.25);
  borenstein (mobile, 0.25);
  borenstein (mobile, 0.25);

  borenstein (mobile, 0.125);
  borenstein (mobile, 0.125);
  borenstein (mobile, 0.125);

  return 0;
}
