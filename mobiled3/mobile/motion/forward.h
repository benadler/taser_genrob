//
// forward.c
// (C) 01/2003 by Torsten Scherer (TeSche)
// <itschere@techfak.uni-bielefeld.de>
//

#ifndef _FORWARD_H_
#define _FORWARD_H_

#include "iotypes.h"
#include "./motion.h"
#include "./filter.h"

/**
 * @ingroup Motion
 * @brief Diese Bewegung fährt den Roboter <b>ohne Kollisionsvermeidung</b> geradeaus.
 *
 * @section author Author
 *   Torsten Scherer,
 *   Daniel Westhoff (westhoff@informatik.uni-hamburg.de)
 * @section date Copyright
 *   (c) 2000-2006
 */
class CFORWARD : protected _CMOTION, _CFILTER
{
private:

  /** NOT IMPLEMENTED */
  CFORWARD (void);

  /** NOT IMPLEMENTED */
  CFORWARD (const CFORWARD &);

  /** NOT IMPLEMENTED */
  CFORWARD &operator= (const CFORWARD &);

  /** NOT IMPLEMENTED */
  bool operator== (const CFORWARD &);

protected:

  bool _running;
  bool _backwards;

  double _vMax;

  CVEC _x0;
  CVEC _dir0;
  double _de;

  double _te;
  virtual double ComputeProfile (void);
  virtual bool IntegrateProfile (void);

  //

  bool _profileDone;
  double _tStarted;
  double _tProfileDone;

public:

  /** Konstruktor. */
  CFORWARD (const double vTransMax,
	    const double aTransMax,
	    const double interval,
	    const double d);

  /** Destructor */
  virtual ~CFORWARD (void);

  /** _CMOTION */
  virtual bool Step(const CVEC pose, 
		    const CVEC velocities,
		    double &transVel, 
		    double &rotVel);
};

#endif
