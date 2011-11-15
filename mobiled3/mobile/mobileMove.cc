//
//
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

static bool globFlagApproach = false;


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

static void move (CMOBILE &mobile, const char *posStr)
{
  GTLC ("move()");

  float pos[3];

  parse (posStr, pos);

  printf ("going to: %f, %f, %f\n", pos[0], pos[1], pos[2]);

  double xErr, yErr, aErr;

  if (globFlagApproach)
    {
      if (mobile.MoveViaIntermediate (pos[0], pos[1], pos[2],
				      xErr, yErr, aErr,
#if 0
#warning "reduced approach distance!"
				      0.3) < 0)
#else
	2.0) < 0)
#endif
	{
	  printf ("failed\n");
	  exit (-1);
	}
    }
  else
    {
      if (mobile.MoveDirect (pos[0], pos[1], pos[2],
			     xErr, yErr, aErr) < 0)
	{
	  printf ("failed\n");
	  exit (-1);
	}
    }

  printf ("error: %f, %f, %f\n", xErr, yErr, aErr);
  write (fileno (stdout), "\007", 1);

  sleep (1);
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static void usage (void)
{
  fprintf (stderr, "usage: mobileMove [-d] [-m <hostname>] [-a] [-h] {<devname> | x,y,a}\n");
  fprintf (stderr, "\twhere\n");
  fprintf (stderr, "\t-d : enable debugging\n");
  fprintf (stderr, "\t-m <hostname> : connect to mobile <hostname>\n");
  fprintf (stderr, "\t-h : print this help\n");
  fprintf (stderr, "\t-a : enable approaching (half meter distance backwards)\n");

  exit (-1);
}


int main (int argc, char **argv)
{
  GTLC ("MobileMove::main()");

  int c;
  char *host = 0x0;

  while ((c = getopt (argc, argv, "adm:")) >= 0)
    {
      switch (c)
	{
	case 'a':
	  globFlagApproach = true;
	  break;

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

  for (int i=optind; i<argc; i++)
    {
      move (mobile, argv[i]);
    }

  return 0;
}
