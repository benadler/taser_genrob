//
//
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

typedef struct point
{
  struct point *next;
  double x, y;
  double cnt, xSum, ySum;
} POINT;


//static int numPoints = 0;
static POINT *pointHead = 0x0, *pointTail = 0x0;


typedef struct line
{
  struct line *next;
  POINT *p0, *pe;
} LINE;


static int numLines = 0;
static LINE *lineHead = 0x0, *lineTail = 0x0;


static const double sqr (const double arg)
{
  return arg * arg;
}


static void AddLine (double x0, double y0, double xe, double ye)
{
  // deal with start point

  double distMin = 0.0;
  POINT *pDistMin = 0x0;
  POINT *p = pointHead;

  while (p)
    {
      double dist = sqrt (sqr (p->x - x0) + sqr (p->y - y0));

      if (!pDistMin || (dist < distMin))
	{
	  pDistMin = p;
	  distMin = dist;
	}

      p = p->next;
    }

  if (!pDistMin || (distMin > 0.001))   // 1mm
    {
      pDistMin = (POINT *)malloc (sizeof (POINT));

      pDistMin->next = 0x0;
      pDistMin->x = 0.0;
      pDistMin->y = 0.0;
      pDistMin->cnt = 0.0;
      pDistMin->xSum = 0.0;
      pDistMin->xSum = 0.0;

      if (pointTail)
	{
	  pointTail->next = pDistMin;
	  pointTail = pDistMin;
	}
      else
	{
	  pointHead = pDistMin;
	  pointTail = pDistMin;
	}
    }

  pDistMin->cnt += 1.0;
  pDistMin->xSum += x0;
  pDistMin->ySum += y0;
  pDistMin->x = pDistMin->xSum / pDistMin->cnt;
  pDistMin->y = pDistMin->ySum / pDistMin->cnt;

  POINT *p0 = pDistMin;

  // deal with end point

  distMin = 0.0;
  pDistMin = 0x0;
  p = pointHead;

  while (p)
    {
      double dist = sqrt (sqr (p->x - xe) + sqr (p->y - ye));

      if (!pDistMin || (dist < distMin))
	{
	  pDistMin = p;
	  distMin = dist;
	}

      p = p->next;
    }

  if (!pDistMin || (distMin > 0.001))   // 1mm
    {
      pDistMin = (POINT *)malloc (sizeof (POINT));

      pDistMin->next = 0x0;
      pDistMin->x = 0.0;
      pDistMin->y = 0.0;
      pDistMin->cnt = 0.0;
      pDistMin->xSum = 0.0;
      pDistMin->xSum = 0.0;

      if (pointTail)
	{
	  pointTail->next = pDistMin;
	  pointTail = pDistMin;
	}
      else
	{
	  pointHead = pDistMin;
	  pointTail = pDistMin;
	}
    }

  pDistMin->cnt += 1.0;
  pDistMin->xSum += xe;
  pDistMin->ySum += ye;
  pDistMin->x = pDistMin->xSum / pDistMin->cnt;
  pDistMin->y = pDistMin->ySum / pDistMin->cnt;

  POINT *pe = pDistMin;

  //

  LINE *l = (LINE *)malloc (sizeof (LINE));

  l->next = 0x0;
  l->p0 = p0;
  l->pe = pe;

  if (lineTail)
    {
      lineTail->next = l;
      lineTail = l;
    }
  else
    {
      lineHead = l;
      lineTail = l;
    }

  numLines++;
}


static void DumpLines (void)
{
  LINE *l = lineHead;

  while (l)
    {
      printf ("%.3f %.3f %.3f %.3f\n", l->p0->x, l->p0->y, l->pe->x, l->pe->y);

      l = l->next;
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static void convert (FILE *fp)
{
  char buf[256];

  while (fgets (buf, 256, fp))
    {
      // convert ',' to '.'

      char *ptr = buf;

      while ((ptr = strchr (ptr, ',')))
	{
	  *ptr++ = '.';
	}

      // read 4 doubles

      double x0, y0, xe, ye;

      sscanf (buf, "%lf %lf %lf %lf", &x0, &y0, &xe, &ye);

      //

      const double inch2meter = 0.0245;

      x0 *= inch2meter;
      y0 *= inch2meter;
      xe *= inch2meter;
      ye *= inch2meter;

      AddLine (x0, y0, xe, ye);
    }

  DumpLines ();
}


int main (int argc, char **argv)
{
  if (argc != 2)
    {
      fprintf (stderr, "usage: %s <mapfile>\n", argv[0]);
      return -1;
    }

  FILE *fp = fopen (argv[1], "r");

  if (!fp)
    {
      perror ("fopen()");
      return -1;
    }

  convert (fp);

  fclose (fp);

  return 0;
}
