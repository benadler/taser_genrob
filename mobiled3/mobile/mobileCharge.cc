//
// mobileCharge.cc
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
				      xErr, yErr, aErr, 0.5) < 0)
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

  sleep (1);
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static void charge (CMOBILE &mobile, int park)
{
  mobile.SetScale (1.0, 1.0);

  if (park)
    {
      move (mobile, "power");   // 0.42cm away from the charger

      mobile.SetScale (0.5, -1.0);
      mobile.Forward (0.3);
      mobile.WaitForCompleted ();
      sleep (1);

      mobile.SetScale (0.25, -1.0);
      mobile.Forward (0.10);
      mobile.WaitForCompleted ();
    }
  else
    {
      mobile.SetScale (0.5, -1.0);
      mobile.Forward (-0.4);
      mobile.WaitForCompleted ();
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static void usage (void)
{
  fprintf (stderr, "usage: mobileCharge [-d] [-m <hostname>] [-a] [-h] [-p | -u]\n");
  fprintf (stderr, "\twhere\n");
  fprintf (stderr, "\t-d : enable debugging\n");
  fprintf (stderr, "\t-m <hostname> : connect to mobile <hostname>\n");
  fprintf (stderr, "\t-h : print this help\n");
  fprintf (stderr, "\t-a : enable approaching (half meter distance backwards)\n");
  fprintf (stderr, "\t-p : park at charger\n");
  fprintf (stderr, "\t-u : unpark at charger\n");

  exit (-1);
}


int main (int argc, char **argv)
{
  GTLC ("MobileMove::main()");

  int c;
  char *host = 0x0;

  int park, defined = false;

  while ((c = getopt (argc, argv, "adm:pu")) >= 0)
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

	case 'p':
	  defined = true;
	  park = true;
	  break;

	case 'u':
	  defined = true;
	  park = false;
	  break;

	case 'h':
	default:
	  usage ();
	}
    }

  if (!defined)
    {
      usage ();
    }

  //

  CMOBILE mobile (host);

  charge (mobile, park);

  return 0;
}
