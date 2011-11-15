//  main.cc
//  (c) 06/20006, Torsten Scherer, Daniel Westhoff
//  westhoff@informatik.uni-hamburg.de


// Die folgenden drei Zeilen einkommentieren, wenn Debugging-
// Informationen ausgegeben werden sollen.
//#ifdef DEBUG
//#undef DEBUG
//#endif

#include <getopt.h>   // GNU
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "sock/streamsock.h"
#include "thread/tracelog.h"
#include "./genBase.h"
#include "./spawner.h"

///////////////////////////////////////////////////////////////////////////////
/// Listet die Kommandozeilen-Optionen für die Robotersteuerung auf.
///////////////////////////////////////////////////////////////////////////////

static void usage (const char * const prgName)
{
  fprintf (stderr, "usage: %s [<option>] \n", prgName);
  fprintf (stderr, "\twhere <option> is any combination of\n");
  fprintf (stderr, "\t-d : enable debugging\n");
  fprintf (stderr, "\t-h : print this help\n");

  exit (-1);
}

///////////////////////////////////////////////////////////////////////////////
/// Startet die Robotersteuerung <i>mobiled</i>.
///////////////////////////////////////////////////////////////////////////////

int main (int argc, char **argv)
{
  GTLC ("main()");

  // Kommandozeilen-Parameter überprüfen

  int c;

  int longOpt = 4;

  const char * const prgName = *argv;

  while ((c = getopt_long (argc, argv, "hds", 0x0, &longOpt)) != -1)
    {
      switch (c)
	{
	case 'd':
	  _globGtlEnable = 1;
	  break;
	case 'h':
	case '?':
	default:
	  usage (prgName);
	}
    }

  // Programminfos ausgeben

  printf ("\n");
  printf ("*******************************************************************************\n");
  printf (" mobiled (Version 3.0)\n");
  printf (" (C) 2000-2002  T. Scherer\n");
  printf (" (C) 2002-2006  T. Scherer, A. Schneider, D. Westhoff\n");
  printf (" (C) 2006-2006  T. Scherer, A. Schneider, D. Westhoff, S. Poehlsen, H. Bistry\n");
  printf (" http://tams-www.informatik.uni-hamburg.de\n");
  printf ("*******************************************************************************\n");
  printf ("\n");

  // Server-Socket

  GTLPRINT (("Öffne Server-Socket auf Port 9002.\n"));

  CSTREAMSOCK servSock;

  if (servSock.Bind (9002))
    {
      GTLFATAL (("Konnte Server-Port nicht öffen, Port 9002 bereits belegt.\n"));
      abort ();
    }

  servSock.Listen (4);

  // Robotersteuerung

  GTLPRINT (("Starte Robotersteuerung.\n"));

  CGENBASE *genBase = 0x0;

  try
    {
      genBase = new CGENBASE ();
    }

  catch (...)
    {
      GTLFATAL (("failed to instantiate CGENBASE\n"));
      abort ();
    }

  //

  CSPAWNER spawner (&servSock, genBase);

  CDAEMON daemon;

  daemon.AddConnection (&spawner);

  //

  signal (SIGPIPE, SIG_IGN);

  //

  fprintf (stderr, "\n# mobiled: up and running\n\n");
  fflush (stderr);

  while (42)
    {
      daemon.Run (1000);
    }

  //

  delete genBase;

  return 0;
}
