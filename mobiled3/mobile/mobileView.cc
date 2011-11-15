//
//
//

#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#include "mobile/mobile.h"

#include "image/image.h"
#include "thread/tracelog.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#define BACKLOGSIZE 127
#undef EXTENDED_LOGGING


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#if 1
// ++itschere20030730: currently the graphics display is quite a bit mocked
// up. we use a define FLIPXY to, yeah, flip the x- and y-axis. we should of
// course simply allow an arbitrary rotation angle, but instead we have
// duplicate code at an awful lot of places... :-(
#define FLIPXY
#define a0 0.0
#else
#define a0 90.0
#endif


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

static int ComputeEllipse (double &angle,
			   double &ev1,
			   double &ev2,
			   const double vxx,
			   const double vxy,
			   const double vyx,
			   const double vyy)
{
  GTLC ("ComputeEllipse()");

  const double p = - (vxx + vyy);
  const double q = vxx * vyy - vxy * vyx;

  const double d = sqr (p) / 4.0 - q;

  if (d < 0.0)
    {
      GTLFATAL (("doesn't exist???\n"));
      GTLFATAL (("  vxx0%f, vxy=%f, vyx=%f, vyy=%f\n", vxx, vxy, vyx, vyy));
      return -1;
    }

  //

  ev1 = -p / 2.0 + sqrt (d);
  ev2 = -p / 2.0 - sqrt (d);

  if ((ev1 < 0.001) || (ev2 < 0.001))
    {
      GTLFATAL (("distribution with no variance (in at least one dimension)\n"));
      return -1;
    }

  if (ev1 < ev2)
    {
      GTLFATAL (("e1<e2???\n"));
      return -1;
    }

  //

  const double fx = vxx - ev1 - vyx;
  const double fy = vyy - ev1 - vxy;

  angle = atan2 (-fx, -fy);

  //

  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static void View (CMOBILE &mobile)
{
  GTLC ("View()");

  //
  // get size of display (in pixel)
  //

  int displayWidth, displayHeight;

  CIMAGE::GetDisplaySize (displayWidth, displayHeight);

  GTLFATAL (("display is %i x %i pixel\n", displayWidth, displayHeight));

  //
  // get size of map (in meter)
  //

  CMOBILE::CMAP map;

  mobile.GetMap (map);

  CVEC::datatype_t *line = &map.line;

#ifdef FLIPXY
  double mapXmin = line[1];
  double mapYmin = line[0];
  double mapXmax = line[1];
  double mapYmax = line[0];
#else
  double mapXmin = line[0];
  double mapYmin = line[1];
  double mapXmax = line[0];
  double mapYmax = line[1];
#endif

  line += 2;

  for (int i=1; i<2*map.numLines; i++)
    {
#ifdef FLIPXY
      const double x = line[1];
      const double y = line[0];
#else
      const double x = line[0];
      const double y = line[1];
#endif

      if (x < mapXmin) mapXmin = x;
      if (x > mapXmax) mapXmax = x;
      if (y < mapYmin) mapYmin = y;
      if (y > mapYmax) mapYmax = y;

      line += 2;
    }

  GTLFATAL (("map extends from x=%f..%f, y=%f..%f\n",
	     mapXmin, mapXmax,
	     mapYmin, mapYmax));

  //
  // compute scale
  //

  mapXmin -= 0.05;   // add 5cm
  mapXmax += 0.05;
  mapYmin -= 0.05;
  mapYmax += 0.05;

  const int tmpWidth  = 9 * displayWidth  / 10;
  const int tmpHeight = 9 * displayHeight / 10;

  const double mapWidth  = mapXmax - mapXmin;
  const double mapHeight = mapYmax - mapYmin;

  double tmpXscale = tmpWidth  / mapWidth;
  double tmpYscale = tmpHeight / mapHeight;

  GTLFATAL (("scales = %f %f\n", tmpXscale, tmpYscale));

  const double SCALE = tmpXscale < tmpYscale ? tmpXscale : tmpYscale;

  const int WIDTH = (int)(SCALE * mapWidth);
  const int HEIGHT = (int)(SCALE * mapHeight);

#ifdef FLIPXY
  const double x0 = (mapYmin + mapYmax) / 2.0;
  const double y0 = (mapXmin + mapXmax) / 2.0;
#else
  const double x0 = (mapXmin + mapXmax) / 2.0;
  const double y0 = (mapYmin + mapYmax) / 2.0;
#endif

  //
  //
  //

  CIMAGE imageRaw (TYPE_RGB888, WIDTH, HEIGHT);

  imageRaw.SetColor (0, 0, 0);
  imageRaw.Pbox (0, 0, WIDTH-1, HEIGHT-1);

#ifdef FLIPXY
  int xm = HEIGHT >> 1;
  int ym = WIDTH >> 1;
#else
  int xm = WIDTH >> 1;
  int ym = HEIGHT >> 1;
#endif

  //
  //
  //

#if 0

  imageRaw.SetColor (50, 200, 50);

#ifdef FLIPXY
  imageRaw.Pcircle (y0 / SCALE + ym,
		    x0 / SCALE + xm,
		    10);
#else
  imageRaw.Pcircle (x0 / SCALE + xm,
		    y0 / SCALE + ym,
		    10);
#endif

#endif

  //
  // draw meter grid
  //

  imageRaw.SetColor (50, 50, 50);

  {
#ifdef FLIPXY

      double dx0 = (0 - xm) / (double)SCALE + x0;
      double dxe = (HEIGHT-1 - xm) / (double)SCALE + x0;
      int ix0 = (int)ceil (dx0);
      int ixe = (int)floor (dxe);

      double dy0 = (0 - ym) / (double)SCALE + y0;
      double dye = (WIDTH-1 - ym) / (double)SCALE + y0;
      int iy0 = (int)ceil (dy0);
      int iye = (int)floor (dye);

      for (int y=iy0; y<=iye; y++)
	{
	  int yPlotInt = (int)(SCALE * (y - y0)) + ym;
	  imageRaw.Vline (yPlotInt, 0, HEIGHT-1);
	}

      for (int x=ix0; x<=ixe; x++)
	{
	  int xPlotInt = (int)(SCALE * (x - x0)) + xm;
	  imageRaw.Hline (0, xPlotInt, WIDTH-1);
	}

#else

      double dx0 = (0 - xm) / (double)SCALE + x0;
      double dxe = (WIDTH-1 - xm) / (double)SCALE + x0;
      int ix0 = (int)ceil (dx0);
      int ixe = (int)floor (dxe);

      double dy0 = (0 - ym) / (double)SCALE + y0;
      double dye = (HEIGHT-1 - ym) / (double)SCALE + y0;
      int iy0 = (int)ceil (dy0);
      int iye = (int)floor (dye);

      for (int x=ix0; x<=ixe; x++)
	{
	  int xPlotInt = (int)(SCALE * (x - x0)) + xm;
	  imageRaw.Vline (xPlotInt, 0, HEIGHT-1);
	}

      for (int y=iy0; y<=iye; y++)
	{
	  int yPlotInt = (int)(SCALE * (y - y0)) + ym;
	  imageRaw.Hline (0, yPlotInt, WIDTH-1);
	}
#endif

      //printf ("%f (%i) .. %f (%i)\n", dx0, ix0, dxe, ixe);
      //printf ("%f (%i) .. %f (%i)\n", dy0, iy0, dye, iye);
  }

  //
  // draw map (lines only)
  //

#if 0

  CMOBILE::CMAP map;

  mobile.GetMap (map);

#endif

  imageRaw.SetColor (200, 200, 200);

  line = &map.line;

  for (int i=0; i<map.numLines; i++)
    {
#ifdef FLIPXY
      imageRaw.Line ((int)(SCALE * (line[1] - y0) + ym),
		     (int)(SCALE * (line[0] - x0) + xm),
		     (int)(SCALE * (line[3] - y0) + ym),
		     (int)(SCALE * (line[2] - x0) + xm));
#else
      imageRaw.Line ((int)( SCALE * (line[0] - x0) + xm),
		     (int)(-SCALE * (line[1] - y0) + ym),
		     (int)( SCALE * (line[2] - x0) + xm),
		     (int)(-SCALE * (line[3] - y0) + ym));
#endif
      line += 4;
    }

  //
  // draw map (initial marks only)
  //

  imageRaw.SetColor (0, 0, 255);

  CVEC::datatype_t *mark = &map.mark;

  for (int i=0; i<map.numMarks; i++)
    {
#ifdef FLIPXY
      int xi = (int)(SCALE * (mark[1] - y0) + ym);
      int yi = (int)(SCALE * (mark[0] - x0) + xm);
#else
      int xi = (int)(SCALE * (mark[0] - x0) + xm);
      int yi = (int)(-SCALE * (mark[1] - y0) + ym);
#endif

      imageRaw.Box (xi-6, yi-6, xi+6, yi+6);

      mark += 2;
    }

  //
  //
  //

  float xxx[BACKLOGSIZE], yyy[BACKLOGSIZE];
  int iii = 0;

  meter_t x, y;
  degree_t a;

  if (mobile.GetPosition (x, y, a) < 0)
    {
      GTLPRINT (("GetPosition() failed\n"));
      throw -42;
    }

  for (int i=0; i<BACKLOGSIZE; i++)
    {
      xxx[i] = x;
      yyy[i] = y;
    }

  //

  CIMAGE image (TYPE_RGB888, WIDTH, HEIGHT, "mobileView");

#ifdef EXTENDED_LOGGING

  int fileCnt = 1;

#endif

  while (42)
    {
      //
      // get current position
      //

      double varXX, varXY, varYX, varYY, varAA;

      if (mobile.GetPositionAndVariance (x, y, a,
					 varXX, varXY, varYX, varYY,
					 varAA) < 0)
	{
	  GTLPRINT (("GetPosition() failed\n"));
	  throw -42;
	}

#ifdef EXTENDED_LOGGING

      char fileName[64];

      sprintf (fileName, "view-%04i.txt", fileCnt);

      FILE *fp = fopen (fileName, "w");

      if (!fp)
	{
	  GTLFATAL (("failed to open file\n"));
	}
      else
	{
	  struct timeval now;
	  gettimeofday (&now, 0x0);

	  struct tm tm;
	  localtime_r (&now.tv_sec, &tm);

	  fprintf (fp, "# YYYYMMDDHHMMSSUUUUUU\n");
	  fprintf (fp, "%04i%02i%02i%02i%02i%02u%06i\n",
		   tm.tm_year + 1900,
		   tm.tm_mon + 1,
		   tm.tm_mday,
		   tm.tm_hour,
		   tm.tm_min,
		   tm.tm_sec,
		   (int)now.tv_usec);

	  fprintf (fp, "# x-pos y-pos a-pos\n");
	  fprintf (fp, "%f %f %f\n", x, y, a);
	}

#endif

      //
      //
      //

      image = imageRaw;

      //
      // draw current marks
      //

      int numMarks;
      CVEC marks;

//#undef MARKSVARIANCE
#ifndef MARKSVARIANCE
#define MARKSVARIANCE
#endif

#ifdef MARKSVARIANCE
      CVEC variances;
      if ((numMarks = mobile.GetAllMarksWithVariances (marks, variances)) > 0)
#else
      if ((numMarks = mobile.GetAllMarks (marks)) > 0)
#endif
	{
#ifdef EXTENDED_LOGGING
	  if (fp)
	    {
	      fprintf (fp, "# num-marks\n");
	      fprintf (fp, "%i\n", numMarks);

	      if (numMarks > 0)
		{
		  fprintf (fp, "# x-mark y-mark\n");
		}
	    }
#endif

	  image.SetColor (160, 160, 255);

	  CVEC::datatype_t *mark = &marks[0];
#ifdef MARKSVARIANCE
	  CVEC::datatype_t *var = &variances[0];
#endif

	  for (int i=0; i<numMarks; i++)
	    {
#ifdef EXTENDED_LOGGING
	      if (fp)
		{
		  fprintf (fp, "%f %f\n", mark[0], mark[1]);
		}
#endif

#ifdef FLIPXY
	      int xi = (int)(SCALE * (mark[1] - y0) + ym);
	      int yi = (int)(SCALE * (mark[0] - x0) + xm);
#else
	      int xi = (int)(SCALE * (mark[0] - x0) + xm);
	      int yi = (int)(-SCALE * (mark[1] - y0) + ym);
#endif

	      //image.Pbox (xi-3, yi-3, xi+3, yi+3);
	      image.Line (xi-10, yi-10, xi+10, yi+10);
	      image.Line (xi-10, yi+10, xi+10, yi-10);

#ifdef MARKSVARIANCE

	      //
	      //
	      //

	      double angle, ev1, ev2;

	      if (ComputeEllipse (angle, ev1, ev2,
				  var[0], var[1], var[2], var[3]) >= 0)
		{
		  //ev1 = sqrt (ev1) / 10.0;
		  //ev2 = sqrt (ev2) / 10.0;

		  image.Ellipse (xi,
				 yi,
				 angle,
				 (int)(SCALE * ev1 / 20.0),
				 (int)(SCALE * ev2 / 20.0));
		}

	      var += 4;
#endif

	      mark += 2;
	    }
	}

      /////////////////////////////////////////////////////////////////////////
      //
      //
      //
      /////////////////////////////////////////////////////////////////////////

      const int numScanners = mobile.GetNumScanners ();

#ifdef EXTENDED_LOGGING
      if (fp)
	{
	  fprintf (fp, "# num-scanners\n");
	  fprintf (fp, "%i\n", numScanners);
	}
#endif

      for (int idx=0; idx<numScanners; idx++)
	{
	  // position of laser

	  meter_t xLaser, yLaser;
	  degree_t aLaser;

	  mobile.GetScannerPosition (xLaser, yLaser, aLaser, idx);

#ifdef EXTENDED_LOGGING
	  if (fp)
	    {
	      fprintf (fp, "# x-scanner y-scanner a-scanner\n");
	      fprintf (fp, "%f %f %f\n", xLaser, yLaser, aLaser);
	    }
#endif

	  const double c = cos (M_PI * a / 180.0);
	  const double s = sin (M_PI * a / 180.0);

	  const double x0Laser = x + c * xLaser - s * yLaser;
	  const double y0Laser = y + s * xLaser + c * yLaser;

	  //

	  CMOBILE::CSCAN scan;

	  if (mobile.GetScanWorld (scan, idx) >= 0)
	    {
	      // draw scan (distances only)

	      image.SetColor (128, 0, 0);

#ifdef EXTENDED_LOGGING
	      if (fp)
		{
		  fprintf (fp, "# num-scans\n");
		  fprintf (fp, "%i\n", scan.numScans);

		  if (scan.numScans > 0)
		    {
		      fprintf (fp, "# x-scan y-scan\n");
		    }
		}
#endif

	      for (int i=0; i<scan.numScans; i++)
		{
#ifdef EXTENDED_LOGGING
		  if (fp)
		    {
		      fprintf (fp, "%f %f\n", scan.sx[i], scan.sy[i]);
		    }
#endif

#ifdef FLIPXY
		  image.Line ((int)(SCALE * (   y0Laser - y0) + ym),
			      (int)(SCALE * (   x0Laser - x0) + xm),
			      (int)(SCALE * (scan.sy[i] - y0) + ym),
			      (int)(SCALE * (scan.sx[i] - x0) + xm));
#else
		  image.Line ((int)( SCALE * (   x0Laser - x0) + xm),
			      (int)( SCALE * (   y0Laser - y0) + ym),
			      (int)( SCALE * (scan.sx[i] - x0) + xm),
			      (int)(-SCALE * (scan.sy[i] - y0) + ym));
#endif
		}

	      image.SetColor (0, 255, 0);

	      for (int i=0; i<scan.numScans; i++)
		{
#ifdef FLIPXY
		  image.Plot ((int)(SCALE * (scan.sy[i] - y0) + ym),
			      (int)(SCALE * (scan.sx[i] - x0) + xm));
#else
		  image.Plot ((int)( SCALE * (scan.sx[i] - x0) + xm),
			      (int)(-SCALE * (scan.sy[i] - y0) + ym));
#endif
		}

	      // draw scan (marks only)

#ifdef EXTENDED_LOGGING
	      if (fp)
		{
		  fprintf (fp, "# num-marks\n");
		  fprintf (fp, "%i\n", scan.numMarks);

		  if (scan.numMarks > 0)
		    {
		      fprintf (fp, "# x-mark y-mark idx-mark\n");
		    }
		}
#endif

	      for (int i=0; i<scan.numMarks; i++)
		{
#ifdef EXTENDED_LOGGING
		  if (fp)
		    {
		      fprintf (fp,
			       "%f %f %i\n",
			       scan.mx[i], scan.my[i], scan.mIdx[i]);
		    }
#endif
		  if (scan.mIdx[i] < 0)
		    {
		      image.SetColor (255, 255, 0);   // yellow: not matched
		    }
		  else
		    {
		      image.SetColor (0, 255, 0);   // green: matched
		    }
#ifdef FLIPXY
		  int xi = (int)(SCALE * (scan.my[i] - y0) + ym);
		  int yi = (int)(SCALE * (scan.mx[i] - x0) + xm);
#else
		  int xi = (int)( SCALE * (scan.mx[i] - x0) + xm);
		  int yi = (int)(-SCALE * (scan.my[i] - y0) + ym);
#endif
		  image.Pcircle (xi, yi, 3);

#if 0
		  if (scan.mIdx[i] < 0)
		    {
		      image.SetColor (0, 0, 0);   // black: not matched
		    }
		  else
		    {
		      image.SetColor (255, 255, 255);   // white: matched
		    }

		  image.Pcircle (xi, yi, 1);
#endif
		}
	    }
	  else
	    {
	      GTLFATAL (("GetScanWorld() failed\n"));
	    }
	}

      /////////////////////////////////////////////////////////////////////////
      //
      //
      //
      /////////////////////////////////////////////////////////////////////////

      int idx = iii++ % BACKLOGSIZE;

      xxx[idx] = x;
      yyy[idx] = y;

      for (int i=BACKLOGSIZE; i>=0; i--)
	{
	  image.SetColor (255-2*i, 255-2*i, 255-2*i);

	  int j = idx - i;

	  if (j < 0)
	    {
	      j += BACKLOGSIZE;
	    }

#ifdef FLIPXY
	  image.Plot ((int)(SCALE * (yyy[j] - y0) + ym),
		      (int)(SCALE * (xxx[j] - x0) + xm));
#else
	  image.Plot ((int)( SCALE * (xxx[j] - x0) + xm),
		      (int)(-SCALE * (yyy[j] - y0) + ym));
#endif
	}

      //
      // draw robot
      //

      {
	int size = (int)(SCALE * 0.33);   // approximate diameter of platform

#ifdef FLIPXY
	int xi = (int)(SCALE * (y - y0) + ym);
	int yi = (int)(SCALE * (x - x0) + xm);
	int xc = (int)(xi + (2*size/3) * sin (M_PI * (a - a0) / 180.0));
	int yc = (int)(yi + (2*size/3) * cos (M_PI * (a - a0) / 180.0));
#else
	int xi = (int)( SCALE * (x - x0) + xm);
	int yi = (int)(-SCALE * (y - y0) + ym);
	int xc = (int)(xi + (2*size/3) * cos (M_PI * (a - a0) / 180.0));
	int yc = (int)(yi - (2*size/3) * sin (M_PI * (a - a0) / 180.0));
#endif

	image.SetColor (255, 255, 255);

	image.Pcircle (xi, yi, size);

	image.SetColor (0, 0, 0);

	image.Pcircle (xc, yc, size/4);
      }

      //
      // draw robot variance
      //
#if 1

      double angle, ev1, ev2;

      if (ComputeEllipse (angle, ev1, ev2,
			  varXX, varXY, varYX, varYY) >= 0)
	{
	  GTLPRINT (("ev1 = %f, ev2 = %f, angle = %f°\n",
		     ev1, ev2, 180.0 * angle / M_PI));

	  ev1 = sqrt (ev1) / 1000.0;
	  ev2 = sqrt (ev2) / 1000.0;

	  int xi = (int)(SCALE * (y - y0) + ym);
	  int yi = (int)(SCALE * (x - x0) + xm);

	  image.SetColor (128, 64, 0);

	  image.Ellipse (xi,
			 yi,
			 angle,
			 (int)(SCALE * ev1),
			 (int)(SCALE * ev2));
	}

#endif

      //
      //
      //

      image.Show ();

#ifdef EXTENDED_LOGGING

      //sprintf (fileName, "view-%04i.ppm.gz", fileCnt++);
      //image.SaveGzipPPM (fileName, 1);

      sprintf (fileName, "view-%04i.ppm", fileCnt++);
      image.SavePPM (fileName);

      if (fp)
	{
	  fclose (fp);
	}

      usleep (100000);

#else

      usleep (200000);

#endif
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

//
//
//

static void usage (void)
{
  fprintf (stderr, "usage: genView [-d] [<host>]\n");
  fprintf (stderr, "\twhere:\n");
  fprintf (stderr, "\t-d : enable debugging\n");
  fprintf (stderr, "\t-m <hostname> : connect to mobile <hostname> instead of default\n");

  exit (-1);
}


//
//
//

int main (int argc, char **argv)
{
  int c;
  char *host = 0x0;

  while ((c = getopt (argc, argv, "dm:h")) >= 0)
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
	case '?':
	default:
	  usage ();
	}
    }

  //

  GTLC ("genView::main()");

  GTLPRINT (("host = %p\n", (void*)host));

  CMOBILE mobile (host);

  View (mobile);

  return 0;
}
