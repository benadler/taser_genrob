//
//
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#define MM2XFIG 45

#define FLIP


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

typedef struct
{
  int x0, y0, xe, ye;
} LINE;


static void WriteLines (LINE *line, int numLines)
{
  printf ("2 1 0 1 0 7 50 0 -1 0.000 0 0 -1 0 0 %i\n", numLines + 1);

  printf ("\t%i %i\n", line[0].x0 * MM2XFIG, line[0].y0 * MM2XFIG);

  for (int i=0; i<numLines; i++)
    {
      printf ("\t%i %i\n", line[i].xe * MM2XFIG, line[i].ye * MM2XFIG);
    }
}


static void processMapFile (const char * const fName)
{
  FILE *fp = fopen (fName, "r");

  if (!fp)
    {
      perror ("fopen()");
      return;
    }

  int numLines = 0;
  int maxLines = 0;
  LINE *line = 0x0;

  char buf[256];

  while (fgets (buf, 256, fp))
    {
      // ignore comments

      if (buf[0] == '#')
	{
	  continue;
	}

      // empty lines separate polygones

      if ((buf[0] == '\n') ||
	  (buf[0] == '\r'))
	{
	  if (numLines > 0)
	    {
	      WriteLines (line, numLines);
	      numLines = 0;
	    }

	  continue;
	}

      // got a new line

      float x0f, y0f, xef, yef;
      sscanf (buf, "%f %f %f %f", &x0f, &y0f, &xef, &yef);
      int x0 = (int)floor (x0f * 1000.0);
      int xe = (int)floor (xef * 1000.0);
      int y0 = (int)floor (y0f * 1000.0);
      int ye = (int)floor (yef * 1000.0);

      int idx = numLines++;

      if (numLines > maxLines)
	{
	  maxLines += 16;

	  if (!(line = (LINE *)realloc (line, maxLines * sizeof (LINE))))
	    {
	      perror ("realloc()");
	      abort ();
	    }
	}

#ifdef FLIP
      line[idx].x0 = y0;
      line[idx].y0 = x0;
      line[idx].xe = ye;
      line[idx].ye = xe;
#else
      line[idx].x0 = x0;
      line[idx].y0 = y0;
      line[idx].xe = xe;
      line[idx].ye = ye;
#endif
    }

  // write the last polygone if necessary

  if (numLines > 0)
    {
      WriteLines (line, numLines);
      numLines = 0;
    }

  free (line);

  //

  fclose (fp);
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static void processMarkFile (const char * const fName)
{
  FILE *fp = fopen (fName, "r");

  if (!fp)
    {
      perror ("fopen()");
      return;
    }

  char buf[256];

  while (fgets (buf, 256, fp))
    {
      // ignore comments and empty lines

      if ((buf[0] == '#') ||
	  (buf[0] == '\n') ||
	  (buf[0] == '\r'))
	{
	  continue;
	}

      // got a new point

      float x0f, y0f;
      sscanf (buf, "%f %f", &x0f, &y0f);
#ifdef FLIP
      int y0 = MM2XFIG * (int)floor (x0f);
      int x0 = MM2XFIG * (int)floor (y0f);
#else
      int x0 = MM2XFIG * (int)floor (x0f);
      int y0 = MM2XFIG * (int)floor (y0f);
#endif

      // 1 3 0 1 2 2 DEPTH 0 20 0.000 1 0.0000 X0 Y0 XR YR X0 Y0 XE YE
      //
      // for circles XR=YE and either XE=X0+XR and YE=Y0, or XE=X0 and YE=Y0+YR

      const int radius = MM2XFIG * 25;

      printf ("1 3 0 1 2 2 50 0 20 0.000 1 0.0000 %i %i %i %i %i %i %i %i\n",
	      x0, y0,
	      radius, radius,
	      x0, y0,
	      x0 + radius, y0);
    }

  //

  fclose (fp);
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int main (void)
{
  printf ("#FIG 3.2\n");
  printf ("Landscape\n");
  printf ("Center\n");
  printf ("Metric\n");
  printf ("A4\n");
  printf ("100.0\n");
  printf ("Single\n");
  printf ("-2\n");
  printf ("1200 2\n");

  processMapFile ("/vol/biorob/share/map.map");

  processMarkFile ("/vol/biorob/share/map.marks");

  return 0;
}
