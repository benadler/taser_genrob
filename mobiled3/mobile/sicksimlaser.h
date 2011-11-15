//
// sicksimlaser.h
//

#ifndef _SICKSIMLASER_H_
#define _SICKSIMLASER_H_

#include "localisation/localisation.h"

#include "./map.h"
#include "./sickrawlaser.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class CSICKSIMLASER : public _CSICKRAWLASER
{
private:

  // not even implemented

  CSICKSIMLASER (void);
  CSICKSIMLASER (const CSICKSIMLASER &);
  CSICKSIMLASER operator= (const CSICKSIMLASER &);
  CSICKSIMLASER operator== (const CSICKSIMLASER &);

  //

protected:

  int _idx;
  CMAP *_map;
  Localisation *_loc;
  double _x0, _y0, _a0;

  virtual void Reset (void);

public:

  //
  //
  //

  CSICKSIMLASER (const int idx,
		 CMAP *map,
		 Localisation *loc,
		 const double x0,
		 const double y0,
		 const double a0);

  virtual ~CSICKSIMLASER (void);

  //
  //
  //

  virtual void StartScanContinuous (void);

  virtual void GetScanContinuousSick (CSICKSCAN &scan);
};


#endif
