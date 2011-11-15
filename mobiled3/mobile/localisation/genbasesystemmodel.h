/***************************************************************************
                          genbasesystemmodel.h  -  description
                             -------------------
    begin                : Wed Feb 20 2002
    copyright            : (C) 2002 by Axel Schneider, Daniel Westhoff
    email                : axel.schneider@uni-bielefeld.de, daniel.westhoff@uni-bielefeld.de
 ***************************************************************************/


#ifndef GENBASESYSTEMMODEL_H
#define GENBASESYSTEMMODEL_H

#include <math.h>
#include "systemmodel.h"


/**
  *@author Axel Schneider, Daniel Westhoff
*/

class GenBaseSystemModel : public SystemModel  {



public:
  GenBaseSystemModel(const int radiusWheels, const int la, const double tau);
  virtual ~GenBaseSystemModel();
/**
   applies the system function to x
   <pre>
   u is the control vector
   deltaT is the time difference since the last call
   </pre>
*/
  void apply(CVEC &x, const CVEC &u, const double deltaT);
/**
   delivers the Jacobian of the system model
*/
  const CMAT& FJacobian() const;
/**
   delivers the Jacobian of the system with respect to the noise
*/
  const CMAT& WJacobian() const;



private: // Private attributes

/**
   minimal size of the x vector
*/
  static const int _minXSize = 11;
/**
   minimal size of the u vector
*/
  static const int _minUSize = 2;
/**
   holds the radius of the wheels
*/
  int _radiusWheels;
/**
   the radius from the robot's base to the centre of a wheel
*/
  int _lengthA;
/**
   the system's time constant
*/
  double _tau;
/** 
    result of the sin operation with phi, phi_dot and phi_dotdot
*/
  double _sinResult;
/**
   result of the cos operation with phi, phi_dot and phi_dotdot
*/
  double _cosResult;
/** 
    the size of the vector x
*/
  int _sizeX;
/** 
    pointer to the F Jacobian which is calculated in FJacobian()
*/
  CMAT _FJMatrix;
/**
   pointer to the W Jacobian which is calculated in WJacobian()
*/
  CMAT _WJMatrix;
/** 
    the result of exp(deltaT/tau)
*/
  double _expResult;



private: // Private methods

/**
   no use for the default constructor
*/
	GenBaseSystemModel();
/** 
    applies F to x
*/
  void calcX(CVEC &x, const CVEC &u, const double deltaT);
/** 
    calculates the F Jaconian
*/
  void calcFJacobian(CVEC &x, const double deltaT);
/**
   calculates the W Jaconian
*/
  void calcWJacobian(CVEC &x, const double deltaT);
};


#endif
