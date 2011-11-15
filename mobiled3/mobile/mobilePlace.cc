//
// mobilePlace.cc
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
  fprintf (stderr, "usage: mobilePlace [-d] [-m <hostname>] [-h] {<devname> | x,y,a}\n");
  fprintf (stderr, "\twhere\n");
  fprintf (stderr, "\t-d : enable debugging\n");
  fprintf (stderr, "\t-m <hostname> : connect to mobile <hostname>\n");
  fprintf (stderr, "\t-h : print this help\n");

  exit (-1);
}


int main (int argc, char **argv)
{
  GTLC ("mobilePlace::main()");

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

  float pos[3];

  parse (argv[optind], pos);

  printf ("%f %f %f\n", pos[0], pos[1], pos[2]);

  //

  CMOBILE mobile (host);

  mobile.SetPosition (pos[0], pos[1], pos[2]);

  sleep (1);

  double x, y, a;

  mobile.GetPosition (x, y, a);

  printf ("%f %f %f (really)\n", x, y, a);

  return 0;
}
