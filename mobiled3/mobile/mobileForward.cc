//
// mobileForward.cc
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

static void usage (void)
{
  fprintf (stderr, "usage: mobileForward [-d] [-m <hostname>] [-h] [-l] {<d>, ...}\n");
  fprintf (stderr, "\twhere\n");
  fprintf (stderr, "\t-d : enable debugging\n");
  fprintf (stderr, "\t-m <hostname> : connect to mobile <hostname>\n");
  fprintf (stderr, "\t-h : print this help\n");
  fprintf (stderr, "\t-l : loop\n");
  fprintf (stderr, "\t<d> : a (positive?) distance (in meter)\n");

  exit (-1);
}


int main (int argc, char **argv)
{
  GTLC ("mobileForward::main()");

  int c;
  char *host = 0x0;
  bool loop = false;

  while ((c = getopt (argc, argv, "hldm:")) >= 0)
    {
      switch (c)
	{
	case 'd':
	  _globGtlEnable = 1;
	  break;

	case 'm':
	  host = optarg;
	  break;

	case 'l':
	  loop = true;
	  break;

	case 'h':
	default:
	  usage ();
	}
    }

  if (argc - optind < 1)
    {
      usage ();
    }

  //

  CMOBILE mobile (host);

  do
    {
      for (int i=optind; i<argc; i++)
	{
	  double d;

	  sscanf (argv[i], "%lf ", &d);

	  printf ("# %f\n", d);

	  mobile.Forward (d);

	  mobile.WaitForCompleted ();

	  double x, y, a;

	  mobile.GetPosition (x, y, a);

	  printf ("%f %f %f\n", x, y, a);
	  fflush (stdout);
	  fflush (stderr);
	}
    }
  while (loop);

  //

  return 0;
}
