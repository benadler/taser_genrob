//
// mobileCalibrateTakeImages.cc
// (C) 09-11/2003 by TeSche (Torsten Scherer)
// <itschere@techfak.uni-bielefeld.de>
//

#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#include "tty/kbd.h"
#include "image/image.h"
#include "mobile/mobile.h"
#include "thread/tracelog.h"


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

#if 0

class CMEDIAN
{
private:

  int _dim, _size, _ommit;

  int *_used;
  double **_data;

public:

  CMEDIAN (const int dim,
	   const int size,
	   const int ommit);

  virtual ~CMEDIAN (void);

  void Input (const int idx, const double arg);

  double Output (const int idx) const;
};


//
//
//

CMEDIAN::CMEDIAN (const int dim,
		  const int size,
		  const int ommit)
{
  GTL ("CMEDIAN::CMEDIAN()");

  if ((dim < 0) ||
      (size < 0) ||
      (ommit < 0) ||
      (size > 0x0fffffff) ||   // `sizeof (double) * size' would wrap
      (ommit > 0x3ffffff))     // `2 * ommit' would wrap
    {
      GTLFATAL (("illegal parameters\n"));
      abort ();
    }

  if (2 * ommit >= size)   // `kernel would be empty'
    {
      GTLFATAL (("kernel would be empty\n"));
      abort ();
    }

  _dim = dim;
  _size = size;
  _ommit = ommit;

  //

  _used = (int *)malloc (dim * sizeof (int));
  _data = (double **)malloc (dim * sizeof (double *));

  if (!_used || !_data)
    {
      GTLFATAL (("out of memory\n"));
      abort ();
    }

  memset (_used, 0, dim * sizeof (int));

  for (int d=0; d<_dim; d++)
    {
      _data[d] = (double *)malloc (size * sizeof (double));

      if (!_data[d])
	{
	  GTLFATAL (("out of memory\n"));
	  abort ();
	}
    }
}


//
//
//

CMEDIAN::~CMEDIAN (void)
{
  GTL ("CMEDIAN::~CMEDIAN()");

  for (int d=_dim-1; d>=0; d--)
    {
      free (_data[d]);
    }

  free (_data);
  free (_used);
}


//
//
//

void CMEDIAN::Input (const int idx,
		     const double arg)
{
  GTL ("CMEDIAN::Input()");

  if ((idx < 0) || (idx >= _dim))
    {
      GTLFATAL (("index out of range 0..%i\n", _dim-1));
      abort ();
    }

  // first element?

  if (!_used[idx])
    {
      _used[idx] = 1;
      _data[idx][0] = arg;
      return;
    }

  // search

  int i;

  for (i=0; i<_used[idx]; i++)
    {
      if (_data[idx][i] < arg)
	{
	  break;
	}
    }

  // largest element?

  abort ();
}


//
//
//

double CMEDIAN::Output (const int idx) const
{
  GTL ("CMEDIAN::Output()");

  if ((idx < 0) || (idx >= _dim))
    {
      GTLFATAL (("index out of range 0..%i\n", _dim-1));
      abort ();
    }

  abort ();
}

#endif


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static void takeImages (CMOBILE &mobile,
			const int run,
			const int pos,
			const char * const prefix)
{
  GTLC ("takeImages()");

  double px = 0.0, py = 0.0, pa = 0.0;
  double pxs = 0.0, pys = 0.0, pas = 0.0;

#define NUM 100

  for (int i=0; i<NUM; i++)
    {
      double x, y, a;

      if (mobile.GetPosition (x, y, a) < 0)
	{
	  GTLFATAL (("GetPosition() failed\n"));
	  abort ();
	}

      px += x;
      py += y;
      pa += a;

      pxs += sqr (x);
      pys += sqr (y);
      pas += sqr (a);

      usleep (50000);
    }

  const double mx = px / (double)NUM;
  const double my = py / (double)NUM;
  const double ma = pa / (double)NUM;

  double vx = pxs / (double)NUM - sqr (mx);
  double vy = pys / (double)NUM - sqr (my);
  double va = pas / (double)NUM - sqr (ma);

  if (vx < 0.0) vx = 0.0;   // avoid numerical problems
  if (vy < 0.0) vy = 0.0;
  if (va < 0.0) va = 0.0;

  const double dx = sqrt (vx);
  const double dy = sqrt (vy);
  const double da = sqrt (va);

  printf ("%f %f %f   %f %f %f\n", mx, my, ma, dx, dy, da);
  fflush (stdout);

  char fName[64];
  //sprintf (fName, "%s-%02i.log", prefix-pos);   AAARRRGGGGLLL ::--((
  sprintf (fName, "%s-%02i.log", prefix, pos);
  FILE *fp = fopen (fName, "a");
  if (fp)
    {
      fprintf (fp, "%f %f %f   %f %f %f\n", mx, my, ma, dx, dy, da);
      fclose (fp);
    }

  static CIMAGE image;

  for (int i=0; i<10; i++)
    {
      image.Grab (TYPE_YUV888, SIZE_HALF_SPECIAL2, CHANNEL_SVHS);
      image.Show ();
    }

  sprintf (fName, "%s-%02i-%03i.ppm.gz", prefix, pos, run);

  if (image.SaveGzipPPM (fName) < 0)
    {
      GTLFATAL (("SaveGzipPPM() failed\n"));
      abort ();
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static inline double frandom (void)
{
  static bool first = true;

  if (first)
    {
      first = false;
      srandom (time (0));
    }

  return random () / (double)RAND_MAX;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

struct position
{
  float x, y, a;
};

#define NUMPOS 3

static struct position pos[NUMPOS] =
  {
    {2, 2, -135},
    {6, 2, -45},
    {6, 5, +45},
    //{2, 4, +135}
  };


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int main (void)
{
  GTLC ("main()");

  //

  CMOBILE mobile ("localhost");

  mobile.SetMode (TARGET);
  sleep (1);

  //

  for (int r=0; r<=1000; r++)
    {
      // 2.0 meter approach distance

      printf ("# run %i, approach distance = 2.0m\n", r+1);

      for (int p=0; p<NUMPOS; p++)
	{
	  printf ("# position %i\n", p+1);
	  fflush (stdout);

	  mobile.MoveViaIntermediate (pos[p].x, pos[p].y, pos[p].a, 2.0);
	  mobile.WaitForCompleted ();

	  mobile.SetMode (STANDBY);
	  sleep (1);

	  takeImages (mobile, r, p+1, "calib-d20");

	  mobile.SetMode (TARGET);
	  sleep (1);

	  mobile.Forward (-0.5);
	}

      // 1.0 meter approach distance

      printf ("# run %i, approach distance = 1.0m\n", r+1);

      for (int p=0; p<NUMPOS; p++)
	{
	  printf ("# position %i\n", p+1);
	  fflush (stdout);

	  mobile.MoveViaIntermediate (pos[p].x, pos[p].y, pos[p].a, 1.0);
	  mobile.WaitForCompleted ();

	  mobile.SetMode (STANDBY);
	  sleep (1);

	  takeImages (mobile, r, p+1, "calib-d10");

	  mobile.SetMode (TARGET);
	  sleep (1);

	  mobile.Forward (-0.5);
	}

#if 0
      // 0.5 meter approach distance

      printf ("# run %i, approach distance = 0.5m\n", r+1);

      for (int p=0; p<NUMPOS; p++)
	{
	  printf ("# position %i\n", p+1);

	  mobile.MoveViaIntermediate (pos[p].x, pos[p].y, pos[p].a, 0.5);
	  mobile.WaitForCompleted ();

	  mobile.SetMode (STANDBY);
	  sleep (1);

	  takeImages (mobile, r, p+1, "calib-d05");

	  mobile.SetMode (TARGET);
	  sleep (1);

	  mobile.Forward (-0.5);
	}
#endif

      // 0.0 meter approach distance

      printf ("# run %i, approach distance = 0.0m\n", r+1);

      for (int p=0; p<NUMPOS; p++)
	{
	  printf ("# position %i\n", p+1);
	  fflush (stdout);

	  mobile.Move (pos[p].x, pos[p].y, pos[p].a);
	  mobile.WaitForCompleted ();

	  mobile.SetMode (STANDBY);
	  sleep (1);

	  takeImages (mobile, r, p+1, "calib-d00");

	  mobile.SetMode (TARGET);
	  sleep (1);

	  mobile.Forward (-0.5);
	}
    }

  return 0;
}
