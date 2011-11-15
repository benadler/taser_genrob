/***************************************************************************
                          genbasesystemmodel.cpp  -  description
                             -------------------
    begin                : Wed Feb 20 2002
    copyright            : (C) 2002 by Axel Schneider, Daniel Westhoff
    email                : axel.schneider@uni-bielefeld.de,
                           daniel.westhoff@uni-bielefeld.de
 ***************************************************************************/

#include <iostream>
#include "genbasesystemmodel.h"



#warning "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
#warning "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
#warning "!!"
#warning "!! ACHTUNG: MODELL OHNE BESCHLEUNIGUNG"
#warning "!!"
#warning "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
#warning "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"


#undef USE_ACCELERATION


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

GenBaseSystemModel::GenBaseSystemModel()
{
}



GenBaseSystemModel::~GenBaseSystemModel()
{
}



GenBaseSystemModel::GenBaseSystemModel(const int radiusWheels,
				       const int la,
				       const double tau)
{
  _radiusWheels = radiusWheels;
  _lengthA = la;
  _tau = tau;
  _cosResult = 0.0;
  _sinResult = 0.0;
  _expResult = 0.0;
  _sizeX = 0;
}



/**
   applies the system function to x
 */
void GenBaseSystemModel::apply(CVEC &x,
			       const CVEC &u,
			       const double deltaT)
{
  if((x.GetSize() >= _minXSize) && (u.GetSize() >= _minUSize))
    {
      //the size is important for the dimension of the Jacobians
      _sizeX = x.GetSize();
      //do the calculations
      calcX(x, u, deltaT);
      calcFJacobian(x, deltaT);
      calcWJacobian(x, deltaT);
    }
}



/**
   delivers the Jacobian of the system model
 */
const CMAT& GenBaseSystemModel::FJacobian() const
{
  return _FJMatrix;
}



/**
   delivers the Jacobian of the system with respect to the noise
 */
const CMAT& GenBaseSystemModel::WJacobian() const
{
  return _WJMatrix;
}



/**
    applies F to x
 */
void GenBaseSystemModel::calcX(CVEC &x, const CVEC &u, const double deltaT)
{

  //we need these values for x[0] to x[3] and furtheron for the FJacobian

  _cosResult = cos (x[10]);
  _sinResult = sin (x[10]);
  _expResult = exp (-deltaT / _tau);

  //
  // first, predict the wheel velocities
  //

  x[0] = u[0] + ((x[0] - u[0]) * _expResult); //omega (r, t+1)
  x[1] = u[1] + ((x[1] - u[1]) * _expResult); //omega (l, t+1)

  //
  // second, predict the platform velocity
  //

  x[2] = 0.5 * (x[0] + x[1]) * _radiusWheels * _cosResult; //x_dot (t+1)
  x[3] = 0.5 * (x[0] + x[1]) * _radiusWheels * _sinResult; //y_dot (t+1)
  x[4] = 0.5 * (x[0] - x[1]) * _radiusWheels / _lengthA;  //phi_dot (t+1)

  //
  // third, predict the platform position
  //

#ifndef USE_ACCELERATION

  // ++itschere20030130: actually, this has to be some kind of integral under
  // the assumed velocity function...

  x[ 8] = x[ 8] + x[2] * deltaT; //x (t+1)
  x[ 9] = x[ 9] + x[3] * deltaT; //y (t+1)
  x[10] = x[10] + x[4] * deltaT; //phi (t+1)

#else

  //we have to remember these "old" values for the calculation of x[5], x[6],
  //x[7]

  double xDotT = x[2];

  double yDotT = x[3];
  double phiDotT = x[4];

  x[8] = x[8] + x[2] * deltaT + (x[5]/2.0 * (deltaT *deltaT)); //x (t+1)
  x[9] = x[9] + x[3] * deltaT + (x[6]/2.0 * (deltaT *deltaT)); //y (t+1)
  x[10] = x[10] + x[4] * deltaT + (x[7]/2.0 * (deltaT *deltaT)); //phi (t+1)

#endif

  //
  // fourth (because we don't need it), predict the platform acceleration
  //

#ifndef USE_ACCELERATION

  x[5] = 0.0;
  x[6] = 0.0;
  x[7] = 0.0;

#else

  x[5] = (x[2] - xDotT) / deltaT; //x_dotdot (t+1)
  x[6] = (x[3] - yDotT) / deltaT; //y_dotdot (t+1)
  x[7] = (x[4] - phiDotT) / deltaT; // phi_dotdot (t+1)

#endif
}



/**
   calculates the F Jaconian
 */
void GenBaseSystemModel::calcFJacobian(CVEC &x, const double deltaT)
{
  int i = 0;

  _FJMatrix.Create(_sizeX, _sizeX);

  //...and fill the F Jacobian with values

  _FJMatrix[0][0] = _expResult;
  _FJMatrix[1][1] = _expResult;
  _FJMatrix[2][0] = _FJMatrix[2][1] = (_radiusWheels / 2.0) * _expResult * _cosResult;
  _FJMatrix[2][4] = -x[3] * deltaT;
#ifndef USE_ACCELERATION
  _FJMatrix[2][7] = 0.0;
#else
  _FJMatrix[2][7] = -x[3] * ((deltaT * deltaT) / 2.0);
#endif
  _FJMatrix[2][10] = -x[3];
  _FJMatrix[3][0] = _FJMatrix[3][1] = (_radiusWheels / 2.0) * _expResult * _sinResult;
  _FJMatrix[3][4] = x[2] * deltaT;
#ifndef USE_ACCELERATION
  _FJMatrix[3][7] = 0.0;
#else
  _FJMatrix[3][7] = x[2] * ((deltaT * deltaT) / 2.0);
#endif
  _FJMatrix[3][10] = x[2];
  _FJMatrix[4][0] = _radiusWheels / (2.0 * _lengthA) * _expResult;
  _FJMatrix[4][1] = -1.0 * _FJMatrix[4][0];
  _FJMatrix[5][0] = _FJMatrix[5][1] = _radiusWheels / (2.0 * deltaT) * _expResult * _cosResult;
  _FJMatrix[5][2] = -1.0 / deltaT;
  _FJMatrix[5][4] = -1.0 * x[3];
  _FJMatrix[5][7] = -1.0 * x[3] * (deltaT / 2.0);
  _FJMatrix[5][10] = (1.0 / deltaT) * (-x[3]);
  _FJMatrix[6][0] = _FJMatrix[6][1] = _radiusWheels / (2.0 * deltaT) * _expResult * _sinResult;
  _FJMatrix[6][3] = -1.0 / deltaT;
  _FJMatrix[6][4] = x[2];
  _FJMatrix[6][7] = x[2] * (deltaT / 2.0);
  _FJMatrix[6][10] = (1.0 / deltaT) * x[2];

  _FJMatrix[7][0] = _radiusWheels / (2.0 * _lengthA * deltaT) * _expResult;
  _FJMatrix[7][1] = -1.0 * _FJMatrix[7][0];
  _FJMatrix[7][4] = -1.0 / deltaT;

  _FJMatrix[8][2] = _FJMatrix[9][3] = _FJMatrix[10][4] = deltaT;
#ifndef USE_ACCELERATION
  _FJMatrix[8][5] = _FJMatrix[9][6] = _FJMatrix[10][7] = 0.0;
#else
  _FJMatrix[8][5] = _FJMatrix[9][6] = _FJMatrix[10][7] = (deltaT * deltaT) / 2.0;
#endif
  _FJMatrix[8][8] = _FJMatrix[9][9] = _FJMatrix[10][10] = 1.0;

  for(i = 11; i < _sizeX; i++)
    {
      _FJMatrix[i][i] = 1.0;
    }
}



/**
   calculates the W Jaconian
 */
void GenBaseSystemModel::calcWJacobian(CVEC &x, const double deltaT)
{
  int i = 0;

  _WJMatrix.Create(_sizeX, _sizeX);

  //..and fill the W Jacobian with values
  for(i = 0; i < _sizeX; i++)
    _WJMatrix[i][i] = 1.0;

  _WJMatrix[2][0] = _WJMatrix[2][1] = (_radiusWheels / 2.0) * _cosResult;
  _WJMatrix[3][0] = _WJMatrix[3][1] = (_radiusWheels / 2.0) * _sinResult;
  _WJMatrix[4][0] = _radiusWheels / (2.0 * _lengthA);
  _WJMatrix[4][1] = -1.0 * _WJMatrix[4][0];
  _WJMatrix[5][0] = _WJMatrix[5][1] = _WJMatrix[2][0] / deltaT;
  _WJMatrix[5][2] = _WJMatrix[6][3] = _WJMatrix[7][4] = 1.0;
  _WJMatrix[6][0] = _WJMatrix[6][1] = _WJMatrix[3][0] / deltaT;
  _WJMatrix[7][0] = _WJMatrix[4][0] / deltaT;
  _WJMatrix[7][1] = _WJMatrix[4][1] / deltaT;
}
