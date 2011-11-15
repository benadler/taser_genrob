//
//
//

#ifndef _LASER_H_
#define _LASER_H_

#include "thread/thread.h"
#include "thread/condition.h"

#include "localisation/localisation.h"

#include "./radialscan.h"
#include "./rawlaser.h"
#include "./map.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class CLASER : protected CTHREAD
{
private:

  // not even implemented
  CLASER (void);
  CLASER (const CLASER &);
  CLASER& operator= (const CLASER &);
  bool operator== (const CLASER &);

protected:

  int _idx;
  unsigned int *_upAndRunningMask;

  CCONDITION *_condition;

  _CRAWLASER *_rawLaser;

  //

  virtual void Fxn (void);

public:

  typedef enum
    {
      SICK_LMS6,
      SICK_SIM
    } TYPE_t;

  CLASER (const int idx,
	  const TYPE_t type,
	  CCONDITION *condition,   // used for production
	  unsigned int *upAndRunningMask,
	  const double x0,
	  const double y0,
	  const double a0,
	  CMAP *map,
	  Localisation *loc);

  virtual ~CLASER (void);

  //
  // public data to be read by everyone
  //

  double _x0, _y0, _a0;

  struct timeval _tv;

  CRADIALSCAN _scanScanner;
  CRADIALSCAN _scanPlatform;

  CRADIALSCANMATCHED _scanPlatformMatched;

  double _closestObstacleDistance;
};


#endif
