//
// laserFeeder.h
// (C) 02/2003 by TeSche (Torsten Scherer)
// <itschere@techfak.uni-bielefeld.de>
//

#ifndef _LASERFEEDER_H_
#define _LASERFEEDER_H_

#include <sys/time.h>

#include "./units.h"

#include "thread/thread.h"
#include "thread/condition.h"
#include "./laser.h"

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class CLASERFEEDER : protected CTHREAD
{
  friend class CGENBASE;

private:

  // not even implemented
  CLASERFEEDER (void);
  CLASERFEEDER (const CLASERFEEDER &);
  CLASERFEEDER &operator= (const CLASERFEEDER &);
  CLASERFEEDER &operator== (const CLASERFEEDER &);

protected:

  //

  int _numLaser;
  CLASER **_laser;

  unsigned int _upAndRunningMask;
  CCONDITION _cond;

  //

  CGENBASE *_genBase;

  //

  virtual void Fxn (void);

public:

  CLASERFEEDER (CGENBASE *genBase, const bool simulate);

  virtual ~CLASERFEEDER (void);

  int GetClosestObstacleDistance (const int idx, double &distance);

  int GetLaserScanScanner (const int idx, CRADIALSCAN &scan);
  int GetLaserScanPlatform (const int idx, CRADIALSCAN &scan);
  int GetLaserScanPlatformMatched (const int idx, CRADIALSCANMATCHED &scan);

  //

  int GetNumScanners (void) const
    {
      return _numLaser;
    }

  int GetScannerPosition (meter_t &x,
			  meter_t &y,
			  radian_t &a,
			  const int idx) const
    {
      if ((idx < 0) || (idx >= _numLaser))
	{
	  return -1;
	}

      x = _laser[idx]->_x0;
      y = _laser[idx]->_y0;
      a = _laser[idx]->_a0;

      return 0;
    }
};

#endif
