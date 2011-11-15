//
//
//

#include <time.h>
#include <stdio.h>
#include <unistd.h>

#include "math/mat.h"
#include "util/normal.h"
#include "image/image.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#define SIZE 1234

float x[SIZE], y[SIZE];

#define WIDTH 700
#define HEIGHT 700
#define SCALE 50


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static inline double sqr (const double arg)
{
  return arg * arg;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static void test (CIMAGE &image,
		  const double var1,
		  const double var2,
		  const double angle)
{
  //
  // generate two linear independant distributions
  //

  printf ("two linear independent distributions:\n");

  static Normal normal (0.0, 1.0);

  const double x0 = normal ();
  const double y0 = normal ();

  for (int i=0; i<SIZE; i++)
    {
      x[i] = x0 + var1 * normal ();
      y[i] = y0 + var2 * normal ();
    }

  //

  printf ("  mean values:\n");

  double mx = 0.0;
  double my = 0.0;

  for (int i=0; i<SIZE; i++)
    {
      mx += x[i];
      my += y[i];
    }

  mx /= (double)SIZE;
  my /= (double)SIZE;

  printf ("    mx = %f\n", mx);
  printf ("    my = %f\n", my);

  //

  printf ("  variances:\n");

  double vx = 0.0;
  double vy = 0.0;

  for (int i=0; i<SIZE; i++)
    {
      vx += sqr (x[i] - mx);
      vy += sqr (y[i] - my);
    }

  vx /= (double)SIZE;
  vy /= (double)SIZE;

  printf ("    vx = %f\n", vx);
  printf ("    vy = %f\n", vy);

  //
  // generate distribution
  //

  printf ("rotated distribution (%f°):\n", angle);

  image.SetColor (100);

  FILE *fp = fopen ("covar.gp", "w");

  for (int i=0; i<SIZE; i++)
    {
      CVEC p (x[i], y[i]);

      CMAT rotz (2, 2);

      rotz[0][0] = cos (M_PI * angle / 180.0);
      rotz[0][1] = sin (M_PI * angle / 180.0);
      rotz[1][0] = -sin (M_PI * angle / 180.0);
      rotz[1][1] = cos (M_PI * angle / 180.0);

      p = rotz * p;

      x[i] = p[0];
      y[i] = p[1];

      fprintf (fp, "%f %f\n", x[i], y[i]);

      image.Plot ((WIDTH>>1) + SCALE * x[i],
		  (HEIGHT>>1) + SCALE * y[i]);
    }

  fclose (fp);

  //
  // remove center-of-gravity
  //

  printf ("  mean values:\n");

  double ex = 0.0;
  double ey = 0.0;

  for (int i=0; i<SIZE; i++)
    {
      ex += x[i];
      ey += y[i];
    }

  ex /= (double)SIZE;
  ey /= (double)SIZE;

  printf ("    ex = %f\n", ex);
  printf ("    ey = %f\n", ey);

  //
  // compute covariance
  //

  printf ("  covariance matrix (should be symmetric):\n");

  double sum = 0.0;

  for (int i=0; i<SIZE; i++)
    {
      sum += x[i] * x[i];
    }

  double vxx = sum / (double)SIZE - ex * ex;

  printf ("    vxx = %f\n", vxx);

  //

  sum = 0.0;

  for (int i=0; i<SIZE; i++)
    {
      sum += x[i] * y[i];
    }

  double vxy = sum / (double)SIZE - ex * ey;

  printf ("    vxy = %f\n", vxy);

  //

  sum = 0.0;

  for (int i=0; i<SIZE; i++)
    {
      sum += y[i] * x[i];
    }

  double vyx = sum / (double)SIZE - ey * ex;

  printf ("    vyx = %f\n", vyx);

  //

  sum = 0.0;

  for (int i=0; i<SIZE; i++)
    {
      sum += y[i] * y[i];
    }

  double vyy = sum / (double)SIZE - ey * ey;

  printf ("    vyy = %f\n", vyy);

  //
  //
  //

  printf ("  interesting attribute: vx + vy = vxx + vyy\n");

  printf ("    vx  + vy  = %f\n", vx + vy);
  printf ("    vxx + vyy = %f\n", vxx + vyy);

  //
  // p/q solution of the eigenvalues
  //

  printf ("  eigenvalues of covariance matrix:\n");

  const double p = - (vxx + vyy);
  const double q = vxx * vyy - vxy * vyx;

  const double d = sqr (p) / 4.0 - q;

  if (d <= 0.0)
    {
      printf ("    don't exist???\n");
    }
  else
    {
      const double e1 = -p / 2.0 + sqrt (d);
      const double e2 = -p / 2.0 - sqrt (d);

      if ((e1 < 0.001) || (e2 < 0.001))
	{
	  printf ("    too small, distribution with no variance\n");
	  abort ();
	}

      printf ("    e1 = %f (this should be the larger one)\n", e1);
      printf ("    e2 = %f\n", e2);

      //

      printf ("    eigenvalue %f:\n", e1);
      {
	const double fx = vxx - e1 - vyx;
	const double fy = vyy - e1 - vxy;
	printf ("      fx = %f, fy = %f\n", fx, fy);
	if (fabs (fy) > 0.001)
	  {
	    printf ("      y = %f * x\n", fx / fy);
	  }
	const double angle = atan2 (-fx, -fy);
	printf ("      angle = %f° (against x-axis)\n", 180.0 * angle / M_PI);

	image.SetColor (255);

	image.Ellipse ((WIDTH >> 1) + ex * SCALE,
		       (HEIGHT >> 1) + ey * SCALE,
		       angle,
		       SCALE * sqrt (e1),
		       SCALE * sqrt (e2));
      }

      //

      printf ("    eigenvalue %f:\n", e2);
      {
	const double fx = vxx - e2 - vyx;
	const double fy = vyy - e2 - vxy;
	printf ("      fx = %f, fy = %f\n", fx, fy);
	if (fabs (fy) > 0.001)
	  {
	    printf ("      y = %f * x\n", fx / fy);
	  }
	printf ("      angle = %f° (against x-axis)\n",
		180.0 * atan2 (-fx, -fy) / M_PI);
      }
    }

  //
  //
  //
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

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


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int main (void)
{
  //
  //
  //

  CIMAGE image (TYPE_GRAY8, WIDTH, HEIGHT);

  //
  //
  //

  while (42)
    {
      image.SetColor (0);
      image.Pbox (0,0, WIDTH-1, HEIGHT-1);

      test (image,
	    1.9 * frandom () + 0.1,
	    1.9 * frandom () + 0.1,
	    2.0 * M_PI * frandom ());

      image.Show ();

      usleep (333333);
    }
}
