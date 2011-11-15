//
// genForward.c
// (C) 01/2003 by Torsten Scherer (TeSche)
// <itschere@techfak.uni-bielefeld.de>
//
// actually the name "forward" is misleading because it can also go
// backwards. the point is that this motion does NOT control the orientation by
// any means. it just goes forward (or backward). it also uses no pi-controller
// for fine tuning, just the profile. therefore you may not want to drive large
// distances with this mode - you'd get off your path. it's just meant for
// small (and slow?) motions.
//

#include <math.h>
#include <sys/time.h>

//#ifdef DEBUG
//#undef DEBUG
//#endif

#include "thread/tracelog.h"

#include "./forward.h"


CFORWARD::CFORWARD (const double vTransMax,
		    const double aTransMax,
		    const double interval,
		    const double d)
  : _CMOTION (MOTION_FORWARD)
{
  GTL ("CFORWARD::CFORWARD()");

  //

  _running = false;

  //

  SetParameters (interval, aTransMax, vTransMax);

  _vMax = vTransMax;

  //

  if ((_backwards = (d < 0.0)))
    {
      _de = -d;
    }
  else
    {
      _de = d;
    }

  //

  _profileDone = false;
  _tStarted = -1.0;
  _tProfileDone = -1.0;

  //

#ifdef USE_LOGGING

  time_t t = time (0);
  struct tm tm;
  localtime_r (&t, &tm);

  char fName[64];
  sprintf (fName,
	   "forward-%04i%02i%02i%02i%02i%02i.log",
	   tm.tm_year + 1900,
	   tm.tm_mon + 1,
	   tm.tm_mday,
	   tm.tm_hour,
	   tm.tm_min,
	   tm.tm_sec);

  if (!(_fp = fopen (fName, "w")))
    {
      GTLFATAL (("failed to create logging file\n"));
    }

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CFORWARD::~CFORWARD (void)
{
  GTL ("CFORWARD::~CFORWARD()");

#ifdef USE_LOGGING

  if (_fp)
    {
      fclose (_fp);
      _fp = 0x0;   // paranoia
    }

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

double CFORWARD::ComputeProfile (void)
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

bool CFORWARD::IntegrateProfile (void)
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

bool CFORWARD::Step (const CVEC pose, 
		     const CVEC velocities, 
		     double &transVel, 
		     double &rotVel)
{
  GTL ("CFORWARD::Step()");

  //

  GTLPRINT (("p0[m,m,°]=(%.3f,%.3f,%.3f), v0[m/s]=(%.3f,%.3f), de[m]=(%.3f)\n",
	     pose[0], pose[1], pose[2] * 180.0 / M_PI,
	     velocities[0], velocities[1],
	     _de));

  static double lastTransVel = 0.0;

  double myTransVel = 0.0;

  //
  //
  //

  if (!_profileDone)
    {
      //
      // initialization in the first cycle
      //

      if (!_running)
	{
	  _running = true;

	  _x0.Create (pose[0], pose[1]);

	  if (_backwards)
	    {
	      _dir0.Create (-cos (pose[2]), -sin (pose[2]));
	    }
	  else
	    {
	      _dir0.Create (cos (pose[2]), sin (pose[2]));
	    }

	  GTLPRINT (("going %s from (%.3f,%.3f) for %.3f along (%.3f,%.3f)\n",
		     _backwards ? "backward" : "forward",
		     _x0[0], _x0[1],
		     _de,
		     _dir0[0], _dir0[1]));

	  if (fabs (_de) < 0.01)
	    {
	      GTLFATAL (("null motion requested (distance < 0.01m)\n"));

#warning "++itschere20030128: fixme!!!"

	      _profileDone = true;
	      //abort ();
	    }

	  // compute initial velocity along trajectory (assumed to be zero)

	  CVEC v0 (velocities[0], velocities[1]);

	  transVel = v0 * _dir0;

	  lastTransVel = transVel;

	  // set profile target

	  CVEC xe (_de);

	  SetPositionAndVelocity (xe, xe);   // hack to get dimensions right

	  SetTarget (xe);

	  //
	}

      //
      // compute current position on trajectory (assumed to be zero in the
      // first cycle)
      //

      CVEC p0 (pose[0], pose[1]);

      double t0 = (p0 - _x0) * _dir0;

      GTLPRINT (("being at %.6f of %.3f\n", t0, _de));

      // set profile start

      {
	CVEC d0 (t0);
	CVEC v0 (_backwards ? -lastTransVel : lastTransVel);

	SetPositionAndVelocity (d0, v0);
      }

      //
      // compute profile
      //

      ComputeProfile ();   // implicit `_te=0'

      struct timeval tv;
      gettimeofday (&tv, 0x0);

      if (_tStarted < 0.0)
	{
	  _tStarted = tv.tv_sec + tv.tv_usec / 1000000.0;
	  _tProfileDone = projTs[0];
	}

      const double tElapsed = (tv.tv_sec + tv.tv_usec / 1000000.0) - _tStarted;

      //
      // evaluate profile
      //

      _te += rate;

      _profileDone = IntegrateProfile ();

      const double t1 = worldX1[0];
      myTransVel = worldV1[0];

      // in case of overshooting, do *never* go backwards but terminate motion
      // immediately instead. errors in x/y positioning are much more tolerable
      // than errors in orientation...

      if (t1 >= _de)
	{
	  myTransVel = 0.0;
	  _profileDone = true;
	}

      // ++westhoff20040728: Stop motion if requested

      if (_abort)
        {
          _profileDone = true;

          return true;
        }

      GTLPRINT (("going to %.6f %s\n",
		 t1,
		 _profileDone ? "(profile done)" : ""));

      //
      // go for at most 1s longer than the initial guess of how long the
      // profile would take was. the idea is that these small incremental
      // motions are not very accurate and therefore one might want them to go
      // a few (one?) cm beyond their intended target. this they will never
      // reach in case of the charger, because it "crashes" into it. this in
      // turns means that the motion would never stop because it isn't yet at
      // the target, leading to an indefinite motion. we break this loop by
      // aborting 1s after the initially computed time elapses, whether the
      // target is reached or not.
      //

      if (tElapsed > _tProfileDone + 1.0)
	{
	  GTLPRINT (("Zeit die vergangen ist eine Sekunde länger als die vorgesehene Zeit für das Profil.\n")); 
	  GTLPRINT (("Beende Bewegung.\n"));	  
	  _profileDone = true;
	}
    }

  //
  //
  //

  transVel = _backwards ? -myTransVel : myTransVel;
  rotVel = 0.0;

  lastTransVel = transVel;

  //

  return _profileDone;
}
