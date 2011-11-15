//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/args.h"
#include "image/image.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static int convert (const char *fName)
{
  FILE *fp;

  if (!(fp = fopen (fName, "r")))
    {
      fprintf (stderr, "file \"%s\" not found\n", fName);
      return -1;
    }

#if 1
  CIMAGE image (TYPE_RGB888, 1024, 768);
  image.SetColor (255, 255, 255);
  float xm = image.GetWidth () / 2.0;
  float ym = image.GetHeight () / 2.0;
#endif

  char buf[256];

  while (fgets (buf, 256, fp))
    {
      if (!strlen (buf) >= 255)
	{
	  fprintf (stderr, "warning: line too long\n");
	}

      //printf (">>> %s", buf);

      //
      // open lines
      // 2 1 0 1 <color> 7 <depth> 0 -1 0.000 0 0 -1 0 0 <numpoints>
      //
      // closed lines
      // 2 3 0 1 <color> 7 <depth> 0 -1 0.000 0 0 -1 0 0 <numpoints>
      //

      if (!strncmp (buf, "2 1 0 1 ", 8))   // open lines
	{
	  CARGS args (buf);

	  if (args.GetNumArgs () != 16)
	    {
	      fprintf (stderr, "error: weird parameters\n");
	      abort ();
	    }

	  int depth = atoi (args[6]);
	  int numPoints = atoi (args[15]);

	  //printf ("\tline of %i pixels @ depth %i\n", numPoints, depth);

	  if (numPoints < 2)
	    {
	      fprintf (stderr, "warning: too few points\n");
	      continue;
	    }

#ifdef __GNUC__
#warning "TUNEABLE PARAMETER"
#endif

	  if (depth != 50)
	    {
	      fprintf (stderr, "warning: wrong depth\n");
	      continue;
	    }

	  //

	  int *coords = (int *)malloc (2 * numPoints * sizeof (int));
	  int *coordPtr = coords;

	  int todo = numPoints;

	  while (todo > 0)
	    {
	      char buf2[256];

	      fgets (buf2, 256, fp);

	      //printf ("--- %s", buf2);

	      CARGS args2 (buf2);

	      int points = args2.GetNumArgs ();

	      //printf ("\t\t%i points in this line\n", points >> 1);

	      if (points & 1)
		{
		  fprintf (stderr, "error: odd number of coordinates\n");
		  abort ();
		}

	      for (int i=0; i<points; i++)
		{
		  *coordPtr++ = atoi (args2[i]);
		}

	      todo -= points >> 1;
	    }

	  //

	  for (int i=0; i<numPoints-1; i++)
	    {
	      printf ("%.2f %.2f %.2f %.2f\n",
		      coords[(i+0)*2+0] / 45000.0,
		      -coords[(i+0)*2+1] / 45000.0,
		      coords[(i+1)*2+0] / 45000.0,
		      -coords[(i+1)*2+1] / 45000.0);
#if 1
	      image.Line ((int)(coords[(i+0)*2+0] / 700.0 + xm),
			  (int)(coords[(i+0)*2+1] / 700.0 + ym),
			  (int)(coords[(i+1)*2+0] / 700.0 + xm),
			  (int)(coords[(i+1)*2+1] / 700.0 + ym));
#endif
	    }

	  //

	  free (coords);
	}
      else if (!strncmp (buf, "2 3 0 1 ", 8))   // closed lines
	{
	  printf ("%s", buf);
	}
      else
	{
	}
    }

  fclose (fp);

#if 1
  image.Show ();
  getchar ();
#endif

  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int main (int argc, char **argv)
{
  if (argc != 2)
    {
      fprintf (stderr, "usage: %s <filename>\n", *argv);
      return -1;
    }

  return convert (argv[1]);
}
