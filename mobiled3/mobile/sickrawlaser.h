//
// sickrawlaser.h
//

#ifndef _SICKRAWLASER_H_
#define _SICKRAWLASER_H_

#include "./rawlaser.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class CSICKSCAN
{
  friend class _CSICKRAWLASER;

private:

  // not even implemented
  CSICKSCAN (void);
  CSICKSCAN (const CSICKSCAN &);
  CSICKSCAN &operator= (const CSICKSCAN &);
  bool &operator== (const CSICKSCAN &);

protected:

  CSICKSCAN (const int numValues,
	     const double angleStart,
	     const double angleInc);

public:

  virtual ~CSICKSCAN (void);

  //

  int _numValues;
  double _angleStart;
  double _angleInc;

  float *_dist;

#if MOBILEVERSION == HAMBURG
  unsigned char *_intensity;
#else
  bool *_blend;
#endif
};


///////////////////////////////////////////////////////////////////////////////
//
// a _CSICKRAWLASER is just a middle thing between a _CRAWLASER and a
// CSICKLASER, say, it's a CSICKLASER with the possibility to convert from
// CSICKSCANs to CRADIALSCANs. this routine is needed by the physical laser as
// well as by the simulated laser and I didn't want to have it in two source
// files - so it ended up here.
//
///////////////////////////////////////////////////////////////////////////////

class _CSICKRAWLASER : public _CRAWLASER
{
private:

  // not even implemented

  _CSICKRAWLASER (const _CSICKRAWLASER &);
  _CSICKRAWLASER operator= (const _CSICKRAWLASER &);
  _CSICKRAWLASER operator== (const _CSICKRAWLASER &);

protected:

  // variable used by `GetScanContinuous()'

  CSICKSCAN _sickScan;

public:

  //
  //
  //

  _CSICKRAWLASER (void);

  virtual ~_CSICKRAWLASER (void);

  //
  //
  //

  virtual void GetScanContinuous (CRADIALSCAN &scan);

  virtual void GetScanContinuousSick (CSICKSCAN &scan) = 0x0;
};


#endif
