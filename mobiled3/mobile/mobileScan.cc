//
// mobileScan.cc
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

static void parse (const char *posStr, float *pos)
{
  GTLC ("parse()");

  if (sscanf (posStr, "%f,%f,%f", pos, pos+1, pos+2) != 3)
    {
      // no coordinate triplet given, treat as device name

      CINFOBASE infobase;

      const int len = sizeof ("/")
	+ strlen (posStr)
	+ sizeof ("/position")
	+ 1;
      char *entryName = (char *)alloca (len);

      sprintf (entryName, "/%s/position", posStr);

      if (infobase.ReadFloat (entryName, pos, 3) != 3)
	{
	  GTLFATAL (("failed to read position\n"));
	  exit (-1);
	}
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static void usage (void)
{
  fprintf (stderr, "usage: mobilePlace [-d] [-h] [-m <hostname>] <idx>\n");
  fprintf (stderr, "\twhere\n");
  fprintf (stderr, "\t-d : enable debugging\n");
  fprintf (stderr, "\t-m <hostname> : connect to mobile <hostname>\n");
  fprintf (stderr, "\t-h : print this help\n");
  fprintf (stderr, "\t<idx> : the number of the scanner 0..NUM\n");

  exit (-1);
}


int main (int argc, char **argv)
{
  GTLC ("mobileScan::main()");

  int c;
  char *host = 0x0;

  while ((c = getopt (argc, argv, "hdm:")) >= 0)
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

  if (argc - optind != 1)
    {
      usage ();
    }

  int idx = atoi (argv[optind]);

  //

  CMOBILE mobile (host);

  CMOBILE::CSCAN scan;

  mobile.GetScanPlatform (scan, idx);

  printf ("# %i scans\n", scan.numScans);

  for (int i=0; i<scan.numScans; i++)
    {
      printf ("%.3f %.3f\n", scan.sx[i], scan.sy[i]);
    }

  printf ("# %i marks\n", scan.numMarks);

  for (int i=0; i<scan.numMarks; i++)
    {
      printf ("%.3f %.3f\n", scan.mx[i], scan.my[i]);
    }

  return 0;
}
