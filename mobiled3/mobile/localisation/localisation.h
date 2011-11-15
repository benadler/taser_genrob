/***************************************************************************
                          localisation.h  -  description
                             -------------------
    begin                : Tue Feb 19 2002
    copyright            : (C) 2002 by Axel Schneider, Daniel Westhoff
    email                : axel.schneider@uni-bielefeld.de,
													 daniel.westhoff@uni-bielefeld.de
 ***************************************************************************/


#ifndef LOCALISATION_H
#define LOCALISATION_H

#include <string>
#include <fstream>
#include <vector>

using namespace std;

#include "thread/mutex.h"
#include "math/vec.h"
#include "math/mat.h"

#include "genbasesystemmodel.h"
#include "genbasemeasurementmodel.h"
#include "ekf.h"



struct Measurement
{
  int markNumber;
  double distanceToMark;
  double distance;
  double phi;
  double x;
  double y;
};



/**
   GenBaseII robot localisation

   @author Axel Schneider, Daniel Westhoff
*/
class Localisation
{
public:

  Localisation(const long robXPos,
	       const long robYPos,
	       const double phi,
	       const string &robParamFilePath = "/vol/biorob/share/robot.prm",
	       const string &featurePosFilePath = "/vol/biorob/share/map.marks");

  virtual ~Localisation (void);

  // returns the system state vector x

  const void getSystemState (CVEC &newState);

  // ++itschere20031117: better return it in an argument -> threadsafe

  void getStateCovMatrix (CMAT &cov);

  // ++itschere20030331: get system state position (and variances)

  void GetPos (double &x,
	       double &y,
	       double &phi);

  void GetPos (double &x, double &y, double &phi,
	       double &vxx, double &vxy, double &vyx, double &vyy,
	       double &vphi);

  const void setSystemStatePos(const double x, const double y, const double a);

  double alignAngle (void);

  double alignAngle (const double refAngle);

  //
  //
  //

private:

  CVEC _u;

public:

  void setDesiredVelocities (const double velLeft, const double velRight);

  //
  //
  //

/** 
    compute on filter iteration stepand return
    estimation of system state
*/
  void updateOdometry (const CVEC &z, CVEC &newState);

/** 
    compute the update step and return estiamtion of system state. returns the
    number of associated marks
*/
  int updateLaser (CVEC &z, CVEC &newState);
  int updateLaser (CVEC &z, CVEC &newState, int *matched);

private:

  Localisation();

public:

  //
  // predict() now (20030121) computes its `deltaT' internally. since there
  // seems to be a problem with very short intervals it should not be called
  // too often.
  //

  struct timeval _lastPredict;

  double predict (CVEC &newState);

  void predict (const double deltaT, CVEC &newState);

/** 
    load the positions of the features from the given file
*/
  CVEC loadFeaturePositions(const string &featPosFile);


private:

/**
   load the robot parameter from its parameter file
*/
  void loadParamFile(const string &robParamFilePath);

/**
   read the parameters from the *open* ifstream 'paramFile'
*/
  void readParameters(ifstream &paramFile);

/** 
    print a message like:
<pre>
Localisation->where: msg
  - comment
</pre>
  */
  void message(const string &where,
	       const string &msg,
	       const string &comment = "") ;

/**
   set the measurement noise covariance matrix
*/
  void setMeasNoiseCovMatrix(const GenBaseMeasurementModel::UpdateType ut,
			     const double scale = 1.0);

/**
   set the system noise covariance matrix
*/
  void setSysNoiseCovMatrix (const double systemVariance);
/**
   read the coordinates of each landmark from the given file
*/
  CVEC readFeaturePositions(ifstream &featPosFile) const;

  //
  // tell the measurement model which marks the robot sees and change the
  // measurement vector z accordingly
  //
  void findCorrectMeasurements (CVEC &z);
  void findCorrectMeasurements (CVEC &z, int *matched);

  //
  //clears the old measurements from the last step and correlates the new ones
  //to the known feature positions
  //
  void correlateMarkAndMeasurement (CVEC &z);

/** 
    create new measurement vector from '_measurements'
    notify measurement model about seen marks
*/
  CVEC changeMeasurement (void);

  void printMeasurement (void);
    	
/** 
    erase all Measurements with distance > '_maxMarkDistance'
    leave only the best correlation to a mark in '_measurement'
*/
  void eraseBadMeasurements (int *matched = 0x0);

  			
private: // Private attributes

/** 
    maximum velocity of the robot
*/
  double _maxVel;
/** 
    what type of robot do we locate
*/
  string _robotType;
/** 
    distance between robot base and drive wheels
*/
  double _la;
/** 
    wheel radius of the drive wheels
*/
  double _wheelRadius;
/** 
    system state vector
*/
  CVEC *_x;
/** 
    system model for GenBaseII robot
*/
  GenBaseSystemModel *_f;
/** 
    model for the measurement on the GenBaseII robot
*/
  GenBaseMeasurementModel *_h;
/** 
    system noise covariance matrix
*/
  CMAT *_Q;
/** 
    measurement noise covariance matrix
*/
  CMAT *_R;
/** 
    system constant
*/
  double _tau;
/** 
    system state covariance matrix
*/
  CMAT *_P;
/** 
    measurement vector
*/
  CVEC *_z;
/** 
    extended Kalman filter
*/
  EKF *_ekf;

  CMUTEX _mutex;
/**
   maximum angle velocity
*/
  double _maxAngleVel;
/**
   number of features in the feature position file
*/
  int _maxMarksNumber;
/** 
    the maximum distance between a mark and its 
    measurement to correlate them
*/
  double _maxDistanceMarkMeasurement;
/** 
    contains all measurements with correlated mark number and distance
*/
  vector<Measurement> *_measurement;
/*
  some variances of the measurements
*/
  double _odometryVariance;
  double _laserDistanceVariance;
  double _laserAngleVariance;
  double _systemVariance;
  double _laserMarkVariance;
  double _gyroVariance;

public:

  void Lock (void)
  {
    _mutex.Lock ();
  }

  void Unlock (void)
  {
    _mutex.Unlock ();
  }
};

#endif
