//***************************************************************************
//                          ekf.h  -  description
//                             -------------------
//    begin                : Tue Feb 19 2002
//    copyright            : (C) 2002 by Axel Schneider, Daniel Westhoff
//    email                : axel.schneider@uni-bielefeld.de,
//                           daniel.westhoff@uni-bielefeld.de
//***************************************************************************

#ifndef EKF_H
#define EKF_H

#include "math/vec.h"
#include "math/mat.h"

#include "systemmodel.h"
#include "measurementmodel.h"


/*
   Extended Kalman Filter class
   @author Axel Schneider, Daniel Westhoff
 */

class EKF
{
public: 

  //
  // Create an extended Kalman filter with given system- and measurement model
  // and apropiate covariance matrices
  //

  EKF (SystemModel *f,
       MeasurementModel *h,
       CMAT *Q,
       CMAT *R);

  ~EKF (void);

  // 
  //predict the system state with controller input u and time dt
  //

  void predict (CVEC &x,
	        CMAT &P,
		const CVEC &u,
		const double &dt) const;

  //
  //update the system state with the current measurement vector z
  //

  void update (CVEC &x,
	       CMAT &P,
	       const CVEC &z) const;

  // 
  // use to change the measurement model
  //

  void setMeasurementModel (MeasurementModel *h);

  //
  // use to change the systemmodel
  //

  void setSystemModel (SystemModel *f);

  //
  // change the measurement covariance matrix
  //

  void setR (CMAT *R);

  //
  // change the system covariance matrix
  //

  void setQ (CMAT *Q);

private:

  EKF (void);

  //
  // pointer to the system model
  //

  SystemModel *_f;

  //
  // pointer to the measurement model
  //

  MeasurementModel *_h;

  //
  // pointer to the measurement covariance matrix
  //

  const CMAT *_R;

  //
  // pointer to the system covariance matrix
  //

  const CMAT *_Q;
};


#endif
