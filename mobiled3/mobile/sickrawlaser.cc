//
// sickrawlaser.cc
//

#include <math.h>
#include <stdlib.h>

//#ifdef DEBUG
//#undef DEBUG
//#endif
#include "thread/tracelog.h"

#include "./sickrawlaser.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CSICKSCAN::CSICKSCAN (const int numValues,
		      const double angleStart,
		      const double angleInc)
{
  GTL ("CSICKSCAN::CSICKSCAN()");

  _angleStart = angleStart;
  _angleInc = angleInc;

  if (!(_dist = (float *)malloc ((_numValues = numValues) * sizeof (float))))
    {
      GTLFATAL (("out of memory\n"));
      abort ();
    }

#if MOBILEVERSION == HAMBURG
  if (!(_intensity = (unsigned char *)malloc (numValues * sizeof (unsigned char))))
#else
  if (!(_blend = (bool *)malloc (numValues * sizeof (bool))))
#endif
    {
      GTLFATAL (("out of memory\n"));
      abort ();
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CSICKSCAN::~CSICKSCAN (void)
{
  GTL ("CSICKSCAN::~CSICKSCAN()");

#if MOBILEVERSION == HAMBURG
  free (_intensity);
#else
  free (_blend);
#endif
  free (_dist);
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

_CSICKRAWLASER::_CSICKRAWLASER (void)
  : _sickScan (361, -M_PI / 2.0, 0.5 * M_PI / 180.0)
{
  GTL ("_CSICKRAWLASER::_CSICKRAWLASER()");
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

_CSICKRAWLASER::~_CSICKRAWLASER (void)
{
  GTL ("_CSICKRAWLASER::~_CSICKRAWLASER()");
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void _CSICKRAWLASER::GetScanContinuous (CRADIALSCAN &radialScan)
{
  GTL ("_CSICKRAWLASER::GetScanContinuous(CRADIALSCAN&)");

  //
  // (blocking) read SICK scan
  //

  GetScanContinuousSick (_sickScan);

  //
  // extract laser marks: the sick scanner marks reflections with a bit for
  // each scan point, but it may set this bit for a contiguous range of
  // points. the next piece of code detects these ranges and reduces them to a
  // single point, the coordinates of which are the averaged values from the
  // respective range.
  // ++westhoff20040401: for robot HAMBURG the 3bit-intensity values of
  //                     the reflections are used to weigh each the average
  //                     calculation

  double angle = _sickScan._angleStart;
  const float *srcDistPtr = _sickScan._dist;

#if MOBILEVERSION == HAMBURG
  const unsigned char *srcIntensityPtr = _sickScan._intensity;
#else
  const bool *srcBlendPtr = _sickScan._blend;
#endif

  int numScans = 0;

  float *dstScanAnglePtr = radialScan._scanAngle;
  float *dstScanDistPtr = radialScan._scanDist;

  int numMarks = 0;
  int markSize = 0;

  double markAngle = 0.0;
  double markDist = 0.0;

#if MOBILEVERSION == HAMBURG
  unsigned char lastIntensity = 0;
#else
  bool lastBlend = false;
#endif

  float *dstMarkAnglePtr = radialScan._markAngle;
  float *dstMarkDistPtr = radialScan._markDist;

  //

  for (int i=0; i<_sickScan._numValues; i++)   // probably 361
    {

#if MOBILEVERSION == HAMBURG
      unsigned char intensity = *srcIntensityPtr++;
#else
      bool blend = *srcBlendPtr++;
#endif

      const double dist = *srcDistPtr++;

      if (dist < 0.0)
	{

#if MOBILEVERSION == HAMBURG
          intensity = 0;
#else
	  blend = false;   // error in scan, abort all marks
#endif

	}

      // deal with marks

#if MOBILEVERSION == HAMBURG

      // ++westhoff20040623: just take reflections with an intensity > 1
      //                     into acount

      if (intensity > 1)
        {
          markSize += intensity;
          markAngle += (double) intensity * angle;
          markDist += (double) intensity * dist;
        }
#else
      if (blend)   // accumulate values
	{
	  markSize++;
	  markAngle += angle;
	  markDist += dist;
	}
#endif

#if MOBILEVERSION == HAMBURG
      else if (lastIntensity > 1)
#else
      else if (lastBlend)   // transition true->false is counted as end of mark
#endif
	{
	  // compute mark

	  numMarks++;
	  *dstMarkAnglePtr++ = markAngle / (double)markSize;
	  *dstMarkDistPtr++ = markDist / (double)markSize;

	  // reset next mark for accumulation
	  markSize = 0;
	  markAngle = 0.0;
	  markDist = 0.0;

#if MOBILEVERSION == HAMBURG
          GTLPRINT (("mark: angle = %5.1f°   distance = %5.3f m\n",
                           *(dstMarkAnglePtr-1) * 180.0 / M_PI,
                           *(dstMarkDistPtr-1)));
#endif
	}

#if MOBILEVERSION == HAMBURG
      lastIntensity = intensity;
#else
      lastBlend = blend;
#endif

      //

      if (dist >= 0.0)
	{
	  *dstScanAnglePtr++ = angle;
	  *dstScanDistPtr++ = dist;
	  numScans++;
	}

      //

      angle += _sickScan._angleInc;
    }

  radialScan._numScans = numScans;

#if MOBILEVERSION == HAMBURG
  if (lastIntensity > 1)
#else
  if (lastBlend)   // finish last mark
#endif
    {
      numMarks++;
      *dstMarkAnglePtr = markAngle / (double)markSize;
      *dstMarkDistPtr = markDist / (double)markSize;
    }

  radialScan._numMarks = numMarks;

  GTLPRINT (("%i scans, %i marks\n", numScans, numMarks));
}
