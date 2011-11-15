//
//
//

#include <math.h>
#include <unistd.h>

#include "./laserFeeder.h"
#include "./genBase.h"

#include "math/vec.h"

//#ifdef DEBUG
//#undef DEBUG
//#endif
#include "thread/tracelog.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CLASERFEEDER::CLASERFEEDER (CGENBASE *genBase, const bool simulate)
{
  GTL ("CLASERFEEDER::CLASERFEEDER()");

  //

  _upAndRunningMask = 0;

  _genBase = genBase;   // for callbacks

  //

  try
    {
//#if MOBILEVERSION == HAMBURG
//      _laser = new CLASER * [_numLaser = 1];
//#else
      _laser = new CLASER * [_numLaser = 2];
//#endif

      // ++itschere20030808: can safely pass `_localisationOdo()' here. in
      // normal mode it is 0x0 but not used at all, and in simulation mode it
      // really exists. this is handled in `genBase.cc'.

      _laser[0] = new CLASER (0,
			      simulate ? CLASER::SICK_SIM : CLASER::SICK_LMS6,
			      &_cond, &_upAndRunningMask,
			      0.336, 0.0, 0.0,
			      &genBase->_map,
			      genBase->_localisationOdo);

//#if MOBILEVERSION != HAMBURG

      // ++westhoff20031212: mobile robot HAMBURG has only one laser scanner.
      // the scanner must be attached to the first port of the MOXA serial
      // card, otherwise it will not be found!
      // ++westhoff20040621: now the robot has two laserscanner

      _laser[1] = new CLASER (1,
			      simulate ? CLASER::SICK_SIM : CLASER::SICK_LMS6,
			      &_cond, &_upAndRunningMask,
			      -0.336, 0.0, M_PI,
			      &genBase->_map,
			      genBase->_localisationOdo);
//#endif

      //

      const unsigned int waitMask = (1 << _numLaser) - 1;

      _cond.Lock ();

      while ((_upAndRunningMask & waitMask) != waitMask)
	{
	  GTLFATAL (("mask is 0x%x, waiting for 0x%x\n",
		     _upAndRunningMask,
		     waitMask));
	  _cond.Wait ();
	}

      _cond.Unlock ();

      GTLFATAL (("mask is 0x%x, lasers are up and running\n", waitMask));
    }

  catch (...)
    {
      GTLFATAL (("error initalizing laser(s)\n"));
      throw;
    }

  //

  GTLFATAL (("starting laser receiver thread\n"));

  Start ();
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CLASERFEEDER::~CLASERFEEDER (void)
{
  GTL ("CLASERFEEDER::~CLASERFEEDER()");

  for (int i=0; i<_numLaser; i++)
    {
      delete _laser[i];
    }

  if (_laser)
  {
      delete[] _laser;
  }

  _laser = 0x0;
  _numLaser = 0;

  Stop ();
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////
inline void dumplaser(const CVEC &mark, int *matched, int numMarks) {

    printf("numMarks: %d\n", numMarks);
    int i2 = 0;
    for (int i=0; i < numMarks; i++) {
	printf("%d: %f %f - %d\n", i, mark[i2++], mark[i2++], matched[i]);
    }
}

void CLASERFEEDER::Fxn (void)
{
  GTL ("CLASERFEEDER::Fxn()");

  ConfirmRunning ();

  GTLFATAL (("pid=%i\n", getpid ()));

  ScheduleRealtime (20);
  nice (-14);   // for `top' statistics only, not used in rt-scheduling

  //

#if 0
  // ++itschere20030912: rate measurements
  int cnt = 0;
  struct timeval start;
  gettimeofday (&start, 0x0);
#endif

  while (!_stopRequested)
    {
      // timeout?

      _cond.Lock ();

      if (!_cond.WaitTimeout (500))   // milliseconds
	{
	  _cond.Unlock ();
	  continue;
	}

      // nope, got new data. but we've got no idea for which laser...

      struct timeval now;

      gettimeofday (&now, 0x0);

      int usMax = 0;

      for (int l=0; l<_numLaser; l++)
	{
	  int us = now.tv_sec > _laser[l]->_tv.tv_sec ?
	    1000000 + now.tv_usec - _laser[l]->_tv.tv_usec :   // `>'
	    now.tv_usec - _laser[l]->_tv.tv_usec;   // `=' (`<' not possible)
#if 0
	  GTLPRINT (("laser %i: %6i us\n", l, us));
#endif
	  if (us > usMax)
	    {
	      usMax = us;
	    }
	}

#warning "TUNEABLE PARAMETER"

      if (usMax > 40000)
	{
	  _cond.Unlock ();
#if 0
	  GTLPRINT (("scan too old, ignoring this run\n"));
#endif
	  continue;
	}

#if 0
      cnt++;
      double s =
	(now.tv_sec - start.tv_sec) +
	(now.tv_usec - start.tv_usec) / 1000000.0;
      if (s >= 1.0)
	{
	  GTLFATAL (("laserFeeder rate : %i in %fs -> %f/s\n",
		     cnt,
		     s,
		     cnt / s));
	  cnt = 0;
	  start = now;
	}
#endif

      for (int l=0; l<_numLaser; l++)
	{
	  _laser[l]->_tv.tv_sec = 0;   // mark `scanDataInput' as empty
	}

      //
      // got a complete set of scans.
      //

      int numMarks = 0;

      for (int l=0; l<_numLaser; l++)
	{
	  numMarks += _laser[l]->_scanPlatform._numMarks;
	}

      GTLPRINT (("got %i marks\n", numMarks));

      //
      //
      //

      int matched[numMarks];

      for (int i=0; i<numMarks; i++)
	{
	  matched[i] = -1;
	}

      //
      //
      //

#if 1
      if (numMarks >= 1)
#else
      if (numMarks >= 3)
#endif
	{
	  int idx = 0;

	  CVEC mark (2 * numMarks);

	  for (int l=0; l<_numLaser; l++)
	    {
	      for (int m=0; m<_laser[l]->_scanPlatform._numMarks; m++)
		{
#if 0
		  GTLPRINT (("mark %i@%i: %.3fm %.1f°\n",
			     m+1,
			     l+1,
			     _laser[l]->_scanPlatform._markDist[m],
			     180.0 * _laser[l]->_scanPlatform._markAngle[m] / M_PI));
#endif
		  mark[idx++] = _laser[l]->_scanPlatform._markDist[m] * 1000.0;   // mm
		  mark[idx++] = _laser[l]->_scanPlatform._markAngle[m];
		}
	    }

	  //
	  // finally, feed the results to the localization
	  //

	  //dumplaser(mark,matched,numMarks);
	  _genBase->UpdateLaser (mark, matched);
	}

      //
      // copy data to `_scanPlatformMatched'. note we have to do this even if
      // we didn't call the localization because we still want the
      // visualization to work. otherwise if no mark is visible the
      // visualization would freeze.
      //

      int idx = 0;

      for (int l=0; l<_numLaser; l++)
	{
	  _laser[l]->_scanPlatformMatched = _laser[l]->_scanPlatform;

	  for (int m=0; m<_laser[l]->_scanPlatform._numMarks; m++)
	    {
	      _laser[l]->_scanPlatformMatched._markIdx[m] = matched[idx++];
	    }
	}

      //
      // we're done with touching _laser[*].scan[*], unlock the condition
      //

      _cond.Unlock ();

      //
      // loop
      //
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CLASERFEEDER::GetClosestObstacleDistance (const int idx,
					      double &distance)
{
  GTL (("CLASERFEEDER::GetClosestObstacleDistance()"));

  if ((idx < 0) || (idx >= _numLaser))
    {
      //GTLPRINT (("illegal laser index %i (0..%i)\n", idx, _numLaser - 1));

      distance = +10.0;

      return -1;
    }

  distance = _laser[idx]->_closestObstacleDistance;

  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CLASERFEEDER::GetLaserScanScanner (const int idx,
				       CRADIALSCAN &scan)
{
  GTL (("CLASERFEEDER::GetLaserScanScanner()"));

  printf("ScanScanner\n");

  if ((idx < 0) || (idx >= _numLaser))
    {
      GTLPRINT (("illegal laser index %i (0..%i)\n", idx, _numLaser-1));
      return -1;
    }

  _cond.Lock ();

  scan = _laser[idx]->_scanScanner;

  _cond.Unlock ();

  return 0;
}


int CLASERFEEDER::GetLaserScanPlatform (const int idx,
					CRADIALSCAN &scan)
 {
  GTL (("CLASERFEEDER::GetLaserScanPlatform()"));

  if ((idx < 0) || (idx >= _numLaser))
    {
      GTLPRINT (("illegal laser index %i (0..%i)\n", idx, _numLaser-1));
      return -1;
    }

  _cond.Lock ();

  scan = _laser[idx]->_scanPlatform;

  _cond.Unlock ();

  return 0;
}


int CLASERFEEDER::GetLaserScanPlatformMatched (const int idx,
					       CRADIALSCANMATCHED &scan)
{
  GTL (("CLASERFEEDER::GetLaserScanPlatformMatched()"));

  if ((idx < 0) || (idx >= _numLaser))
    {
      GTLPRINT (("illegal laser index %i (0..%i)\n", idx, _numLaser-1));
      return -1;
    }

  _cond.Lock ();

  scan = _laser[idx]->_scanPlatformMatched;

  _cond.Unlock ();

  return 0;
}
