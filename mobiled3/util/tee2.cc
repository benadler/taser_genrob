//
// tee2.cc
// (C) 2003 by Torsten Scherer (TeSche)
// <itschere@techfak.uni-bielefeld.de>
//
// inspired by the standard `tee', but (by default) only prints the last <n>
// lines of its input. can print everything if given the `-v' argument. can
// print the last <n> lines to a file if given the `-f' argument.
//
// used for debugging: by only printing the last <n> lines you will still see
// all the information that lead to a crash or abort, but you won't have too
// much stuff scrolling through your terminal until that point. since scrolling
// in a xterm/kvt can take quite some cpu cycles or bandwith if logged in via a
// slow wlan it helps saving resources.
//

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


///////////////////////////////////////////////////////////////////////////////
//
// automatically buffer allocating and resizing fgets()... :-)
//
///////////////////////////////////////////////////////////////////////////////

static char *fgets2 (char **bufPtr, int *bufSizePtr, FILE *fp)
{
  char *buf = *bufPtr;
  int bufSize = *bufSizePtr;
  int bufUsed = 0;

  //

#define BUFSIZE 256

  char chunk[BUFSIZE], *ret;

  //fprintf (stderr, "#############\n");

  do
    {
      //fprintf (stderr, "used %i/%i\n", bufUsed, bufSize);

      if (!(ret = fgets (chunk, BUFSIZE, fp)))
	{
	  return 0x0;
	}

      int chunkSize = strlen (chunk);
      int totalSize = bufUsed + chunkSize + 1;   // include trailing \000

      //fprintf (stderr, "chunk %i\n", chunkSize);
      //fprintf (stderr, "total %i (incl \\000)\n", totalSize);

      if (totalSize > bufSize)
	{
	  bufSize = totalSize;

	  if (!(buf = (char *)realloc (buf, bufSize)))
	    {
	      perror ("malloc()");
	      abort ();
	    }
	}

      memcpy (buf + bufUsed, chunk, chunkSize + 1);

      bufUsed += chunkSize;   // excluding trailing \000
    }
  while (buf[bufUsed-1] != '\n');

  //

  //fprintf (stderr, "> \"%s\"\n", buf);

  *bufPtr = buf;
  *bufSizePtr = bufSize;

  return buf;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static void tee (const int numLines,
		 const bool verbose,
		 const char *fName)
{
  //

  char **line;
  int *size;

  if (!(line = (char **)malloc (numLines * sizeof (char *))))
    {
      perror ("malloc()");
      abort ();
    }

  memset (line, 0, numLines * sizeof (char *));

  if (!(size = (int *)malloc (numLines * sizeof (int))))
    {
      perror ("malloc()");
      abort ();
    }

  memset (size, 0, numLines * sizeof (int));

  //

  int idx = 0;

  while (fgets2 (&line[idx], &size[idx], stdin))
    {
      if (verbose)
	{
	  printf ("%s", line[idx]);
	}

      idx = ++idx % numLines;
    }

  //

  FILE *fp = fName ? fopen (fName, "w") : 0x0;

  for (int i=0; i<numLines; i++)
    {
      if (line[idx])
	{
	  printf ("%s", line[idx]);

	  if (fp)
	    {
	      fprintf (fp, "%s", line[idx]);
	    }
	}

      idx = ++idx % numLines;
    }

  if (fp)
    {
      fclose (fp);
    }

  // don't clean up memory, just exit
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static void usage (const char * const pName)
{
  fprintf (stderr, "usage: %s [-h] [-v] [-f <filename>] <numlines>\n", pName);
  fprintf (stderr, "\twhere:\n");
  fprintf (stderr, "\t-h : print this help\n");
  fprintf (stderr, "\t-v : be verbose, always print everything, not just last <numlines> lines\n");
  fprintf (stderr, "\t-f <filename> : copy output to file <filename>\n");

  exit (-1);
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int main (int argc, char **argv)
{
  int c;
  char *fName = 0x0;
  bool verbose = false;

  while ((c = getopt (argc, argv, "vf:")) >= 0)
    {
      switch (c)
	{
	case 'f':
	  fName = optarg;
	  break;

	case 'v':
	  verbose = true;
	  break;

	case '?':
	default:
	  usage (argv[0]);
	  break;
	}
    }

  if (optind != argc - 1)
    {
      usage (argv[0]);
    }

  const int numLines = atoi (argv[optind]);

  if (numLines < 1)
    {
      usage (argv[0]);
    }

  // catch SIGINT, so that you can kill the program at the head of the pipe
  // without killing me.

  signal (SIGINT, SIG_IGN);

  //

  tee (numLines, verbose, fName);

  //

  return 0;
}
