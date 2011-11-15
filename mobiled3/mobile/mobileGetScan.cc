//
// mobileGetScan.cc
// (C) 12/2003 by TeSche (Torsten Scherer)
// <itschere@techfak.uni-bielefeld.de>
//

#include <math.h>
#include <stdio.h>
#include <unistd.h>

#include "mobile/mobile.h"
#include "thread/tracelog.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static void GetScan (CMOBILE &mobile)
{
  double x0, y0, a0;

  mobile.GetPosition (x0, y0, a0);

  const int numScanners = mobile.GetNumScanners ();

  system ("rm -f scan-rays.out scan-border.out scan-marks.out");

  for (int idx=0; idx<numScanners; idx++)
    {
      // position of laser

      meter_t xLaser, yLaser;
      degree_t aLaser;

      mobile.GetScannerPosition (xLaser, yLaser, aLaser, idx);

      const double c = cos (M_PI * a0 / 180.0);
      const double s = sin (M_PI * a0 / 180.0);

      const double x0Laser = x0 + c * xLaser - s * yLaser;
      const double y0Laser = y0 + s * xLaser + c * yLaser;

      //

      CMOBILE::CSCAN scan;

      mobile.GetScanWorld (scan, idx);

      FILE *fp;

      if ((fp = fopen ("scan-rays.out", "a")))
	{
	  for (int i=0; i<scan.numScans; i++)
	    {
	      fprintf (fp,
		       "%f %f\n",
		       x0Laser, y0Laser);
	      fprintf (fp,
		       "%f %f\n",
		       scan.sx[i], scan.sy[i]);
	      fprintf (fp, "\n");
	    }
	  fprintf (fp, "\n");
	  fclose (fp);
	}

      if ((fp = fopen ("scan-border.out", "a")))
	{
	  fprintf (fp, "%f %f\n", x0Laser, y0Laser);
	  for (int i=0; i<scan.numScans; i++)
	    {
	      fprintf (fp,
		       "%f %f\n",
		       scan.sx[i], scan.sy[i]);
	    }
	  fprintf (fp, "%f %f\n", x0Laser, y0Laser);
	  fprintf (fp, "\n");
	  fclose (fp);
	}

      if ((fp = fopen ("scan-marks.out", "a")))
	{
	  for (int i=0; i<scan.numMarks; i++)
	    {
	      fprintf (fp,
		       "%f %f %i\n",
		       scan.mx[i], scan.my[i], scan.mIdx[i]);
	    }
	  fprintf (fp, "\n");
	  fclose (fp);
	}
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static void usage (const char * const prgName)
{
  fprintf (stderr, "usage: %s [-d] [<host>]\n", prgName);
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
	  usage (argv[0]);
	}
    }

  //

  GTLC ("genView::main()");

  GTLPRINT (("host = %s\n", host));

  CMOBILE mobile (host);

  GetScan (mobile);

  return 0;
}
