//
// mobileStop.cc
//

#include <stdio.h>
#include <unistd.h>

#include "mobile/mobile.h"

#include "thread/tracelog.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static void usage (void)
{
  fprintf (stderr, "usage: mobileStop [-d] [-m <hostname>]\n");
  fprintf (stderr, "\twhere\n");
  fprintf (stderr, "\t-d : enable debugging\n");
  fprintf (stderr, "\t-m <hostname> : connect to mobile <hostname>\n");

  exit (-1);
}


int main (int argc, char **argv)
{
  GTLC ("MobileStop::main()");

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

  //

  CMOBILE mobile (host);

  mobile.Stop ();

  return 0;
}
