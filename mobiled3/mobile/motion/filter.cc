//
// filter.cc, based on filt.cc
// (C) 04/2002 TeSche,
// <itschere@techfak.uni-bielefeld.de>
//

#include <math.h>

#include "./filter.h"

#ifdef DEBUG
#undef DEBUG
#endif
#include "thread/tracelog.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static inline double fsgn (const double arg)
{
  if (arg < 0.0)
    return -1.0;

  return 1.0;
}


static inline double fsqr (const double arg)
{
  return arg * arg;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

_CFILTER::~_CFILTER (void)
{
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

double _CFILTER::computeProfile (const CVEC &x0In,
				const CVEC &v0In,
				const CVEC &xeIn,
				double a,
				const double vm,
				CVEC &tsInOut,
				CVEC &vpOut) const
{
  GTL ("_CFILTER::computeProfile()");

  // this is just to get the dimensionality of `vpOut' right, because later on
  // we will just set single values with operator[].

  vpOut = x0In;

  double tsMax = 0.0;

  for (int idx=0; idx<x0In.GetSize(); idx++)
    {
      double d = xeIn[idx] - x0In[idx];
      double v0 = v0In[idx];
      double t0, d1, vp = 0.0, ts = 0.0, tm = tsInOut[idx];

      // it's not legal to just check for the distance here, because we may be
      // exactly at the target, but with a non-zero velocity. what this check
      // essentially does, is to ensure the target can be reached in just one
      // cycle and explicitly tells the other parts so by setting `ts=0'.

      if ((fabs (d) < 0.000001) && (fabs (v0) < 0.000001))
	{
	  // since integration should take care about explicitly placing the
	  // coordinate at its target it's legal to expect real zero here. but
	  // still we're even more carefully...
	  vpOut[idx] = 0.0;
	  tsInOut[idx] = 0.0;
	  continue;
	}

      bool directionNegative = false;

      if (d < 0.0)
	{
	  directionNegative = true;
	  d = -d;
	  v0 = -v0;

	  GTLPRINT (("d=%f, v0=%f (flipped so that `d>0')\n", d, v0));
	}
      else
	{
	  GTLPRINT (("d=%f, v0=%f\n", d, v0));
	}

#if 1
      bool doDefault = true;
#endif

      if (v0 < 0.0)
	{
#if 1
	  doDefault = false;
#endif

	  // this case is easy: we're moving away from the target, so bring the
	  // coordinate to rest as fast as possible and then go to target with
	  // a ramp-up profile with v0 = 0.

	  d1 = (t0 = -v0 / a) * -v0 / 2.0;

	  GTLPRINT (("moving away: distance = %f + %f\n", d, d1));

	  // since we're moving away the distance increases
	  d += d1;

	  if ((vp = sqrt (a * d)) > vm)
	    {
	      vp = vm;
	    }

	  ts = d / vp + vp / a + t0;   // plus the deceleration time
	}

      /////////////////////////////////////////////////////////////////////////

      else if (d < (d1 = (t0 = v0 / a) * v0 / 2.0))
	{
	  GTLPRINT (("overshooting, distance = %f < %f\n", d, d1));
#if 1
	  // d = (v0 / a) * v0 / 2.0
	  // 2.0 * d / v0 = v0 / a
	  // a * 2.0 * d / v0 = v0
	  // a = v0 / (2.0 * d / v0)
	  // a = v0^2 / (2.0 * d)

#if 1
	  const double aNeeded = fsqr (v0) / (2.0 * d);

	  GTLPRINT (("continuing with %f times the deceleration\n",
		     aNeeded / a));

	  a = aNeeded;
#endif

	  if (0)
	    {
#endif
	      doDefault = false;

	      // this case is also easy: we're moving too fast and will
	      // overshoot, so just bring the coordinate to rest as fast as
	      // possible and then go back to target with a ramp-up profile to
	      // some negative vp with v0 = 0.

	      d = d1 - d;   // keep distance positive

	      if ((vp = sqrt (a * d)) > vm)
		{
		  vp = vm;
		}

	      ts = d / vp + vp / a + t0;   // plus the deceleration time

	      if (ts < tm)
		{
		  vp =
		    0.5 * (a*(tm-t0) - sqrt (fsqr(a)*fsqr(tm-t0) - 4.0*a*d));
		  ts = tm;
		}

	      vp = -vp;   // peak velocity is negative

#if 1
	    }
#endif
	}

      /////////////////////////////////////////////////////////////////////////

      if (doDefault)
	{
	  // default (difficult) case: we're moving towards the target and will
	  // not overshoot, but there're two different profile types
	  // possible. these types require different computations, so this case
	  // is a bit longer.

	  if ((vp = sqrt (a * d + v0 * v0 / 2.0)) > vm)
	    vp = vm;

	  if (v0 <= vp)
	    ts = d / vp + (vp - v0) / a + v0 * v0 / (2.0 * a * vp);
	  else
	    ts = d / vp - v0 * v0 / (2.0 * a * vp) + v0 / a;

	  if (ts < tm)
	    {
	      if (v0 <= vp)
		{
		  // difficult case: we may drop from 'v0 < vp' to `v0 > vp'
		  vp = 0.5 * (v0 + a*tm - sqrt (2.0*a*tm*v0 + a*a*tm*tm - 4.0*a*d - v0*v0));
		  if (vp < v0)
		    {
		      // we HAVE dropped from 'v0 < vp' to `v0 > vp'
		      vp = v0 * v0 / (2.0 * (v0 - a * tm)) - a * d / (v0 - a * tm);
		    }
		}
	      else
		{
		  // easy case: we'll stay at `v0 > vp'
		  vp = v0 * v0 / (2.0 * (v0 - a * tm)) - a * d / (v0 - a * tm);
		}
	      ts = tm;
	    }

	  if (v0 <= vp)
	    {
	      GTLPRINT (("normal `v0<=vp'\n"));
	    }
	  else
	    {
	      GTLPRINT (("normal `v0>vp'\n"));
	    }
	}

#if 1
      if (directionNegative)
	{
	  vp = -vp;
	}
#endif

      if (ts > tsMax)
	{
	  tsMax = ts;
	}

      vpOut[idx] = vp;
      tsInOut[idx] = ts;
    }

  return tsMax;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

bool _CFILTER::integrateProfile (const CVEC &x0In,
				 const CVEC &v0In,
				 const CVEC &xeIn,
				 const CVEC &tsIn,
				 const CVEC &vpIn,
				 const double a,
				 const double te,
				 CVEC &x1Out,
				 CVEC &v1Out) const
{
  GTL ("_CFILTER:integrateProfile()");

  bool isReady = true;

  // this is just to get the dimendionality of `x1Out' and `v1Out' right,
  // because later on we will just set single values with operator[].
  x1Out = x0In;
  v1Out = v0In;

  for (int idx=0; idx<x0In.GetSize(); idx++)
    {
      double v0 = v0In[idx];
      double ts = tsIn[idx];
      double vp = vpIn[idx];
      double dt, t1 = -1.0, t2 = -1.0, v1, d = 0.0;

      if (te >= ts)
	{
	  GTLPRINT (("dimension %i is ready\n", idx+1));

	  // either we'll do the complete profile in this cycle, or
	  // `computeProfiles' has set `ts=0' to indicate there's not really
	  // something to do. we do, however, compensate for rounding errors by
	  // explicitly going to the target.

	  x1Out[idx] = xeIn[idx];
	  v1Out[idx] = 0.0;

	  continue;
	}

      isReady = false;

      // phase #1
      if (v0 < vp)
	{
	  // phase #1 is ramp-up
	  if ((dt = te) > (t1 = fabs (vp - v0) / a))
	    dt = t1;

	  v1 = v0 + a * dt;
	  // ramp-up from arbitrary velocity to arbitrary velocity
	  d += v0 * dt + 0.5 * (v1 - v0) * dt;
	}
      else
	{
	  // phase #1 is ramp-down
	  if ((dt = te) > (t1 = fabs (v0 - vp) / a))
	    dt = t1;

	  v1 = v0 - a * dt;
	  // ramp-down from arbitrary velocity to arbitrary velocity
	  d += v0 * dt - 0.5 * (v0 - v1) * dt;
	}

      if (te > t1)
	{
	  // phase #2
	  t2 = ts - fabs (vp) / a;

	  if (te > t2)
	    dt = t2 - t1;
	  else
	    dt = te - t1;

	  v1 = vp;
	  d += vp * dt;

	  if (te > t2)
	    {
	      // phase #3. note that `te>=ts' is impossible here because of the
	      // extra check before even computing phase #1.

	      dt = te - t2;

	      if (vp < 0.0)
		{
		  // phase #3 is ramp-up
		  v1 = vp + a * dt;
		  d += vp * dt + 0.5 * (v1 - vp) * dt;
		}
	      else
		{
		  // phase #3 is ramp-down
		  v1 = vp - a * dt;
		  d += vp * dt - 0.5 * (vp - v1) * dt;
		}
	    }
	}

      x1Out[idx] = x0In[idx] + d;
      v1Out[idx] = v1;
    }

  return isReady;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void _CFILTER::SetParameters (const double newRate,
			     const double a,
			     const double vm)
{
  rate = newRate;
  worldA = a;
  worldVm = vm;
}


///////////////////////////////////////////////////////////////////////////////
//
// note that the velocity here is measured in 1/s and not in 1/cycle, so if you
// want to feed this a differential velocity you must divide it through the
// cycle rate in seconds.
//
///////////////////////////////////////////////////////////////////////////////

void _CFILTER::SetPositionAndVelocity (const CVEC &x0,
				      const CVEC &v0)
{
  GTL ("_CFILTER::SetPositionAndVelocity()");

  if (x0.GetSize () != v0.GetSize ())
    {
      GTLFATAL (("sizes don't match\n"));
      abort ();
    }

  if (x0.GetSize () > 6)
    {
      GTLFATAL (("vector space too big, must have dim<=6!\n"));
      abort ();
    }

  worldX0 = x0;
  worldV0 = v0;

  worldX1 = x0;
  worldV1 = v0;
}


///////////////////////////////////////////////////////////////////////////////
//
// it is silently assumed that you first call `SetPositionAndVelocity()' if
// needed and then `SetTarget()'. the check for matching vector sizes will only
// work this way round...
//
///////////////////////////////////////////////////////////////////////////////

void _CFILTER::SetTarget (const CVEC &xe)
{
  GTL ("_CFILTER::SetTarget()");

  if (worldX1.GetSize () != xe.GetSize ())
    {
      GTLFATAL (("sizes don't match!"));
      abort ();
    }

  worldXe = xe;
}


///////////////////////////////////////////////////////////////////////////////
//
// compute deviation from trajectory and/or target
//
///////////////////////////////////////////////////////////////////////////////

#define SMALL 1e-5

double _CFILTER::ComputeTrajectoryError (const CVEC &p)
{
  CVEC trajDir = worldTe - worldT0;
  double trajDirNorm;

  if ((trajDirNorm = trajDir.Norm ()) < SMALL)
    {
      // trajectory too short, consider target to be the nearest point
      return (worldTe - p).Norm ();
    }

  trajDir /= trajDirNorm;

  return (trajDir * ((p - worldT0) * trajDir) + worldT0 - p).Norm ();
}


double _CFILTER::ComputeTargetError (const CVEC &p)
{
  return (worldXe - p).Norm ();
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

double _CFILTER::StretchProfile (const double tMin)
{
  GTL ("CFILTER::StretchProfile()");

  return tMin;
}
