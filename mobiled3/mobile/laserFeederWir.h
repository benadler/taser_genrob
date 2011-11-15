// laserFeeder.h

#ifndef _LASERFEEDER_H_
#define _LASERFEEDER_H_

#include "thread/thread.h"
#include "thread/condition.h"

#include "iotypes.h"
#include "radialscan.h"

class CLASERFEEDER : protected CTHREAD {
  friend class CGENBASE;

private:

  // not even implemented
/*
  CLASERFEEDER (void);
  CLASERFEEDER (const CLASERFEEDER &);
  CLASERFEEDER &operator= (const CLASERFEEDER &);
  CLASERFEEDER &operator== (const CLASERFEEDER &);
*/

protected:

  CGENBASE *_genBase;

  int _numLaser;

  double *_x0, *_y0, *_a0;

  int fd;

  unsigned char *buf;
  size_t buflen;

  unsigned *_alarm_level;
  unsigned *_platform_valid;

  CRADIALSCAN **_scanner;
  CRADIALSCANMATCHED **_platform;

  virtual void Fxn (void);
  void marks2matched(unsigned char *const buf, size_t l, int idx);
  void raw2radial(unsigned char *const buf, int idx);

  time_t *scantime;
  time_t *marktime;

public:

  CLASERFEEDER (CGENBASE *genBase, const bool simulate);

  virtual ~CLASERFEEDER (void);

  int GetClosestObstacleDistance (const int idx, double &distance);

  int GetLaserScanScanner (const int idx, CRADIALSCAN &scan);
  int GetLaserScanPlatform (const int idx, CRADIALSCAN &scan);
  int GetLaserScanPlatformMatched (const int idx, CRADIALSCANMATCHED &scan);

  // conversts scandata from scanner to platform coordinate system
  void updatePlatform(const int idx);

  int GetNumScanners (void) const {
      return _numLaser;
  }

  int GetScannerPosition (meter_t &x,
			  meter_t &y,
			  radiant_t &a,
			  const int idx) const {
      if ((idx < 0) || (idx >= _numLaser)) {
	  return -1;
      }

      x = _x0[idx];
      y = _y0[idx];
      a = _a0[idx];

      return 0;
  }
};

#endif
