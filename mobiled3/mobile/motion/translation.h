//
//
//

#ifndef _GENTRANSLATION_H_
#define _GENTRANSLATION_H_

#include "iotypes.h"
#include "./motion.h"
#include "./filter.h"
#include "../controller/picontroller.h"


/**
 * @ingroup Motion
 * @brief Diese Bewegung fährt den Roboter geradeaus.
 *
 * @section author Author
 *   Torsten Scherer,
 *   Daniel Westhoff (westhoff@informatik.uni-hamburg.de)
 * @section date Copyright
 *   (c) 2000-2006
 */
class CTRANSLATION : protected _CMOTION, _CFILTER
{
private:

  /** NOT IMPLEMENTED */
  CTRANSLATION (void);

  /** NOT IMPLEMENTED */
  CTRANSLATION (const CTRANSLATION &);

  /** NOT IMPLEMENTED */
  CTRANSLATION &operator= (const CTRANSLATION &);

  /** NOT IMPLEMENTED */
  bool operator== (const CTRANSLATION &);

protected:

  PIController _headController;

  bool _running;

  double _vMax;

  double _oldAngle1;
  double _oldAngle2;
  double _oldAngle3;
  double _oldAngle4;

  CVEC _x0;
  CVEC _xe;
  CVEC _dDir;
  double _dVal;

  double _te;
  virtual double ComputeProfile (void);
  virtual bool IntegrateProfile (void);

  //

  //double _lastTransVel;
  bool _profileDone;
  int _fadeOut;

public:

  /** Konstruktor. */
  CTRANSLATION (const double vTransMax,
		const double aTransMax,
		const double interval,
		const double xe,
		const double ye);

  /** Destruktor. */
  virtual ~CTRANSLATION (void);

  /** _CMOTION */
  virtual bool Step (const CVEC pose,
		     const CVEC velocities,
		     double &transVel,
		     double &rotVel);
};

#endif
