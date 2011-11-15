//
//
//

#ifndef _ROTATION_H_
#define _ROTATION_H_

#include "iotypes.h"
#include "./motion.h"
#include "./filter.h"
#include "../controller/controller.h"
#include "../localization/localization.h"

/**
 * @ingroup Motion
 * @brief Diese Bewegung dreht den Roboter um sein Zentrum.
 *
 * @section author Author
 *   Torsten Scherer,
 *   Daniel Westhoff (westhoff@informatik.uni-hamburg.de)
 * @section date Copyright
 *   (c) 2000-2006
 */
class CROTATION : protected _CMOTION, _CFILTER
{
private:

  /** NOT IMPLEMENTED */
  CROTATION (void);

  /** NOT IMPLEMENTED */
  CROTATION (const CROTATION &);

  /** NOT IMPLEMENTED */
  CROTATION &operator= (const CROTATION &);

  /** NOT IMPLEMENTED */
  bool operator== (const CROTATION &);

public:

  bool _collisionAvoidance;

protected:

  // if in angle mode, use `_ae', else use `_xe'/`_ye'

  bool _angleMode;

  double _ae;
  double _xHead, _yHead;

  double _te;

  bool _running;
  bool _profileDone;
  bool _controllerDone;

  bool _fineTune;
  //  double _a1;
  int _fadeOut;

  virtual double ComputeProfile (void);
  virtual bool IntegrateProfile (void);

  PIController _pathController;
  PIController _targetController;

  double _targErrLast;
  double _targErrSum;
  double _targGain;

  CVEC _p0;

  /** 
   * Gibt eine Orientirung aus die weniger als PI von der Zielorientierung entfernt ist.
   *
   * @param[in] clippedOrientation Orientierung des Roboters zwischen -PI..+PI.
   * @param[in] targetOrientation Orientierung zu der sich der Roboter drehen soll.
   *
   * @return Orientierung des Roboters, so dass der Winkel zur Zielorientierung kleiner PI ist.
   */
  float GetOrientation (const float &clippedOrientation, const float &targetOrientation);

public:

  /** Konstruktor für Drehung in die angegebene Orientierung. */
  CROTATION (const double vRotMax,
	     const double aRotMax,
	     const double interval,
	     const double ae,
	     const bool fineTune = true,
	     const bool collisionAvoidance = true);

  /** Destruktor. */
  virtual ~CROTATION (void);

  /** _CMOTION */
  virtual bool Step (const CVEC pose, 
		     const CVEC velocities,
		     double &transVel,
		     double &rotVel);
};

#endif
