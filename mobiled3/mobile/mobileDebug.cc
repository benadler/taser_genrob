//
// mobileDebug.cc
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
  fprintf (stderr, "usage: mobileDebug [-d] [-h] [-m <host>] <flag>\n");
  fprintf (stderr, "\twhere\n");
  fprintf (stderr, "\t-d        : enable debugging\n");
  fprintf (stderr, "\t-h        : print this help\n");
  fprintf (stderr, "\t-m <host> : connect to mobile <hostname>\n");
  fprintf (stderr, "\t<flag>    : a boolean flag\n");
  exit (-1);
}


int main (int argc, char **argv)
{
  GTLC ("mobileDebug::main()");

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

  const int flag = atoi (argv[optind]);

  //

  CMOBILE mobile (host);

  return mobile.Debugging (flag);
}
