/***************************************************************************
                          systemmodel.h  -  description
                             -------------------
    begin                : Tue Feb 19 2002
    copyright            : (C) 2002 by Axel Schneider, Daniel Westhoff
    email                : axel.schneider@uni-bielefeld.de,
                           daniel.westhoff@uni-bielefeld.de
 ***************************************************************************/


#ifndef SYSTEMMODEL_H
#define SYSTEMMODEL_H

#include "math/vec.h"
#include "math/mat.h"

/**
   the current system model "F" for the robot

   @author Axel Schneider, Daniel Westhoff
*/

class SystemModel{


public:
  SystemModel();
  virtual ~SystemModel();
/**
   applies the system function to x
   <pre>
   u is the control vector
   deltaT is the time difference since the last call
   </pre>
*/
  virtual void apply(CVEC &x, 
		     const CVEC &u, 
		     const double deltaT) = 0;
/**
   delivers the Jacobian of the system model
*/
  virtual const CMAT& FJacobian() const = 0;
/**
   delivers the Jacobian of the system with respect to the noise
*/
  virtual const CMAT& WJacobian() const = 0;

};

#endif
