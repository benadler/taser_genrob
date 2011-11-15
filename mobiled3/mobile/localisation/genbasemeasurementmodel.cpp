/***************************************************************************
                          genbasemeasurementmodel.cpp  -  description
                             -------------------
    begin                : Wed Feb 20 2002
    copyright            : (C) 2002 by Axel Schneider, Daniel Westhoff
    email                : axel.schneider@uni-bielefeld.de, daniel.westhoff@uni-bielefeld.de
 ***************************************************************************/

#ifndef DEBUG_GENBASE_MEASUREMENT_MODEL
#define DEBUG_GENBASE_MEASUREMENT_MODEL 0
#endif

#include "math.h"
#include <iostream>

#include "genbasemeasurementmodel.h"


GenBaseMeasurementModel::GenBaseMeasurementModel()
{
  _sizeX = 0;
}



GenBaseMeasurementModel::~GenBaseMeasurementModel()
{
}



/**
   applies the measurement function to x to get a prediction for z
 */
CVEC GenBaseMeasurementModel::apply(const CVEC &x)
{
  if(x.GetSize() >= _minXSize)
    {
      _sizeX = x.GetSize();
      CVEC zHat;
      
      zHat = calcZ(x);
      
#if DEBUG_GENBASE_MEASUREMENT_MODEL		
      cout << "# SYSTEM MODEL - MEASUREMENT PREDICTION:" << endl;
      for (int i=0; i < zHat.GetSize(); i++)
	{
	  cout << "#    z[" << i << "]=" << zHat[i] << endl;
	}
#endif
      
      calcHJacobian(x);
      calcVJacobian(zHat.GetSize());
      
      return zHat;
    }
  return 0;
}



/**
   delivers the Jacobian of the measurment model
 */
const CMAT& GenBaseMeasurementModel::HJacobian() const 
{
   return _HJMatrix;
}



/**
   delivers the Jacobian of the measurement with respect to the noise
 */
const CMAT& GenBaseMeasurementModel::VJacobian() const 
{
   return _VJMatrix;
}



/**
   applies the measurement function to get a prediction for z
 */
CVEC GenBaseMeasurementModel::calcZ(const CVEC &x)
{
  if (_updateFlag == ODOMETRY)
    {
      CVEC zHat(3);
      zHat[0] = x[0];   // omega right
      zHat[1] = x[1];   // omega left
      zHat[2] = x[4];   // omega vel
      return zHat;
    }
  else
    {
      CVEC zHat(_seenMarksNumbers.GetSize()*2);
      for (int i = 0; i < _seenMarksNumbers.GetSize(); i++)
	{
	  int markNr = (int) _seenMarksNumbers[i];
	  
#if DEBUG_GENBASE_MEASUREMENT_MODEL	  		  	
	  cout << "#    MARK " << markNr << ": (" << x[11 + (markNr*2)];
	  cout << "," << x[11 + (markNr*2) + 1] << ")" << endl;
#endif
	  
	  // compute distance
	  zHat[i*2]     = hypot(
				( (x[8] - x[11 + (markNr*2)])     ),
				( (x[9] - x[11 + (markNr*2) + 1]) )
				);
	  // compute angle
	  zHat[(i*2)+1] = atan2(
				(x[11 + (markNr*2) + 1] - x[9]),
				(x[11 + (markNr*2)]     - x[8])
				) - x[10];

	  // try to keep the (predicted) angle close to where the real angle is

	  double diff = zHat[(2*i)+1] - _zHint[(2*i)+1];

	  if (diff > M_PI)
	    diff -= 2.0 * M_PI;
	  else if (diff < -M_PI)
	    diff += 2.0 * M_PI;

	  zHat[(2*i)+1] = _zHint[(2*i)+1] + diff;

	}
      return zHat;			
    }
}



/**
   calculates the H Jacobian
 */
void GenBaseMeasurementModel::calcHJacobian(const CVEC &x)
{
  if (_updateFlag == ODOMETRY)
    {
      _HJMatrix.Create(3,_sizeX);
      _HJMatrix[0][0] = _HJMatrix[1][1] = _HJMatrix[2][4] = 1.0;
    }	
  else
    {
      _HJMatrix.Create( _seenMarksNumbers.GetSize() * 2,
			_sizeX
			);	
      for (int i = 0; i < _seenMarksNumbers.GetSize(); i++)
	{
	  int markNr = (int) _seenMarksNumbers[i];
	  
	  // compute some differences
	  double x_xf = x[8] - x[11 + (markNr*2)    ];
	  double y_yf = x[9] - x[11 + (markNr*2) + 1];
	  double xf_x = x[11 + (markNr*2)    ] - x[8];
	  double yf_y = x[11 + (markNr*2) + 1] - x[9];
	  double root = hypot(x_xf, y_yf);
	  double sq   = (xf_x * xf_x) + (yf_y * yf_y);
	  
	  // fill line i*2 of the matrix	  	
	  _HJMatrix[i*2][8] = x_xf / root;
	  _HJMatrix[i*2][9] = y_yf / root;
	  _HJMatrix[i*2][11+markNr*2] = xf_x / root;
	  _HJMatrix[i*2][11+markNr*2+1] = yf_y / root;
	  
	  // fill line i*2+1 of the matrix
	  _HJMatrix[i*2+1][8] = yf_y / sq;
	  _HJMatrix[i*2+1][9] = x_xf / sq;
	  _HJMatrix[i*2+1][10] = -1;
	  _HJMatrix[i*2+1][11+markNr*2] = y_yf / sq;
	  _HJMatrix[i*2+1][11+markNr*2+1] = xf_x / sq;	  	
	}	
    }
}



/**
   calculates the V Jacobian
 */
void GenBaseMeasurementModel::calcVJacobian(int size)
{
  int i;
  
  _VJMatrix.Create(size, size);
  
  for(i = 0; i < size; i++)
    {
      _VJMatrix[i][i] = 1.0;
    }	
}



/** 
  sets the flag that is used by apply to choose
  the right measurement model
 */
void GenBaseMeasurementModel::setUpdateFlag(UpdateType u)
{
  _updateFlag = u;
}



/**
   copies the vector with the numbers of the seen marks
 */
void GenBaseMeasurementModel::setSeenMarksNumbers(CVEC &marks)
{

#if DEBUG_GENBASE_MEASUREMENT_MODEL	
  cout << "# SEEN MARKS: ";
  for (int i=0; i < marks.GetSize(); i++)
    {
      cout << marks[i] << " ";
    }
  cout << endl;
#endif

  _seenMarksNumbers = marks;
}

