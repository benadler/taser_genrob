/***************************************************************************
                          measurementmodel.h  -  description
                             -------------------
    begin                : Tue Feb 19 2002
    copyright            : (C) 2002 by Axel Schneider, Daniel Westhoff
    email                : axel.schneider@uni-bielefeld.de,
													 daniel.westhoff@uni-bielefeld.de
 ***************************************************************************/


#ifndef MEASUREMENTMODEL_H
#define MEASUREMENTMODEL_H

#include "math/vec.h"
#include "math/mat.h"


/**
   the current measurement model "H" for the robot

  @author Axel Schneider, Daniel Westhoff
*/

class MeasurementModel{



public:
	MeasurementModel();
	virtual ~MeasurementModel();
/**
   applies the measurement function to x to get a prediction for z
*/
  virtual CVEC apply(const CVEC &x) = 0;
/** 
    delivers the Jacobian of the measurement model
*/
  virtual const CMAT& HJacobian() const = 0;
/** 
    delivers the Jacobian of the measurement with respect to the noise
*/
  virtual const CMAT& VJacobian() const = 0;
};

#endif
