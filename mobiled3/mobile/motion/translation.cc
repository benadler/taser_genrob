//
//
//


// Die folgenden drei Zeilen kommentieren, wenn Debugging-
// Informationen ausgegeben werden sollen.
//#ifdef DEBUG
//#undef DEBUG
//#endif

#include <math.h>
#include "thread/tracelog.h"
#include "./translation.h"

#define MATH_PARANOIA
#undef MATH_PARANOIA_CURE

#define PROFILE_STUCK_CURE
 

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CTRANSLATION::CTRANSLATION (const double vTransMax,
			    const double aTransMax,
			    const double interval,
			    const double xe,
			    const double ye)
  : _CMOTION (MOTION_TRANSLATE),
    _headController (1.2,    // kr
		     50.0)   // tr
{
  GTL ("CTRANSLATION::CTRANSLATION()");

  //

  _running = false;

  //

  SetParameters (interval, aTransMax, vTransMax);

  _vMax = vTransMax;

  //

  _xe.Create (xe, ye);

  //

  _profileDone = false;
  _fadeOut = 0;
  //_lastTransVel = 0.0;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CTRANSLATION::~CTRANSLATION (void)
{
  GTL ("CTRANSLATION::~CTRANSLATION()");

  // nichts zu tun
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

double CTRANSLATION::ComputeProfile (void)
{
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

bool CTRANSLATION::IntegrateProfile (void)
{
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

bool CTRANSLATION::Step (const CVEC pose,
			 const CVEC velocities,
			 double &transVel,
			 double &rotVel)
{
  GTL ("CTRANSLATION::Step()");

  static double lastTransVelOutput, lastRotVelOutput; 
  
  transVel = lastTransVelOutput;
  rotVel = lastRotVelOutput;

  // ++westhoff20040728: Stop motion if requested
  // ??? Don't know what i am doing here, but seems to work ???

  if (_abort)
    {
       _profileDone = true;

       return true;
    }

  //

  GTLPRINT (("p0[m,°]=(%.3f,%.3f,%.3f), v0[m/s]=(%.3f,%.3f), pe[m]=(%.3f,%.3f)\n",
	     pose[0], pose[1], pose[2] * 180.0 / M_PI,
	     velocities[0], velocities[1],
	     _xe[0], _xe[1]));

  //
  // settings for the first cycle
  //

  if (!_running)
    {
      _running = true;

      _oldAngle1 = _oldAngle2 = _oldAngle3 = _oldAngle4 = 0.0;

      _x0.Create (pose[0], pose[1]);

      GTLPRINT (("going from (%.3f,%.3f) to (%.3f,%.3f)\n",
		 _x0[0], _x0[1],
		 _xe[0], _xe[1]));

      // compute trajectory direction

      _dDir = _xe - _x0;
      _dVal = _dDir.Norm ();

      if (_dVal < 0.01)   // never less than 1cm
	{
	  GTLFATAL (("null motion requested\n"));

#warning "++itschere20020513: fixme!!!"

	  //abort ();
	}

      _dDir /= _dVal;

      GTLPRINT (("trajectory direction = (%.3f,%.3f)\n",
		 _dDir[0], _dDir[1]));

      // compute initial velocity along trajectory (assumed to be zero)

      CVEC v0 (velocities[0], velocities[1]);

      transVel = v0 * _dDir;

      // set profile target

      CVEC xe (_dVal);

#if 1
      SetPositionAndVelocity (xe, xe);   // hack to get dimensions right
#endif
      SetTarget (xe);
    }

  //
  //
  //

  double myTransVel = 0.0, myRotVel = 0.0;

  //
  // compute current position on trajectory (assumed to be zero in the first
  // cycle)
  //

  CVEC p0 (pose[0], pose[1]);

  const double t0 = (p0 - _x0) * _dDir;

  GTLPRINT (("p0: %f/%f m @ %e m/s\n", t0, _dVal, transVel));

  //
  // now for the real stuff
  //

  if (!_profileDone)
    {
      // set profile start

      {
	CVEC d0 (t0);
	CVEC v0 (transVel);

	SetPositionAndVelocity (d0, v0);
      }

      //
      // TRANSLATION: compute and evaluate profile
      //

      ComputeProfile ();   // implicit `_te=0'

      _te += rate;

      _profileDone = IntegrateProfile ();

      const double t1 = worldX1[0];
      myTransVel = worldV1[0];

      // in case of overshooting, do *never* go backwards but terminate motion
      // immediately instead. small errors in x/y positioning are much more
      // tolerable than errors in orientation...

      if ((myTransVel < 0.0) || (t1 > _dVal))
	{
	  myTransVel = 0.0;
	  _profileDone = true;
	}

      GTLPRINT (("p1: %f/%f m (%f%%) @ %e m/s %s\n",
		 t1, _dVal,
		 100.0 * t1 / _dVal,
		 myTransVel,
		 _profileDone ? "-- profile done" : ""));

#ifdef PROFILE_STUCK_CURE

      //
      // ++itschere20030731: try to determine if we're stuck close to the end.
      //
      // this may happen because the profile velocity may become so small that
      // the motors won't move any more. since the profile is in the
      // decelerating phase it probably won't computer a sigificantly higher
      // value in the next cycle, so we're in an endless loop.
      //
      // the idea is to test for three conditions:
      //
      // 1. are we decelerating? this is done by looking at the last
      //    `_transVel' compared to what this cycle says. we have to do this
      //    because the motor velocity may also remain zero in the first
      //    cycle(s) of the acceleration phase.
      //
      // 2. have the velocities been changed (because of collision avoidance)?
      //    in this case we're obviously decelerating, but we don't want to
      //    count this as coming to stop at the target.
      //
      // 3. has the motor been stopped? this finally marks the last phase of
      //    decelerating. if we're decelerating AND we're not doing so because
      //    of collision avoidance AND we're already so slow that the motor
      //    velocities have become zero THEN we should probably better stop.
      //
      // this work's just fine at first sight, but does it really cover all
      // possible situations?
      //

      bool decelerating = false;

      if (!_profileDone)
	{
	  if (myTransVel < transVel)
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

#endif

      //
      //
      //

      if (!_profileDone)
	{
	  //
	  // ROTATION:
	  //

#if 1

	  // compute the angle between heading direction and
	  // trajectory. minimizing this angle only aligns the robot to the
	  // trajectory but does not (yet) move it back onto it.

	  GTLPRINT (("dDir   = (%f,%f) norm = %e\n",
		     _dDir[0], _dDir[1],
		     _dDir.Norm ()));

	  CVEC head (cos (pose[2]), sin (pose[2]));   // where we're heading to

	  GTLPRINT (("head   = (%f,%f) norm = %e\n",
		     head[0], head[1],
		     head.Norm ()));

	  double angleCos = _dDir * head;

#if 1

	  //
	  // ++itschere20030802: that's weird - suppose two numbers a and b as
	  //
	  // a = 1.000000006454241451 and
	  // b = 1.000000000000000000,
	  //
	  // then (a>b) is false, but ((a-1.0)>(b-1.0)) is true. that applies
	  // to one pentium-iii (coppermine) and one pentium-m (centrino) i've
	  // seen, but not to two pention-iv (northwood). i didn't know todays
	  // processors are that different in their floating point unit... :-(
	  //
	  // or is this a side effect of the compiler that converts everything
	  // from `double' down to `float' before comparing? pretty much the
	  // same code did run on the mentioned pentium-iii on an older version
	  // of gcc i sort of remember.
	  //

	  if (angleCos - 1.0 > 0.0)
	    {
	      angleCos = 1.0;
	    }
	  else if (angleCos + 1.0 < 0.0)
	    {
	      angleCos = -1.0;
	    }

#else

	  if (angleCos < -1.0)
	    {
	      angleCos = -1.0;
	    }

	  if (angleCos > 1.0)
	    {
	      angleCos = 1.0;
	    }

#endif

	  GTLPRINT (("angleCos = %.20e\n", angleCos));

	  //

	  CVEC dDirPerp (-_dDir[1], _dDir[0]);   // right hand in 2d-plane

	  double angle1;
	  const double side = dDirPerp * head;

	  if (side >= 0.0)
	    {
	      angle1 = acos (angleCos);
	    }
	  else
	    {
	      angle1 = -acos (angleCos);
	    }

	  GTLPRINT (("angle1 = %e [rad], %e [°]\n",
		     angle1,
		     angle1 * 180.0 / M_PI));

#ifdef MATH_PARANOIA

	  if (isnan (angle1))
	    {
	      GTLFATAL (("is NAN! :-(\n"));

#ifdef MATH_PARANOIA_CURE

	      if (side >= 0.0)
		{
		  if (angleCos > 0.1)   // any threshold a bit less than 1.0
		    {
		      angle1 = 0.0;
		    }
		  if (angleCos < -0.1)
		    {
		      angle1 = M_PI;
		    }
		}
	      else
		{
		  if (angleCos > 0.1)
		    {
		      angle1 = 0.0;
		    }
		  if (angleCos < -0.1)
		    {
		      angle1 = -M_PI;
		    }
		}
#else
	      abort ();
#endif
	    }

#endif

	  GTLPRINT (("angle1 = %e [rad], %e [°]\n",
		     angle1,
		     angle1 * 180.0 / M_PI));

	  //

	  CVEC d0 = _x0 + t0 * _dDir;
	  //CVEC d1 = _x0 + t1 * _dDir;
	  CVEC d1 = _x0 + (t0 + 0.5) * _dDir;   //

	  CVEC path = d1 - p0;

	  double angle2 = 0.0;

	  const double pathNorm = path.Norm ();

	  if (pathNorm > 0.001)
	    {
	      path /= pathNorm;

	      GTLPRINT (("path   = (%f,%f), norm = %f\n", path[0], path[1], pathNorm));

	      angleCos = path * head;

	      GTLPRINT (("angleCos2 = %e (-1:%e) (+1:%e)\n",
			 angleCos,
			 angleCos - 1.0,
			 angleCos + 1.0));

#if 1

	      // see above

	      if (angleCos - 1.0 > 0.0)
		{
		  angleCos = 1.0;
		}
	      else if (angleCos + 1.0 < 0.0)
		{
		  angleCos = -1.0;
		}

#else

	      if (angleCos < -1.0)
		{
		  angleCos = -1.0;
		}
	      else if (angleCos > 1.0)
		{
		  angleCos = 1.0;
		}

#endif

	      GTLPRINT (("angleCos2 = %e (-1:%e) (+1:%e)\n",
			 angleCos,
			 angleCos - 1.0,
			 angleCos + 1.0));

	      CVEC pathPerp (-path[1], path[0]);   // right hand in 2d-plane

	      const double side = pathPerp * head;

	      if (side >= 0.0)
		{
		  angle2 = acos (angleCos);
		}
	      else
		{
		  angle2 = -acos (angleCos);
		}
	    }

	  GTLPRINT (("d0     = (%f,%f)\n", d0[0], d0[1]));
	  GTLPRINT (("p0     = (%f,%f)\n", p0[0], p0[1]));
	  GTLPRINT (("d1     = (%f,%f)\n", d1[0], d1[1]));

	  GTLPRINT (("angle2 = %e [rad], %e [°]\n",
		     angle2,
		     angle2 * 180.0 / M_PI));

#ifdef MATH_PARANOIA

	  if (isnan (angle2))
	    {
	      GTLFATAL (("is NAN! :-(\n"));

#ifdef MATH_PARANOIA_CURE

	      if (side >= 0.0)
		{
		  if (angleCos > 0.1)   // any threshold a bit less than 1.0
		    {
		      angle2 = 0.0;
		    }
		  if (angleCos < -0.1)
		    {
		      angle2 = M_PI;
		    }
		}
	      else
		{
		  if (angleCos > 0.1)
		    {
		      angle2 = 0.0;
		    }
		  if (angleCos < -0.1)
		    {
		      angle2 = -M_PI;
		    }
		}
#else
	      abort ();
#endif
	    }

#endif

	  GTLPRINT (("angle2 = %e [rad], %e [°]\n",
		     angle2,
		     angle2 * 180.0 / M_PI));

	  ////////////////////////////////////////////////////////////

	  // const double trajErr = fabs ((p0 - _x0) * dDirPerp);

	  // const double alpha = 1.0 - exp (-trajErr * 20.0);   // weight

	  // double angle = alpha * angle2 + (1.0 - alpha) * angle1;
	  // angle *= myTransVel / _vMax;   // scaled by translation velocity

	  // GTLPRINT (("error  = %.4f\n", trajErr));
	  // GTLPRINT (("weight = %.4f\n", alpha));
	  // GTLPRINT (("angle  = %.3f°\n", angle * 180.0 / M_PI));

	  // myRotVel = 2.0 * -angle;   // compensate against this angle

	  // GTLPRINT (("rotVel = %f\n", myRotVel));

#endif

	  /////////////////////////////////////////////////////////////////////

	  // myRotVel = 5.0 * -angle2;   // guter Wert :)

	  angle2 *= myTransVel / _vMax;

          double a = (angle2 + _oldAngle1 + _oldAngle2 + _oldAngle3 + _oldAngle4) / 5.0;

          myRotVel = _headController.step (0.0, a);

	  _oldAngle4 = _oldAngle3;
	  _oldAngle3 = _oldAngle2;
	  _oldAngle2 = _oldAngle1;
          _oldAngle1 = angle2;

	  //myRotVel = _headController.step (0.0,angle);

	}
    }

  //
  //
  //

  else   // _profileDone==true
    {
      _fadeOut++;
    }

  //
  //
  //

  transVel = myTransVel;
  rotVel = myRotVel;
  _velocitiesChanged = false;

  _motorStopped = false;

  //

  if (_fadeOut >= 32)
    {
      return true;
    }

  lastTransVelOutput = transVel;
  lastRotVelOutput = rotVel;

  return false;
}
