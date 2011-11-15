//
// filter.h
//
// multi-dimensional continuous filter with a-posteriori discretization, based
// on the paper "trajectory generation as a non-linear filter" by john lloyd,
// corrected and adapted by torsten scherer.
//

#ifndef _FILTER_H_
#define _FILTER_H_

#include "math/vec.h"


/**
 * @ingroup Motion
 * @brief Mehrdimensionaler, kontinuierlicher Filter mit <i>a posteriori</i> Diskretisierung.
 *
 * Dieser Filter basiert auf dem Paper <i>Trajectory generation as a non-linear filter</i>
 * von John Lloyd und ist korrigiert und angepasst von Torsten Scherer.
 *
 * @section author Author
 *   Torsten Scherer
 * @section date Copyright
 *   (c) 2000-2002
 */
class _CFILTER
{
private:

protected:

  // generic profile computation, doesn't in fact touch `this'...

  double computeProfile (const CVEC &x0In,
			 const CVEC &v0In,
			 const CVEC &xeIn,
			 double a,
			 const double vm,
			 CVEC &tsInOut,
			 CVEC &vpOut) const;

  // generic profile integration, doesn't in fact touch `this'...

  bool integrateProfile (const CVEC &x0In,
			 const CVEC &v0In,
			 const CVEC &xeIn,
			 const CVEC &tsIn,
			 const CVEC &vpIn,
			 const double a,
			 const double te,
			 CVEC &x1Out,
			 CVEC &v1Out) const;

  // generic information

  double rate, worldA, worldVm;

  // world coordinates

  CVEC worldX0, worldV0, worldXe;
  CVEC worldX1, worldV1;   // next setpoint

  // times and peak velocities are always needed. for CFILT_ORTHO there is no
  // real projection, so the names are a bit wrong, but what the heck...

  CVEC projTs, projVp;

  // desired trajectory (start and end point), not obeyed by all filters

  CVEC worldT0, worldTe;

public:

  /** Konstruktor. */
  _CFILTER (void)
  {
    // we need to be able to change parameters later anyway, so there's no
    // point in having a full-fledged constructor if there's also a function to
    // change all this...
    rate = -1.0;                             // ok, at least mark it as illegal
  }

  /** Destruktor. */
  virtual ~_CFILTER (void);

  void SetParameters (const double newRate, const double a, const double vm);
  void SetPositionAndVelocity (const CVEC &x0, const CVEC &v0);
  void SetTarget (const CVEC &xe);

  void SetTrajectory (const CVEC &x0, const CVEC &xe)
  {
    worldT0 = x0;
    worldTe = xe;
  }

  // compute deviation of point `p' from trajectory and/or target

  double ComputeTrajectoryError (const CVEC &p);
  double ComputeTargetError (const CVEC &p);

  // declaring these to return `const CVEC &' should prohibit writing to them

  const CVEC &GetTarget (void) const
  {
    return worldXe;
  }
  const CVEC &GetTimes (void) const
  {
    return projTs;
  }
  const CVEC &GetNewPosition (void) const
  {
    return worldX1;
  }
  const CVEC &GetNewVelocity (void) const
  {
    return worldV1;
  }

  //

  virtual double ComputeProfile (void) = NULL;
  virtual bool IntegrateProfile (void) = NULL;

  virtual double StretchProfile (const double tMin);
};

#endif
