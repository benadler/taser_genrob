//
// mobileTeach.cc
// (C) 2001,2002 by TeSche (Torsten Scherer)
// <itschere@techfak.uni-bielefeld.de>
//

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
  fprintf (stderr, "usage: mobileTeach: [-d] [-m <hostname>] [-s <devicename> [-c] [-h]\n");
  fprintf (stderr, "\twhere:\n");
  fprintf (stderr, "\t-d : enable debugging\n");
  fprintf (stderr, "\t-m <hostname>: connect to <hostname> instead of default\n");
  fprintf (stderr, "\t-s <devicename> : store under name <devicename>\n");
  fprintf (stderr, "\t-c : create device if it does not exist\n");
  fprintf (stderr, "\t-h : print this help\n");

  exit (-1);
}


int main (int argc, char **argv)
{
  GTLC ("mobileTeach::main()");

  bool flagStore = false, flagCreate = false;
  int c;
  char *devName = 0x0;
  char *host = 0x0;

  while ((c = getopt (argc, argv, "cds:h:")) >= 0)
    {
      switch (c)
	{
	case 'c':
	  flagCreate = true;
	  break;

	case 'd':
	  _globGtlEnable = 1;
	  break;

	case 'h':
	  host = optarg;
	  break;

	case 's':
	  flagStore = true;
	  devName = optarg;
	  break;

	default:
	  usage ();
	}
    }

  if (argc != optind)
    {
      usage ();
    }

  //

  CMOBILE mobile (host);

  double pos[3];

  for (int i=0; i<20; i++)
    {
      mobile.GetPosition (pos[0], pos[1], pos[2]);
    }

  printf ("\n/%s/position: %.3f %.3f %.2f\n\n",
	  devName,
	  pos[0],
	  pos[1],
	  pos[2]);

  //

  if (flagStore)
    {
      CINFOBASE infobase;

      if (infobase.Chdir (devName) < 0)
	{
	  if (!flagCreate)
	    {
	      GTLFATAL (("directory does not exist, maybe use `-c'?\n"));
	      return 0;
	    }

	  if (infobase.Mkdir (devName) < 0)
	    {
	      GTLFATAL (("failed to create infobase directory\n"));
	      return -1;
	    }

	  if (infobase.Chdir (devName) < 0)
	    {
	      GTLFATAL (("failed to set infobase directory\n"));
	      return -1;
	    }
	}

      //

      int isDir;

      if (infobase.Stat ("position", &isDir) < 0)
	{
	  if (!flagCreate)
	    {
	      GTLFATAL (("entry does not exist, maybe use `-c'?\n"));
	      return 0;
	    }

	  if (infobase.Creat ("position") < 0)
	    {
	      GTLFATAL (("failed to create infobase entry\n"));
	      return -1;
	    }

	  isDir = 0;
	}

      if (isDir)
	{
	  GTLFATAL (("entry already exists as a directory\n"));
	  return -1;
	}

      //

      char entryName[128];

      sprintf (entryName, "/%s/position", devName);

      if (infobase.WriteDouble (entryName, pos, 3) != 3)
	{
	  GTLFATAL (("failed to write entry\n"));
	  return -1;
	}
    }
  else
    {
      printf ("position data NOT stored in infobase!\n\n");
    }

  return 0;
}
