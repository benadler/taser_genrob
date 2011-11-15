//
// mobileCalibrateEvaluateImagesPrepare.cc
// (C) 11/2003 by TeSche (Torsten Scherer)
// <itschere@techfak.uni-bielefeld.de>
//
// prepares evaluation of images by ensuring that all models are
// recognizable. this is the only part that is interactive and therefore
// tedious. once all the unrecognizable images have been removed the rest is
// straightforward and automatic.
//

#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#include "thread/tracelog.h"
#include "vision/model.h"
#include "tty/kbd.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static char *readline (double &px, double &py, double &pa,
		       double &dx, double &dy, double &da,
		       FILE *fp)
{
  static char buf[256];
  char *ret;

  while ((ret = fgets (buf, 256, fp)))
    {
      if (buf[0] != '#')
	{
	  break;
	}
    }

  if (ret)
    {
      sscanf (buf, "%lf %lf %lf %lf %lf %lf", &px, &py, &pa, &dx, &dy, &da);
    }

  return ret;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int main (int argc, char **argv)
{
#if 0
  _globGtlEnable = 1;
#endif

  if (argc != 2)
    {
      fprintf (stderr, "usage: %s <namepref>\n", argv[0]);
      fprintf (stderr, "\twhere:\n");
      fprintf (stderr, "\t<namepref> is the prefix of the logfile/images (including paths)\n");
      return -1;
    }

  GTLC ("main()");

  //
  //
  //

  const int maxNameLen = strlen (argv[1]) + 16;
  char srcName[maxNameLen];

  snprintf (srcName, maxNameLen, "%s.log", argv[1]);

  FILE *src = fopen (srcName, "r");

  if (!src)
    {
      GTLFATAL (("file \"%s\" not found\n", srcName));
      abort ();
    }

  //

  char dstName[maxNameLen];
  snprintf (dstName, maxNameLen, "%s.log~", argv[1]);

  FILE *dst = fopen (dstName, "w");

  if (!dst)
    {
      GTLFATAL (("file \"%s\" not writeable\n", dstName));
      abort ();
    }

  //
  //
  //

  for (int cnt=0; ; cnt++)
    {
      double px, py, pa, dx, dy, da;

      if (!readline (px, py, pa, dx, dy, da, src))
	{
	  break;
	}

      printf ("%i : %f %f %f\n", cnt, px, py, pa);

      //

      char iName[maxNameLen];
      snprintf (iName, maxNameLen, "%s-%03i.ppm.gz", argv[1], cnt);

      CIMAGE image;

      if (image.LoadPPM (iName) < 0)
	{
	  GTLFATAL (("failed to load image \"%s\"\n", iName));
	  abort ();
	}

      CMODEL model ("model");

      static CIMAGE regionImage;

      model.AcquireAutomatic (image, &regionImage);

      model.MarkAllRegions (image, regionImage);

      regionImage.Show ();

      //

      CKBD kbd;

      unsigned long key = kbd.GetKey ();

      switch (key)
	{
	case 't':
	  fprintf (dst, "# ");
	  // fall-thru

	default:
	  fprintf (dst,
		   "%f %f %f  %f %f %f %s\n",
		   px, py, pa,
		   dx, dy, da,
		   iName);

	  break;
	}
    }

  //
  //
  //

  fclose (dst);
  fclose (src);

  //
  //
  //

  if (rename (dstName, srcName) < 0)
    {
      GTLFATAL (("failed to rename \"%s\" to \"%s\"\n", srcName, dstName));
      return -1;
    }

  return 0;
}
