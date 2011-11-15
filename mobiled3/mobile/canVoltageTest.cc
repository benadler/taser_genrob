//
// canVoltageTest.cc
// (C) by Torsten Scherer (TeSche)
// <itschere@techfak.uni-bielefeld.de>
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "mobile/canPlatform.h"

#ifdef WITH_RCCL

#include <rccl.h>
#include <fts3.h>

#include "tty/kbd.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static void stepLog (LOG *l, const double v)
{
  //

  if (++l->used > l->size)
    {
      l->used = l->size;
    }

  for (int i=0; i<4; i++)
    {
      memmove (l->item[i].data + 1,
	       l->item[i].data,
	       (l->size - 1) * sizeof (float));
    }

  static time_t tStart = 0;

  if (!tStart)
    {
      tStart = time (0);
    }

  l->item[0].data[0] = (time (0) - tStart) / 3600.0;
  l->item[1].data[0] = v;
  l->item[2].data[0] = 54.0;
  l->item[3].data[0] = 44.0;

  //

  static VIEW *view = 0x0;

  if (!view)
    {
      if (!(view = viewCreate ("t=Zeit[h],u,u_max,u_min", 1, VIEW_GRID)))
	{
	  abort ();
	}

      viewSetTitle (view, "Batteriespannung");
      viewSetLabel (view, "Spannung [Volt]");
    }

  if (l->used > 1)
    {
      viewLog (view, l);
    }
}

#endif


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static int medianInsert (int *buf,
			 const int bufSize,
			 const int ommitSize,
			 const int value)
{
  // insert at head

  memmove (buf+1, buf, (bufSize-1) * sizeof (int));

  buf[0] = value;

  // sort into temporary buffer

  int *tmpBuf = (int *)alloca (bufSize * sizeof (int));

  memcpy (tmpBuf, buf, bufSize * sizeof (int));

  for (int i=0; i<bufSize-1; i++)
    {
      int smallestIdx = i;
      int smallestValue = tmpBuf[i];

      for (int j=i+1; j<bufSize; j++)
	{
	  if (tmpBuf[j] < smallestValue)
	    {
	      smallestValue = tmpBuf[j];
	      smallestIdx = j;
	    }
	}

      if (smallestIdx != i)
	{
	  int tmp = tmpBuf[i];
	  tmpBuf[i] = tmpBuf[smallestIdx];
	  tmpBuf[smallestIdx] = tmp;
	}
    }

  // compute median

  int sum = 0, cnt = bufSize - 2 * ommitSize;

  if (cnt > 0)
    {
      for (int i=ommitSize; i<bufSize-ommitSize; i++)
	{
	  sum += tmpBuf[i];
	}

      return sum / cnt;
    }

  // error

  fprintf (stderr, "median parameter error\n");
  abort ();
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int main (void)
{
  CCANPLATFORM platform;

#ifdef WITH_RCCL

  LOG *l = logCreate (10000, 4);

  float dummy;
  logDefineItem (l, 0, "t", &dummy, 0x0);
  logDefineItem (l, 1, "u", &dummy, 0x0);
  logDefineItem (l, 2, "u_max", &dummy, 0x0);
  logDefineItem (l, 3, "u_min", &dummy, 0x0);

  CKBD kbd;

#endif

  int run = 0;

  //

#define BUFSIZE 15

  int cntBuf[BUFSIZE];

  const int cntBufInit = platform.GetBatteryVoltageRaw ();

  for (int i=0; i<BUFSIZE; i++)
    {
      cntBuf[i] = cntBufInit;
    }

  //

  while (42)
    {
#ifdef WITH_RCCL

      bool save = !(++run % (60*15));

      switch (kbd.GetKey (1000))
	{
	case 's':
	  {
	    save = true;
	  }
	  break;

	case 0:
	  {
	    const int raw = platform.GetBatteryVoltageRaw ();

#define OMMITSIZE 4

	    const int avg = medianInsert (cntBuf, BUFSIZE, OMMITSIZE, raw);

	    if (!(run % BUFSIZE))
	      {
		const double voltage = platform.ConvertBatteryVoltage (avg);

		printf ("%i -> %f volt\r\n", avg, voltage);
		fflush (stdout);

		stepLog (l, voltage);
	      }
	  }
	  break;

	default:
	  break;
	}

      if (save)
	{
	  time_t t = time (0);
	  struct tm tm;
	  localtime_r (&t, &tm);
	  char buf[64];
	  sprintf (buf,
		   "voltage-%04i%02i%02i%02i%02i.log",
		   tm.tm_year + 1900,
		   tm.tm_mon + 1,
		   tm.tm_mday,
		   tm.tm_hour,
		   tm.tm_min);
	  logSave (l, buf);
	  printf ("\r%s saved\r\n", buf);
	}

#else

      fprintf (stderr, "non-rccl (fts) mode not supported\n");
      abort ();

#endif

    }

  return 0;
}
