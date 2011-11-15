//
// laser.cc
//

#include <math.h>
#include <stdlib.h>
#include <sys/time.h>

#ifdef DEBUG
#undef DEBUG
#endif
#include "thread/tracelog.h"

#include "./laser.h"
#include "./sicklaser.h"
#include "./sicksimlaser.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#undef DEBUG_RATE
#undef DEBUG_SERIALPORT


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CLASER::CLASER (const int idx,
		const TYPE_t type,
		CCONDITION *condition,   // used for production
		unsigned int *upAndRunningMask,
		const double x0,
		const double y0,
		const double a0,
		CMAP *map,
		Localisation *loc)
{
  GTL ("CLASER::CLASER()");

  _idx = idx;
  _upAndRunningMask = upAndRunningMask;
  _condition = condition;

  _x0 = x0;
  _y0 = y0;
  _a0 = a0;

  gettimeofday (&_tv, 0x0);

  _closestObstacleDistance = 0.0;

  switch (type)
    {
    case SICK_LMS6:
#ifdef SIMONLY
      GTLFATAL (("real SICK laser not available in simulation mode\n"));
      abort ();
#else
      _rawLaser = new CSICKLASER (idx, true);
#endif
      break;

    case SICK_SIM:
      _rawLaser = new CSICKSIMLASER (idx, map, loc, x0, y0, a0);
      break;

    default:
      GTLFATAL (("unknown laser type\n"));
      abort ();
    }

  //
  // the thread does all the remaining laser initialization
  //

  Start ();
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CLASER::~CLASER (void)
{
  GTL ("CLASER::~CLASER()");
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static inline double fsqr (const double arg)
{
  return arg * arg;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CLASER::Fxn (void)
{
  GTL ("CLASER::Fxn()");

  ConfirmRunning ();

  //
  // reset and set the laser into continuous mode (this may take a while, which
  // is why this is already part of the per-laser-thread).
  //

  GTLPRINT (("reseting laser\n"));

  _rawLaser->Reset ();

  GTLPRINT (("starting continuous scanning\n"));

  _rawLaser->StartScanContinuous ();

  // ++itschere20030912: rate measurements

#ifdef DEBUG_RATE

  int cnt = 0;
  struct timeval rateStart;

  gettimeofday (&rateStart, 0x0);

#endif

  // ++itschere20031217: serial port error debugging output

#ifdef DEBUG_SERIALPORT

  struct timeval lastSerialPortOutput;

  gettimeofday (&lastSerialPortOutput, 0x0);

#endif

  //
  // the big loop
  //

  CRADIALSCAN scanScanner;

  while (!_stopRequested)
    {
#if defined (DEBUG_RATE) || defined (DEBUG_SERIALPORT)
      struct timeval now;
      double s;
#endif

      // blocking read() of laser scanner

      _rawLaser->GetScanContinuous (scanScanner);

#ifdef DEBUG_RATE

      cnt++;

      gettimeofday (&now, 0x0);

      s = (now.tv_sec - rateStart.tv_sec) +
	(now.tv_usec - rateStart.tv_usec) / 1000000.0;

      if (s >= 1.0)
	{
	  if (!_idx)
	    {
	      GTLFATAL (("laser %i rate : %i in %fs -> %f/s\n",
			 _idx,
			 cnt,
			 s,
			 cnt / s));
	    }

	  cnt = 0;
	  rateStart = now;
	}
#endif

      //

#ifdef DEBUG_SERIALPORT

      gettimeofday (&now, 0x0);

      s = (now.tv_sec - lastSerialPortOutput.tv_sec) +
	(now.tv_usec - lastSerialPortOutput.tv_usec) / 1000000.0;

      if (s >= 10.0)
	{
	  int rx, rxOverrun, tx;

	  _rawLaser->GetSerialPortStatistics (rx, rxOverrun, tx);

	  GTLFATAL (("laser %i: rx=%i, rxOver=%i, tx=%i\n",
		     _idx, rx, rxOverrun, tx));

	  lastSerialPortOutput = now;
	}

#endif

      // lock the data and update them

      _condition->Lock ();

      *_upAndRunningMask |= 1 << _idx;

      gettimeofday (&_tv, 0x0);

      _scanScanner = scanScanner;

      //
      // convert from scanner coordinate system into platform coordinate system
      // (needed by localization) and search for closest obstacle.
      //

      // 1. the scan points

      double closestObstacleDist = 9.0;

      const float *srcAnglePtr = scanScanner._scanAngle;
      const float *srcDistPtr = scanScanner._scanDist;

      float *dstScanAnglePtr = _scanPlatform._scanAngle;
      float *dstScanDistPtr = _scanPlatform._scanDist;

      for (int i=0; i<scanScanner._numScans; i++)
	{
	  double angle = _a0 + *srcAnglePtr++;
	  double dist = *srcDistPtr++;

	  const double x = _x0 + cos (angle) * dist;
	  const double y = _y0 + sin (angle) * dist;

	  angle = atan2 (y, x);
	  dist = sqrt (fsqr (x) + fsqr (y));

	  if (dist < closestObstacleDist)
	    {
	      closestObstacleDist = dist;
	    }

	  *dstScanAnglePtr++ = angle;
	  *dstScanDistPtr++ = dist;
	}

      _scanPlatform._numScans = scanScanner._numScans;

      // 2. the marks

      srcAnglePtr = scanScanner._markAngle;
      srcDistPtr = scanScanner._markDist;

      float *dstMarkAnglePtr = _scanPlatform._markAngle;
      float *dstMarkDistPtr = _scanPlatform._markDist;

      for (int i=0; i<scanScanner._numMarks; i++)
	{
	  double angle = _a0 + *srcAnglePtr++;
	  double dist = *srcDistPtr++;

	  const double x = _x0 + cos (angle) * dist;
	  const double y = _y0 + sin (angle) * dist;

	  angle = atan2 (y, x);
	  dist = sqrt (fsqr (x) + fsqr (y));

	  if (dist < closestObstacleDist)
	    {
	      closestObstacleDist = dist;
	    }

	  // GTLPRINT (("%i : %.3fm @ %.3f°\n", i+1, dist, angle * 180.0 / M_PI));

	  *dstMarkAnglePtr++ = angle;
	  *dstMarkDistPtr++ = dist;
	}

      _scanPlatform._numMarks = scanScanner._numMarks;

      _closestObstacleDistance = closestObstacleDist;

      //
      // we're done, signal the condition and release the lock
      //

      gettimeofday (&_tv, 0x0);

      _condition->Signal ();

      _condition->Unlock ();
    }

  //
  //
  //

  GTLPRINT (("thread aborting...\n"));
}
