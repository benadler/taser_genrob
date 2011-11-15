//
//
//

// Die folgenden drei Zeilen kommentieren, wenn Debugging-
// Informationen ausgegeben werden sollen.
//#ifdef DEBUG
//#undef DEBUG
//#endif

#include <iostream>
#include <math.h>
#include "thread/tracelog.h"
#include "util/fmap.h"
#include "./rotation.h"

#define PROFILE_STUCK_CURE

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static inline double fsqr (const double arg)
{
  return arg * arg;
}


static inline double fsgn (const double arg)
{
  if (arg < 0.0)
    {
      return -1.0;
    }

  return 1.0;
}

static inline double clipAngle (radiant_t angle)
{
  // Winkel im Intervall [-PI..PI] halten
  if (angle >= M_PI) 
    {
      angle -= (2 * M_PI);
    }
  else if (angle < (-1.0 * M_PI))
    {
      angle += (2 * M_PI);
    }

  return angle;
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CROTATION::CROTATION (const double vRotMax,
		      const double aRotMax,
		      const double interval,
		      const double ae,
		      const bool fineTune,
		      const bool collisionAvoidance) // ++westhoff: enable collisionAvoidence
  : _CMOTION (MOTION_ROTATEANGLE),
    _pathController (0.3, 0.3),
    _targetController (2.0, 0.5)
{
  GTL ("CROTATION::CROTATION()");

  //

  SetParameters (interval, aRotMax, vRotMax);

  //

  _angleMode = true;

  _ae = ae;

  _fineTune = fineTune;

  // ++westhoff20060424
  if (!collisionAvoidance) 
    {
      GTLFATAL (("Keine Kollisionsvermeidung!\n"));
    }

  _collisionAvoidance = collisionAvoidance;

  GTLPRINT (("this = %p : _collisionAvoidance = %i\n", this, _collisionAvoidance));

  _fadeOut = 0;
  _running = false;
  _profileDone = false;
  _controllerDone = true;

  //

  _pathController.setPFade (1.0);
  _pathController.setIFade (0.2);

  _targetController.setPFade (1.0);
  _targetController.setIFade (1.0);

  _targErrSum = 0.0;
  _targErrLast = 0.0;
  _targGain = 1.0;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CROTATION::~CROTATION (void)
{
  GTL ("CROTATION::~CROTATION()");

  // nichts zu tun
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

double CROTATION::ComputeProfile (void)
{
  GTL ("CROTATION::ComputeProfile()");

  projTs.Create (1);

  _te = 0.0;

  return computeProfile (worldX0,
			 worldV0,
			 worldXe,
			 worldA,
			 worldVm,
			 projTs,
			 projVp);
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

bool CROTATION::IntegrateProfile (void)
{
  GTL ("CROTATION::IntegrateProfile()");

  return integrateProfile (worldX0,
			   worldV0,
			   worldXe,
			   projTs,
			   projVp,
			   worldA,
			   _te,
			   worldX1,
			   worldV1);
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

float CROTATION::GetOrientation (const float &clippedOrientation, 
				 const float &targetOrientation)
{
  float orientation = clippedOrientation;

  // Gegebenenfalls Winkel nahe aneinander bringen
  if (fabs (targetOrientation - clippedOrientation) >= M_PI)
    {
      if ((targetOrientation > 0.0) && (clippedOrientation < 0.0))
	{
	  orientation = clippedOrientation + 2*M_PI;
	}
      else if ((targetOrientation < 0.0) && (clippedOrientation > 0.0))
	{
	  orientation = clippedOrientation - 2*M_PI;
	}
    }
  
  return orientation;
}


///////////////////////////////////////////////////////////////////////////////
//
// this is an experimental version which uses the last output velocity, but the
// real current position to compute the profile in each cycle, thus really
// making it dynamic. i sort of remember that with the old hardware this didn't
// work because the profile would then overshoot because it could not
// decelerate the platform in time, but maybe this has improved with the new
// hardware...
//
///////////////////////////////////////////////////////////////////////////////

bool CROTATION::Step (const CVEC pose,
		      const CVEC velocities,
		      double &transVel,
		      double &rotVel)
{
  GTL ("CROTATION::Step()");

  static double lastTransVelOutput, lastRotVelOutput; 
  
  transVel = lastTransVelOutput;
  rotVel = lastRotVelOutput;

  if (_abort)
  {
     _profileDone = true;

     return true;
  }

  if (!_angleMode)
    {
      GTLFATAL (("point heading mode not (yet) supported!\n"));
      abort ();
    }

  GTLPRINT (("Orientierung/Ziel: %.3f/%.3f [°], Rotationsgeschwindigkeit/Sollgeschwindigkeit: %.3f/%.3f [°/s]\n",
	     180.0 * pose[2] / M_PI,
	     180.0 * _ae / M_PI,
	     180.0 * velocities[2] / M_PI,
	     180.0 * rotVel / M_PI));

  //
  // special adjustments in the first cycle
  //

  if (!_running)
    {
      GTLPRINT (("Anpassungen im ersten Zyklus:\n"));

      _ae = fmap (_ae - pose[2] + M_PI, 2.0 * M_PI) + pose[2] - M_PI;

      GTLPRINT (("Orientierung/Ziel: %.3f/%.3f [°] (ANGEPASST), Rotationsgeschwindigkeit/Sollgeschwindigkeit: %.3f/%.3f [°/s]\n",
		 180.0 * pose[2] / M_PI,
		 180.0 * _ae / M_PI,
		 180.0 * velocities[2] / M_PI,
		 180.0 * rotVel / M_PI));

      _p0 = pose;
    }

  //
  // the profile
  //

  double a1 = 0.0;
  double va1 = 0.0;

  if (!_profileDone)
    {
      GTLPRINT (("PROFIL: Sollgeschwindigkeit anhand des Geschwindigkeitsprofils bestimmen\n"));

      // initialize profile parameters

      if (!_running)
	{
	  GTLPRINT (("PROFIL initialisieren.\n"));

	  _running = true;

	  CVEC x0v (pose[2]);
	  CVEC v0v (velocities[2]);
	  SetPositionAndVelocity (x0v, v0v);

	  CVEC xev (_ae);
	  SetTarget (xev);
	}
      else
	{
	  //
	  // ++itschere20030124: with the old platform we had to use a
	  // basically static profile because if we really computed it in each
	  // cycle based on the real state it would a) not ramp up to full
	  // velocity and b) overshoot. the first problem is because the
	  // initial output velocity of the profile would not be enough to make
	  // the motors start running, and thus get stuck. the second problem
	  // is because the profile could not decelerate the platform in
	  // time. the way to go was to use the output data from the last cycle
	  // (only modified by collision avoidance) and not the real position,
	  // plus a pi-controller to compensate for the remaining error the
	  // profile does not see.
	  //
	  // but the new hardware can do better. the motor controllers react
	  // much quicker and better to the profile output and we can therefore
	  // use the real position. we still use the last output velocity to be
	  // sure it ramps up to full velocity, but this does not hurt that
	  // much. a little overshoot remains, but during normal operation you
	  // can barely see it.
	  //

	  float orientation = GetOrientation (pose[2], _ae);
      
	  CVEC x0v (orientation);
	  CVEC v0v (rotVel);

	  SetPositionAndVelocity (x0v, v0v);

	  GTLPRINT (("Orientierung: %f [°],  Rotationsgeschwindigkeit: %f [°/s]\n", 
		     180.0 * orientation / M_PI, 
		     180.0 * rotVel / M_PI));
	}

      // compute profile

      double tMax = ComputeProfile ();   // implicit _te=0.0

      GTLPRINT (("Verbleibende Zeit für das Profil: %.3f [s]\n", tMax));

      // compute next profile step

      _te += rate;   // effective _te=rate

      if ((_profileDone = IntegrateProfile ()))
	{
	  GTLPRINT (("PROFIL fertig.\n"));

	  //
	  // now that the profile is more dynamic and reaches the target
	  // position much better we can make this conditional on whether we
	  // want fine tuning or not. the idea is that rotation between
	  // straight line segments of a complex path need not be 100%
	  // accurate, 99% will do nicely and the behavious looks much
	  // smoother.
	  //

	  if (_fineTune)
	    {
	      _controllerDone = false;
	      _fadeOut = 0;
	    }
	}

      a1 = worldX1[0];
      va1 = worldV1[0];

      GTLPRINT (("Orientierung nach Profil: %.3f [°], Sollgeschwindigkeit: %.3f [°/s]\n",
		 180.0 * a1 / M_PI,
		 180.0 * va1 / M_PI));
    }

  GTLPRINT (("Rotationsgeschwindigkeit letzter Zyklus: %f [rad/s]\n", rotVel));
  GTLPRINT (("Rotationsgeschwindigketi nach Profil   : %f [rad/s]\n", va1));

#ifdef PROFILE_STUCK_CURE

  //
  // ++itschere200307312: see comment in genTranslation.cc
  //

  bool decelerating = false;

  if (!_profileDone)
    {
	if (fabs (va1) < fabs (rotVel)) // ++westhoff20070620: '_rotVel' in 'rotVel' geändert
	{
	  decelerating = true;
	}
    }

  if (decelerating)
    {
      GTLPRINT (("decelerating **************************************\n"));
    }

  if (!_velocitiesChanged)
    {
      GTLPRINT (("velocities not changed ****************************\n"));
    }

  if (_motorStopped)
    {
      GTLPRINT (("motor stopped *************************************\n"));
    }

  if (decelerating && !_velocitiesChanged && _motorStopped)
    {
      GTLFATAL (("forced to stop\n"));
      _profileDone = true;
    }

#else

  //
  // the path controller
  //
  // must NOT be used when using the above trick to force the profile to
  // terminate when close to the end because it breaks the `decelerating'
  // check.
  //

  if (!_profileDone)
    {
      GTLPRINT (("using path controller\n"));

      const double ctrlOut = _pathController.step (a1, pose[2]);

      GTLPRINT (("%f %f -> %f\n", _ae, pose[2], ctrlOut));

      va1 += ctrlOut;
    }

#endif

  //
  // the target controller
  //

  if (_profileDone && !_controllerDone)
    {
      GTLPRINT (("PID-Regler für die Endpositionierung.\n"));

      float orientation = GetOrientation (pose[2], _ae);

      GTLPRINT (("PID-Regler: Orientierung/Ziel: %f/%f [°]\n",
		 180.0 * orientation / M_PI,
		 180.0 * _ae / M_PI));

      const double ctrlOut = _targetController.step (_ae, orientation);

      va1 += ctrlOut;

      GTLPRINT (("PID-Regler: Rotationsgeschwindigkeit: %f [rad/s]\n", va1));
    }

  //
  //
  //

  transVel = 0.0;

  rotVel = va1;

  _velocitiesChanged = false;

  _motorStopped = false;

  //
  // has the motion terminated?
  //

  if (!_controllerDone)
    {
      _fadeOut++;
    }

  double err = fabs (pose[2] - _ae);

  GTLPRINT (("Fehler zur Zielorientierung: %f\n", err));

  if ((err * 180.0 / M_PI) < 0.01) 
    {
      _profileDone = true;
    }

  if ((_fadeOut >= 100) || ((err < 0.15) && (_fadeOut > 50)))
    {
      _controllerDone = true;

      GTLPRINT (("PID-Regler fertig.\n"));
    }

  bool done = _profileDone && _controllerDone;

  //
  // special adjustments in the last cycle
  //

  if (done)
    {
      GTLPRINT (("Rotation fertig.\n"));
    }

  lastTransVelOutput = transVel;
  lastRotVelOutput = rotVel;

  return done;
}
