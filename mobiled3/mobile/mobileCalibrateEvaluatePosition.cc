//
// mobileCalibrateEvaluatePosition.cc
// (C) 02/2004 by TeSche (Torsten Scherer)
// <itschere@techfak.uni-bielefeld.de>
//
// only evaluates the position information, which does not say anything about
// where the platform really was!
//

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "thread/tracelog.h"

#include "arrays.h"

#undef VERBOSE


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static char *readline (double &px, double &py, double &pa,
		       FILE *fp)
{
  static char buf[256];
  char *ret;

  while ((ret = fgets (buf, 256, fp)))
    {
#if 1
      break;
#else
      if (buf[0] != '#')
	{
	  break;
	}
#endif
    }

  if (ret)
    {
#if 1
      if (*ret == '#')
	{
	  ret += 2;
	}
#endif

      sscanf (ret, "%lf %lf %lf", &px, &py, &pa);
    }

  return ret;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#define CENTER_ON_DATA

int main (int argc, char **argv)
{
#if 0
  _globGtlEnable = 1;
#endif

#ifdef CENTER_ON_DATA

  if (argc != 2)
    {
      fprintf (stderr, "usage: %s <namepref>\n", argv[0]);
      fprintf (stderr, "\twhere:\n");
      fprintf (stderr, "\t<namepref> is the prefix of the logfile (including paths)\n");

      return -1;
    }

#else

  if (argc != 4)
    {
      fprintf (stderr, "usage: %s <namepref> <x0> <y0>\n", argv[0]);
      fprintf (stderr, "\twhere:\n");
      fprintf (stderr, "\t<namepref> is the prefix of the logfile (including paths)\n");
      fprintf (stderr, "\t<x0> is the intended x-position\n");
      fprintf (stderr, "\t<y0> is the intended y-position\n");

      return -1;
    }

#endif

  GTLC ("main()");

  //
  //
  //

  const int maxNameLen = strlen (argv[1]) + 16;
  char fName[maxNameLen];

  snprintf (fName, maxNameLen, "%s.log", argv[1]);

  FILE *fp = fopen (fName, "r");

  if (!fp)
    {
      GTLFATAL (("file \"%s\" not found\n", fName));
      abort ();
    }

#ifndef CENTER_ON_DATA

  const double x0 = atof (argv[2]);
  const double y0 = atof (argv[3]);

#endif

  //
  // now enter the loop
  //

  CPOSARRAY posPltf;
  double px, py, pa;

  while (readline (px, py, pa, fp))
    {
      // base position [m]

#ifdef VERBOSE
      printf ("platform     : %+8.5f %+8.5f %+8.4f\n", px, py, pa);
#endif
      posPltf.Add (px, py, pa);
    }

  fclose (fp);

  const int cnt = posPltf.GetSize ();
  printf ("# number of runs: %i\n", cnt);

  //
  // remove averages
  //

  double xAvg, yAvg, aAvg, xDev, yDev, aDev;

  posPltf.Avg (xAvg, yAvg, aAvg);
  posPltf.Dev (xDev, yDev, aDev);
  printf ("# average platform position [m/°]     : %+8.5f %+8.5f %+8.4f   stddev : %+8.5f %+8.5f %+8.4f\n",
	  xAvg, yAvg, aAvg, xDev, yDev, aDev);

#ifdef CENTER_ON_DATA

  const double x0 = xAvg;
  const double y0 = yAvg;

#endif

  //
  // plot results
  //
  // (eigentlich ja überflüssig. wir könnten ja auch gleich aus dem logfile
  // plotten, aber was soll's...)
  //

  printf ("# -----------------------------------------------------------------------------\n");

  printf ("# generating gnuplot data (%s.gp_data)\n", argv[1]);

  const double scatterPosLimit = 0.02;
  int scatterPosDrop = 0;

  snprintf (fName, maxNameLen, "%s.gp_data", argv[1]);

  if (!(fp = fopen (fName, "w")))
    {
      GTLFATAL (("fopen() failed\n"));
      abort ();
    }

  for (int i=0; i<cnt-1; i++)
    {
      fprintf (fp,
	       "%f %f %f\n",
	       posPltf.X[i],
	       posPltf.Y[i],
	       posPltf.A[i]);

      if ((fabs (posPltf.X[i] - x0) > scatterPosLimit) ||
	  (fabs (posPltf.Y[i] - y0) > scatterPosLimit))
	{
	  scatterPosDrop++;
	}
    }

  fclose (fp);

  //
  //
  //

  printf ("# generating gnuplot commands (%s.gp_cmd)\n", argv[1]);

  snprintf (fName, maxNameLen, "%s.gp_cmd", argv[1]);

  if (!(fp = fopen (fName, "w")))
    {
      GTLFATAL (("fopen() failed\n"));
      abort ();
    }

  // scatter plot

  fprintf (fp, "set key box\n");
  fprintf (fp, "set parametric\n");
  //fprintf (fp, "set size square\n");
  fprintf (fp, "set size ratio -1\n");
  fprintf (fp, "set grid\n");
  fprintf (fp, "set xtics 0.01\n");
  fprintf (fp, "set ytics 0.01\n");
  fprintf (fp, "set title \"scatter plot (%i extreme outlier%s removed)\"\n", scatterPosDrop, scatterPosDrop != 1 ? "s" : "");
  fprintf (fp, "set xlabel \"x-axis [m]\"\n");
  fprintf (fp, "set ylabel \"y-axis [m]\"\n");

  fprintf (fp, "set term postscript eps color solid\n");
  fprintf (fp, "set output \"%s-pos-scatter.eps\"\n", argv[1]);

  //fprintf (fp, "set xrange [%.0f:%.0f]\n", -scatterLimit, scatterLimit);
  //fprintf (fp, "set yrange [%.0f:%.0f]\n", -scatterLimit, scatterLimit);

  fprintf (fp, "set label \"# <- target\" at %f,%f\n",
	   floor (x0 + 0.5), floor (y0 + 0.5));

  fprintf (fp, "set xrange [%f:%f]\n", x0-scatterPosLimit, x0+scatterPosLimit);
  fprintf (fp, "set yrange [%f:%f]\n", y0-scatterPosLimit, y0+scatterPosLimit);

  fprintf (fp, "plot \"%s.gp_data\" using 1:2 title \"position\"\n");

  fclose (fp);

  //
  //
  //

  printf ("# executing gnuplot commands\n");

  char buf[128];
  snprintf (buf, 128, "gnuplot %s.gp_cmd", argv[1]);
  system (buf);

  snprintf (buf, 128, "eps2square %s-pos-scatter.eps", argv[1]);
  system (buf);

  printf ("# converting eps to pdf\n");

  snprintf (buf, 128, "epstopdf %s-pos-scatter.eps", argv[1]);
  system (buf);

  //
  //
  //

  return 0;
}
