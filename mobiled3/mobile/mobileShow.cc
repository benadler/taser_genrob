//
// mobileShow.cc
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
  fprintf (stderr, "usage: mobileShow [-d] [-m <hostname>] [-h]\n");
  fprintf (stderr, "\twhere\n");
  fprintf (stderr, "\t-d : enable debugging\n");
  fprintf (stderr, "\t-m <hostname> : connect to mobile <hostname>\n");
  fprintf (stderr, "\t-h : print this help\n");

  exit (-1);
}


int main (int argc, char **argv)
{
  GTLC ("mobileShow::main()");

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

  if (argc - optind != 0)
    {
      usage ();
    }

  //

  CMOBILE mobile (host);

  meter_t x, y;
  degree_t a;

  mobile.GetPosition (x, y, a);

  printf ("%f %f %f\n", x, y, a);

  return 0;
}
