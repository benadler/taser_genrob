//
//
//

#include <math.h>
#include <unistd.h>

#include "mobile/mobile.h"
#include "thread/tracelog.h"

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CMOBILE::ComputeError (const meter_t xe,
			   const meter_t ye,
			   const degree_t ae,
			   meter_t *xErr,
			   meter_t *yErr,
			   degree_t *aErr)
{
  GTL ("CMOBILE::ComputeError()");

  //

  double xs = 0.0, ys = 0.0, as = 0.0;

  for (int i=0; i<10; i++)
    {
      float x, y, a;

      if (GetPosition (x, y, a) < 0)
	{
	  GTLFATAL (("GetPosition() failed\n"));
	  abort ();
	}

      usleep (100000);   // 0.1s

      xs += x;
      ys += y;
      as += a;
    }

  xs /= 10.0;
  ys /= 10.0;
  as /= 10.0;

  //

  *xErr = xs - xe;
  *yErr = ys - ye;
  *aErr = as - ae;

  //
  // in case the platform should report a rotation angle out of the bounds -PI
  // .. PI adjust for the error.
  //

  while (*aErr < -180.0)
    {
      *aErr += 360.0;
    }

  while (*aErr > 180.0)
    {
      *aErr -= 360.0;
    }

  GTLPRINT (("error = (%.3f,%.3f,%.3f)\n", *xErr, *yErr, *aErr));

  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CMOBILE::MoveDirect (const meter_t xe,
			 const meter_t ye,
			 const degree_t ae,
			 meter_t &xErr,
			 meter_t &yErr,
			 degree_t &aErr)
{
  GTL ("CMOBILE::MoveDirect()");

  float x, y, a;

  if (GetPosition (x, y, a) < 0)
    {
      GTLFATAL (("GetFRame() failed\n"));
      return -1;
    }

  GTLPRINT (("curr = (%.3f,%.3f,%.3f)\n", x, y, a));
  GTLPRINT (("targ = (%.3f,%.3f,%.3f)\n", xe, ye, ae));

#if 0
  if (SetMode (CMOBILE::TARGET) < 0)
    {
      GTLFATAL (("CMOBILE::SetMode(TARGET) failed\n"));
      return -1;
    }

  if (SetSpeed (1.0) < 0)
    {
      GTLFATAL (("CMOBILE::SetSpeed() failed\n"));
      return -1;
    }
#endif

  if (Move (xe, ye, ae) < 0)
    {
      GTLFATAL (("CMOBILE::Move() failed\n"));
      return -1;
    }

  if (WaitForCompleted () < 0)
    {
      GTLFATAL (("CMOBILE::WaitForCompleted() failed\n"));
      return -1;
    }

#if 0
  if (SetMode (CMOBILE::STANDBY) < 0)
    {
      GTLFATAL (("CMOBILE::SetMode(STANDBY) failed\n"));
      return -1;
    }
#endif

  if (ComputeError (xe, ye, ae, &xErr, &yErr, &aErr) < 0)
    {
      GTLFATAL (("ComputeError() failed\n"));
      return -1;
    }

  //

  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CMOBILE::MoveViaIntermediate (const meter_t xe,
				  const meter_t ye,
				  const degree_t ae,
				  meter_t &xErr,
				  meter_t &yErr,
				  degree_t &aErr,
				  const meter_t approachDistance)
{
  GTL ("CMOBILE::MoveViaIntermediate()");

  float x, y, a;

  if (GetPosition (x, y, a) < 0)
    {
      GTLFATAL (("GetPosition() failed\n"));
      return -1;
    }

  GTLPRINT (("curr = (%.3f,%.3f,%.3f)\n", x, y, a));
  GTLPRINT (("targ = (%.3f,%.3f,%.3f)\n", xe, ye, ae));
  GTLPRINT (("dist = %f\n", approachDistance));

  if (approachDistance < 0.0)
    {
      GTLFATAL (("negative approachDistance (%f)\n", approachDistance));
      return -1;
    }

  //

  double x1 = xe - approachDistance * cos (M_PI * ae / 180.0);
  double y1 = ye - approachDistance * sin (M_PI * ae / 180.0);
  double a1 = ae;

  GTLPRINT (("going to intermediate point #1 (%.3f,%.3f,%.3f)\n", x1, y1, a1));

  //

#if 0
  if (SetMode (CMOBILE::TARGET) < 0)
    {
      GTLFATAL (("CMOBILE::SetMode(TARGET) failed\n"));
      return -1;
    }

  if (SetSpeed (1.0) < 0)
    {
      GTLFATAL (("CMOBILE::SetSpeed() failed\n"));
      return -1;
    }
#endif

#if 0

  //
  // ++itschere20030307
  //
  // the idea was that for motions with an intermediate point some short and
  // directly reachable distance in front of the target the final point of the
  // path planned part (the iintermediate point) needs no final orientation
  // because the very final motion already includes a rotation towards the
  // final target (finally).
  //
  // however, this does not work. the basic motion sequence in this case looks
  // like this:
  //
  //   (R, T, {R, T,}^* R, T), (R, T, R)
  //
  // if we only look at the ending
  //
  //   ...T, R, T, R
  //
  // it may (does) happen that the first T moves the platform over a drain in
  // the lab floor, so that the first R does not really only rotate, but
  // actually translates it because one wheel is slipping. since the
  // localisation sees this it will still rotate until it points to the target
  // angle, but this target angle no longer points towards the target because
  // it was only computed once in advance and is NOT updated during the motion.
  //
  // the translation then does not start pointing towards the target, and if
  // it's a short translation (0.5m) it may (does) not have enough time to
  // compensate this error before reaching the target (in terms of reaching a
  // position that's on the same line perpendicular to the assumed target
  // direction).
  //
  // practically, in one situation the platform arrives with an error of more
  // than 6cm... :-(
  //
  // so it cannot be generally guaranteed that a motion reaches its goal.
  //
  // on the other hand the alternative idea to track a position until it is
  // finally reached, regardless of what motions are necessary to achive this
  // (basically the old gencontrol mode) cannot be guaranteed to terminate...
  //

  if (MovePoint (x1, y1) < 0)
    {
      GTLFATAL (("CMOBILE::Move() failed\n"));
      return -1;
    }

#else

  if (Move (x1, y1, a1) < 0)
    {
      GTLFATAL (("CMOBILE::Move() failed\n"));
      return -1;
    }

  if (ComputeError (x1, y1, a1, &xErr, &yErr, &aErr) < 0)
    {
      GTLFATAL (("ComputeError() failed\n"));
    }

#endif

  GTLPRINT (("going to target point (%.3f,%.3f,%.3f)\n", xe, ye, ae));

  if (Move (xe, ye, ae) < 0)
    {
      GTLFATAL (("CMOBILE::Move() failed\n"));
      return -1;
    }

  if (WaitForCompleted () < 0)
    {
      GTLFATAL (("CMOBILE::WaitForCompleted() failed\n"));
      return -1;
    }

#if 0
  if (SetMode (CMOBILE::STANDBY) < 0)
    {
      GTLFATAL (("CMOBILE::SetMode(STANDBY) failed\n"));
      return -1;
    }
#endif

  if (ComputeError (xe, ye, ae, &xErr, &yErr, &aErr) < 0)
    {
      GTLFATAL (("ComputeError() failed\n"));
      return -1;
    }

  return 0;
}
