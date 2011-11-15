//
// mobileScan.cc
// (C) 02/2002 by TeSche (Torsten Scherer)
// <itschere@techfak.uni-bielefeld.de>
//

#include <time.h>
#include <stdio.h>

#include "mobile/genbase.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int main (void)
{
#if 1
  CGENBASE genbase ("localhost");
#else
  CGENBASE genbase ("bunsen.techfak.uni-bielefeld.de");
#endif

  double x, y, a;

  genbase.GetFrame (x, y, a);

  CGENBASE::CSCAN scan1, scan2;

  genbase.Scan (scan1, CGENBASE::FRONT);
  genbase.Scan (scan2, CGENBASE::REAR);

  //

  const time_t t = time (0);
  struct tm *tm = localtime (&t);

  char fName[32];

  sprintf (fName,
	   "%04i%02i%02i%02i%02i%02i.scan",
	   tm->tm_year + 1900,
	   tm->tm_mon + 1,
	   tm->tm_mday,
	   tm->tm_hour,
	   tm->tm_min,
	   tm->tm_sec);

  FILE *fp = fopen (fName, "w");

  if (!fp)
    {
      perror ("fopen()");
      return -1;
    }

  fprintf (fp, "# position:\n");
  fprintf (fp, "\n");
  fprintf (fp, "%.3f %.3f %.1f\n", x, y, a);
  fprintf (fp, "\n");

  fprintf (fp, "# front scan:\n");
  fprintf (fp, "\n");
  for (int i=0; i<scan1.numScans; i++)
    {
      fprintf (fp, "%.3f %.3f\n", scan1.sx[i], scan1.sy[i]);
    }
  fprintf (fp, "\n");

  fprintf (fp, "# rear scan:\n");
  fprintf (fp, "\n");
  for (int i=0; i<scan2.numScans; i++)
    {
      fprintf (fp, "%.3f %.3f\n", scan2.sx[i], scan2.sy[i]);
    }

  fclose (fp);

  printf ("%s saved\n", fName);

  return 0;
}
