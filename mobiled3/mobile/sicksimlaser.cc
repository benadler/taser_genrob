//
// sicksimlaser.cc
//
//

#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef DEBUG
#undef DEBUG
#endif
#include "thread/tracelog.h"

#include "math/vec.h"
#include "util/normal.h"

#include "./sicksimlaser.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CSICKSIMLASER::CSICKSIMLASER (const int idx,
			      CMAP *map,
			      Localisation *loc,
			      const double x0,
			      const double y0,
			      const double a0)
{
  GTL ("CSICKSIMLASER::CSICKSIMLASER(const int)");

  _idx = idx;
  _map = map;
  _loc = loc;
  _x0 = x0;
  _y0 = y0;
  _a0 = a0;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CSICKSIMLASER::~CSICKSIMLASER (void)
{
  GTL ("CSICKSIMLASER::~CSICKSIMLASER()");
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CSICKSIMLASER::Reset (void)
{
  GTL ("CSICKSIMLASER::Reset()");

  GTLFATAL (("laser #%i up and running\n", _idx));
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CSICKSIMLASER::StartScanContinuous (void)
{
  GTL ("CSICKSIMLASER::StartScanContinuous()");
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static double inline threshold (const double arg)
{
  if (arg < -0.001)
    {
      return arg;
    }

  if (arg > 0.001)
    {
      return arg;
    }

  return 0.0;
}


static double inline fsgn (const double arg)
{
  if (arg < 0.0)
    {
      return -1.0;
    }

  return 1.0;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static bool intersects (const double xRay,
			const double yRay,
			const double aRay,
			const double x0,
			const double y0,
			const double xe,
			const double ye,
			double &xInter,
			double &yInter)
{
  // compute ray direction. the angle is supposed to be counted positive around
  // the z-axis with 0 at the x-axis.

  CVEC rayDir (cos (aRay), sin (aRay));

  // chose a perpendicular direction. let the original direction be the x-axis
  // of a right-handed coordinate system and let the z-axis come out of the
  // plane towards the observer. then the y-axis is the perpendicular axis.

  CVEC rayDirPerp (-rayDir[1], rayDir[0]);

  // move the coordinate system to the ray origin.

  CVEC l0 (x0 - xRay, y0 - yRay);
  CVEC le (xe - xRay, ye - yRay);

  // now compute the projections of the start point (l0) and end point (le) of
  // the line onto this perpendicular direction. if the ray is supposed to
  // intersect the line the projections must have different signs.
  //
  // if they don't (and maybe only because of slight rounding errors or because
  // the line is too short) this means that the ray is almost parallel to the
  // line. in this case a computation of the intersection point (if it still
  // exists) would mainly depend on rounding errors, so we just assume that it
  // doesn't intersect at all.
  //
  // on the other hand, if the signs are different we can be somewhat sure that
  // further computations are almost safe.
  //
  // to be even more on the safer side we don't simply require the signs to be
  // different, we even require the absolute value to be greater than a small
  // threshold (1mm). so the signs are computed from the thresholded values.

  const double l0Proj = threshold (l0 * rayDirPerp);
  const double leProj = threshold (le * rayDirPerp);

  if ((l0Proj == 0.0) ||   // projection of start too short
      (leProj == 0.0) ||   // projection of end too short
      (fsgn (l0Proj) == fsgn (leProj)))   // projections on same side
    {
      return false;
    }

  // now one projection is negative and the other is positive. also their
  // distance is at least 2mm.
  //
  // regardless of which projection is negative we can compute a total
  // projection (the distance between the two projections) and the `start'
  // projection (the fraction of the start point of the line of the total
  // projection).

  const double totalProjLen = fabs (leProj - l0Proj);
  const double zeroProjLen = fabs (l0Proj);

  // again, we require the total projection to be larger than 1mm, but that
  // should never fail (because of the above test). if it does, well, then we
  // assume no intersection...

  if (totalProjLen < 0.001)
    {
      fprintf (stderr,
	       "fatal: projection too short, how could this happen?\n");
      return false;
    }

  // the ratio of these projections is the same as the ratio with which the
  // intersection points cuts the original line.

  const double scale = zeroProjLen / totalProjLen;

  CVEC r0 (xRay, yRay);

  CVEC inter = l0 + scale * (le - l0) + r0;

  // however, we still don't know if the intersection point is in front of us
  // or behind. we never look back ;-) so we ignore it if it is.

  if ((inter - r0) * rayDir < 0.0)
    {
      return false;
    }

  // now we've got a valid intersection point! note that all the computations
  // here used well defined and reasonable thresholds and are numerically
  // stable.

  xInter = inter[0];
  yInter = inter[1];

  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static inline double sqr (const double arg)
{
  return arg * arg;
}


///////////////////////////////////////////////////////////////////////////////
//
// CSICKSIMLASER::GetScanContinuousSick()
//
// simulates a SICK laser scan by taking the current position and map and
// intersect all laser rays with all lines in the map. this isn't really very
// efficient but just like for everybody and everything else this doesn't count
// right now. what counts is that it works.
//
// (hey, if *that's* the general attitude then why are you still wondering why
// computers and their programs appear more buggy than anything else human
// beings have ever built? :-)
//
//
///////////////////////////////////////////////////////////////////////////////

void CSICKSIMLASER::GetScanContinuousSick (CSICKSCAN &sickScan)
{
  GTL ("CSICKSIMLASER::GetScanContinuousSick(CSICKSCAN&)");

  //
  // reduce speed to (33.3hz) 25hz
  //

  usleep (30000);   // 40000

  //
  // get the current position
  //

  double x0, y0, a0;

#if 1

  _loc->GetPos (x0, y0, a0);

#else

  if (_loc->GetPos (x0, y0, a0) < 0)
    {
      GTLFATAL (("Localisation::GetPos() failed\n"));
      sickScan._numValues = 0;
      return;
    }

  sickScan._numValues = 361;

#endif

  x0 /= 1000.0;
  y0 /= 1000.0;

  //
  // compute the position of the scanner
  //

  const double c = cos (a0);
  const double s = sin (a0);

  x0 += c * _x0 - s * _y0;
  y0 += s * _x0 + c * _y0;
  a0 += _a0;

  //
  //
  //

  float *distPtr = sickScan._dist;

#if MOBILEVERSION == HAMBURG
#warning Arrrrrrrgh!!!! Ob das gut geht, ist noch nicht ausprobiert worden!
  unsigned char *blendPtr = sickScan._intensity;
#else
  bool *blendPtr = sickScan._blend;
#endif

  double angle = sickScan._angleStart;
  const double angleInc = sickScan._angleInc;

#if 1
  static Normal normal (0.0, 0.0001);
#endif

  for (int i = 0; i < sickScan._numValues; i++)
    {
      //
      // first the distance: test a ray from x0,y0,a0+angle against the map.
      //

      const double xRay = x0;
      const double yRay = y0;
      const double aRay = a0 + angle;

      double distMin = 8.192;
      double xInterMin, yInterMin;

      for (int l=0; l<_map->_numLines; l++)
	{
	  double xInter, yInter;

	  if (intersects (xRay, yRay, aRay,
			  _map->_line[l].x0, _map->_line[l].y0,
			  _map->_line[l].xe, _map->_line[l].ye,
			  xInter, yInter))
	    {
#if 1
	      xInter += 2.0 * normal ();
	      yInter += 0.2 * normal ();
#endif
	      const double dist = sqrt (sqr (xInter - x0) +
					sqr (yInter - y0));

	      if (dist < distMin)
		{
		  distMin = dist;

		  xInterMin = xInter;
		  yInterMin = yInter;
		}
	    }
	}

      if (distMin > 8.0)
	{
	  distMin = -1.0;
	}

      *distPtr++ = distMin;

      //
      // second the marks: if there was an intersection point test if it is
      // close enough to a mark (in the static map).
      //

      bool blend = false;

#if 1

      if (distMin >= 0.0)
	{
	  CMAP::SMARK_t *mark = _map->_mark;

	  for (int m=0; m<_map->_numMarks; m++)
	    {
	      const double distMark = sqrt (sqr (mark[m].x - xInterMin) +
					    sqr (mark[m].y - yInterMin));

	      if (distMark < 0.02)
		{
		  blend = true;
		  break;
		}
	    }
	}

#endif

      *blendPtr++ = blend;

      //

      angle += angleInc;
    }

  //
}
