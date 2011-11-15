//**************************************************************************
//
//                          ekf.cpp  -  description
//                             -------------------
//    begin                : Tue Feb 19 2002
//    copyright            : (C) 2002 by Axel Schneider, Daniel Westhoff
//    email                : axel.schneider@uni-bielefeld.de,
//                           daniel.westhoff@uni-bielefeld.de
//
//***************************************************************************

#include <iostream>
#include <stdio.h>
#include <math.h>

#include "ekf.h"


EKF::EKF (void)
{
}


EKF::EKF (SystemModel *f,
	  MeasurementModel *h,
	  CMAT *Q,
	  CMAT *R)
{
  _f = f;
  _h = h;
  _Q = Q;
  _R = R; 				
}


EKF::~EKF (void)
{
}


/** 
  predict the system state with controller input u and time dt
 */
void EKF::predict (CVEC &x,
		   CMAT &P,
		   const CVEC &u,
		   const double &dt) const
{
  // 1. projection of the state
  _f->apply (x, u, dt);

  // 2. projection of the state covariance matrix

  CMAT fj = _f->FJacobian ();
  CMAT wj = _f->WJacobian ();

  P = (fj * P * fj.Trans()) + (wj * (*_Q) * wj.Trans());

  //
  // ++itschere20030515: this takes about 1.3ms on a 2ghz pentium-4, which
  // seems a lot of time. let's have a quick analysis of the complexity of the
  // matrix multiplications. basically all matrices are of size 47x47, so the
  // expression we're computing looks like:
  //
  // 47x47 * 47x47 * 47x47 + 47x47 * 47x47 * 47x47
  //
  // a single 47x47 * 47x47 multiplication needs:
  //
  // 47x47 * 47x47
  // = (47*47) * (47 MUL + 46 ADD)
  //
  // two subsequent multiplications therefore need:
  //
  // 47x47 * 47x47 * 47x47
  // = 2 * ((47*47) * (47 MUL + 46 ADD))
  //
  // and the whole expression:
  //
  // 47x47 * 47x47 * 47x47 + 47x47 * 47x47 * 47x47
  // = 4 * ((47*47) * (47 MUL + 46 ADD)) + (47*47) ADD
  // = 4 * (2209 * (47 MUL + 46 ADD)) + 2209 ADD
  // = 4 * (103823 MUL + 101614 ADD) + 2209 ADD
  // = 415292 MUL + 406456 ADD + 2209 ADD
  // = 415292 MUL + 408665 ADD
  //
  // and you're wondering why this takes so much time?
  //
}


/**
  update the system state with the current measurement vector z
 */
void EKF::update(CVEC &x,
		 CMAT &P,
		 const CVEC &z) const
{
  // create Kalman matrix
  CMAT K;

  // compute measurement prediction and implicit
  // create H and V
  CVEC zhat = _h->apply(x);

  // compute Kalman matrix
  // TODO: P * H^T calculation two times in formula

  K = P * (_h->HJacobian()).Trans()
    * (_h->HJacobian() * P * (_h->HJacobian()).Trans() +
       _h->VJacobian() * (*_R) * (_h->VJacobian()).Trans()
       ).InvLU();

#if 0
  CVEC innovation = z - zhat;
  innovation.Print (stderr);
  K.Print (stderr);
#endif

  // a posteriori estimate of the system state using z 			
  x = x + K * (z - zhat);
	
  // create I matrix
  CMAT I( K.GetRows(), (_h->HJacobian()).GetCols() );
  I.UnitDiagSelf();
	
  // covariance matrix of a posteriori estimate
  // calculation with "Joseph form covariance update"
  // -> Bar-Shalom, Li: "Estimation and Tracking: Priciples,
  //    Techniques and Software", p. 294
  // TODO: twice calculation of (I - WH)
  P = (I - K * _h->HJacobian()) * P * (I - K * _h->HJacobian()).Trans()
    + K * (*_R) * K.Trans();

#if 0
  P.Print (stderr);
#endif
}



/** 
   use to change the systemmodel
 */
void EKF::setSystemModel(SystemModel *f){
	_f = f;
}



/**
  use to change the measurement model
 */
void EKF::setMeasurementModel(MeasurementModel *h){
	_h = h;
}



/**
  change the system covariance matrix
 */
void EKF::setQ(CMAT *Q){
	_Q = Q;
}



/**
  change the measurement covariance matrix
 */
void EKF::setR(CMAT *R){
	_R = R;	
}




