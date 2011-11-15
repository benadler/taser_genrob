/***************************************************************************
                          genbasemeasurementmodel.h  -  description
                             -------------------
    begin                : Wed Feb 20 2002
    copyright            : (C) 2002 by Axel Schneider, Daniel Westhoff
    email                : axel.schneider@uni-bielefeld.de, daniel.westhoff@uni-bielefeld.de
 ***************************************************************************/


#ifndef GENBASEMEASUREMENTMODEL_H
#define GENBASEMEASUREMENTMODEL_H

#include "measurementmodel.h"


/**
  *@author Axel Schneider, Daniel Westhoff
*/

class GenBaseMeasurementModel : public MeasurementModel  {



private:

  CVEC _zHint;

public:

  enum UpdateType { ODOMETRY, LASER };

  GenBaseMeasurementModel();
  ~GenBaseMeasurementModel();
/**
   applies the measurement function to x to get a prediction for z
*/
  CVEC apply(const CVEC &x);
/**
   delivers the Jacobian of the measurement with respect to the noise
*/
  const CMAT& VJacobian() const;
/** 
   delivers the Jacobian of the measurment model
*/
  const CMAT& HJacobian() const;
/** 
   sets the flag that is used by apply to choose the right measurement model
*/
  void setUpdateFlag(UpdateType u);
/**
   set a vector with the numbers of the seen marks
*/
  void setSeenMarksNumbers(CVEC &marks);
/**
    set hint z for later magic in apply()/calcZ()
*/
  void setHintZ (CVEC &zHint)
    {
      _zHint = zHint;
      //printf ("setting hint z:\n");
      //_zHint.Print ();
      //fflush (stdout);
    }

private: // Private attributes

/** 
   minimal length of the x vector
*/
  static const int _minXSize = 11;
/**
   pointer to the H Jacobian which is calculated in HJacobian()
*/
  CMAT _HJMatrix;
/**
   pointer to the V Jacobian which is calculated in VJacobian()
*/
  CMAT _VJMatrix;
/** 
   the size of the x vector
*/
  int _sizeX;
/** 
   apply chooses the kind of update (laser or odometry)
*/
  UpdateType _updateFlag;
/** 
   numbers of seen laser reflector marks
*/
  CVEC _seenMarksNumbers;



private: // Private methods

/**
   calculates the H Jacobian
*/
  void calcHJacobian(const CVEC &x);
/**
   calculates the V Jacobian
*/
  void calcVJacobian(int size);
/**
   applies the measurement function to get a prediction for z
*/
  CVEC calcZ(const CVEC &x);

};


#endif
