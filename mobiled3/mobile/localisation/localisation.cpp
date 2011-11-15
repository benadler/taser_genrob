/***************************************************************************
                          localisation.cpp  -  description
                             -------------------
    begin                : Tue Feb 19 2002
    copyright            : (C) 2002 by Axel Schneider, Daniel Westhoff
    email                : axel.schneider@uni-bielefeld.de,
													 daniel.westhoff@uni-bielefeld.de
 ***************************************************************************/

#ifndef DEBUG_LOCALISATION
#define DEBUG_LOCALISATION 0
#endif

#undef VARIANCE_LOGGING
#undef INTERNAL_PREDICT

#include <sys/time.h>

#include "localisation.h"
#include <iostream>
#include <sstream>


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#if 0   // ++itschere20030714: needed for suse-7.2 (???)

static const double fmax (const double arg1, const double arg2)
{
  if (arg1 > arg2)
    {
      return arg1;
    }

  return arg2;
}

#endif


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

Localisation::Localisation (const long robXPos,
			    const long robYPos,
			    const double phi,
			    const string &robParamFilePath,
			    const string &featurePosFilePath)
{
  CVEC features;

  _ekf = 0x0;

  // load robot parameters
  loadParamFile (robParamFilePath);

  // load feature positions
  features = loadFeaturePositions (featurePosFilePath);
  _maxMarksNumber = features.GetSize () / 2;

  // create system state vector
  _x = new CVEC (11 + features.GetSize ());

  // init measurement vector
  _z = new CVEC ();

  // create system model
  _f = new GenBaseSystemModel ((int)_wheelRadius,
			       (int)_la,
			       _tau);

  // create measurement model
  _h = new GenBaseMeasurementModel ();

  // create system noise covariance matrix
  _Q = new CMAT;

  setSysNoiseCovMatrix (_systemVariance);

  // create measurement noise covariance matrix
  _R = new CMAT;

  // *** setMeas... will be called before each update ***
  // setMeasNoiseCovMatrix(GenBaseMeasurementModel::ODOMETRY);

  // create and init the state covariance matrix
  _P = new CMAT (*_Q);

#if MOBILEVERSION == HAMBURG

  // ++westhoff20040621: Q is zero for the landmarks (there is no noise),
  //                     but the estimated covariance should be > 0.
  //                     Otherwise: In figure 3.10 (diploma thesis) equation 2
  //                     of the prediction is always zero.
  for (int i = 0; i < features.GetSize(); i++)
  {
    (*_P)[11+i][11+i] = 25.0;
  }

#endif

  // set initial values of the robot
  //  - robot is standing still -> x[0-7] = 0
  (*_x)[8] = robXPos;
  (*_x)[9] = robYPos;
  (*_x)[10] = phi;

  for (int i = 0; i < features.GetSize (); i++)
    {
      (*_x)[11+i] = features[i];
    }

  // create extended Kalman filter
  _ekf = new EKF (_f, _h, _Q, _R);

  // init measurement vector
  _measurement = new vector<Measurement>;

  //
  //
  //

  _u.Create (0.0, 0.0);

  gettimeofday (&_lastPredict, 0x0);
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

Localisation::~Localisation (void)
{
  delete _x;
  delete _z;
  delete _f;
  delete _h;
  delete _Q;
  delete _R;
  delete _P;
  delete _ekf;
  delete _measurement;
}


///////////////////////////////////////////////////////////////////////////////
//
// load the robot parameter from its parameter file
//
///////////////////////////////////////////////////////////////////////////////

void Localisation::loadParamFile (const string &robParamFilePath)
{
  // open file for reading
  std::ifstream paramFile (robParamFilePath.c_str ());

  // test if file has been opend, if not terminate program
  if (!paramFile)
    {
      message ("loadParamFile()",
	       "ERROR",
	       "No parameter file at: " + robParamFilePath
	       );
      abort ();
    }

  // read the parameter from the file
  readParameters (paramFile);

  // close the parameter file
  paramFile.close ();

#if DEBUG_LOCALISATION
  message("loadParamFile()", "SUCCESS");
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
// read the parameters from the *open* ifstream 'paramFile'
//
///////////////////////////////////////////////////////////////////////////////

void Localisation::readParameters (ifstream &paramFile)
{
  // *** TODO: Parameter file must have correct syntax! Fix it! ***

  // read the parameter file line by line
  string line, parameter, equal, rest;
  int i_value      = 0;
  double d_value   = 0.0;
  string str_value = "";

  while (getline(paramFile, line))
    {
      if ((line[0]!='#') && (line != ""))
	{
	  // init value-variables
	  d_value   = 0.0;
	  str_value = "";

	  // build stream with line
	  istringstream streamLine(line);

	  // read parameter name
	  streamLine >> parameter >> equal;

	  // check syntax concerning the equal symbol
	  if (equal == "=")
	    {
	      // print the parameter
	      string empty = "";
	      for (int i = 0; i < (30 - (int)parameter.length()); i++ )
		{
		  empty += ' ';
		}

#if DEBUG_LOCALISATION
	      cout << "# " << parameter  << empty << ": ";
#endif

	      // check parameter, get value and print it
	      if (parameter == "RobotType")
		{
		  streamLine >> str_value;

#if DEBUG_LOCALISATION
		  if (str_value[0] != '#')
		    cout << str_value;
		  cout << endl;
#endif

		}
	      else if (parameter == "MaxVel")
		{
		  streamLine >> d_value;

#if DEBUG_LOCALISATION
		  cout << d_value << " m/s" << endl;
#endif

		}
	      else if (parameter == "MaxAngleVel")
		{
		  streamLine >> d_value;

#if DEBUG_LOCALISATION
		  cout << d_value << " degrees/s" << endl;
#endif

		}
	      else if (parameter == "OdometryVariance")
		{
		  streamLine >> d_value;
		  _odometryVariance = d_value;

#if DEBUG_LOCALISATION
		  cout << d_value << "" << endl;
#endif

		}
	      else if (parameter == "GyroVariance")
		{
		  streamLine >> d_value;
		  _gyroVariance = d_value;

#if DEBUG_LOCALISATION
		  cout << d_value << "" << endl;
#endif

		}
	      else if (parameter == "LaserDistanceVariance")
		{
		  streamLine >> d_value;
		  _laserDistanceVariance = d_value;

#if DEBUG_LOCALISATION
		  cout << d_value << "" << endl;
#endif

		}
	      else if (parameter == "LaserAngleVariance")
		{
		  streamLine >> d_value;
		  _laserAngleVariance = d_value;

#if DEBUG_LOCALISATION
		  cout << d_value << "" << endl;
#endif

		}
	      else if (parameter == "SystemVariance")
		{
		  streamLine >> d_value;
		  _systemVariance = d_value;

#if DEBUG_LOCALISATION
		  cout << d_value << "" << endl;
#endif

		}
	      else if (parameter == "LaserMarkVariance")
		{
		  streamLine >> d_value;
		  _laserMarkVariance = d_value;

#if DEBUG_LOCALISATION
		  cout << d_value << "" << endl;
#endif

		}

	      else if (parameter == "WheelRadius")
		{
		  streamLine >> i_value;
		  _wheelRadius = i_value;

#if DEBUG_LOCALISATION
		  cout << i_value << " mm" << endl;
#endif

		}
	      else if (parameter == "DistanceDriveWheels")
		{
		  streamLine >> i_value;
		  _la = i_value / 2.0;

#if DEBUG_LOCALISATION
		  cout << i_value << " mm" << endl;
#endif

		}
	      else if (parameter == "Tau")
		{
		  streamLine >> d_value;
		  _tau = d_value;

#if DEBUG_LOCALISATION
		  cout << d_value << " s" << endl;
#endif

		}
	      else if (parameter == "MaxDistanceMarkMeasurement")
		{
		  streamLine >> i_value;
		  _maxDistanceMarkMeasurement = (double) i_value;

#if DEBUG_LOCALISATION
		  cout << i_value << " mm" << endl;
#endif

		}
	      else
		{
		  message("readParameters()",
			  "ERROR",
			  "Unknown parameter in file: " + parameter
			  );
		  abort();
		}
	    }
	  else
	    {
	      message("readParameters()",
		      "ERROR",
		      "Equal symbol expected after: " + parameter
		      );
	      abort();
	    }
	}
    }
}



/**
  print a message like:
  <pre>
  Localisation->where: msg
    - comment
  </pre>
 */
void Localisation::message(const string &where,
			   const string &msg,
			   const string &comment)
{
  cout << endl << "# Localisation->" << where << ": ";
  cout << "*** " << msg << " ***" << endl;
  if (comment != "")
    cout << "#   - " << comment << endl;
  cout << endl;
}



/**
  load the positions of the features from the given file
 */
CVEC Localisation::loadFeaturePositions(const string &featPosFilePath)
{
  CVEC features;

  // open file for reading
  std::ifstream featPosFile(featPosFilePath.c_str());

  // test if file has been opened, if not terminate program
  if (!featPosFile)
    {
      message("loadParamFile()",
	      "ERROR",
	      "No parameter file at: " + featPosFilePath
	      );
      abort();
    }

  // read all feature positions into the vector 'features'
  features = readFeaturePositions(featPosFile);

  // close file
  featPosFile.close();

#if DEBUG_LOCALISATION
  message("loadFeaturesPositions()", "SUCCESS");
#endif

  return features;
}


///////////////////////////////////////////////////////////////////////////////
//
// set the system noise covariance matrix
//
///////////////////////////////////////////////////////////////////////////////

void Localisation::setSysNoiseCovMatrix (const double systemVariance)
{
  _Q->Create (_x->GetSize (), _x->GetSize ());

  // set variances of the system
#if 1
  // TODO: THINK ABOUT IT
  (*_Q)[0][0]   = systemVariance;                   // wheel velocities
  (*_Q)[1][1]   = systemVariance;                   // wheel velocities
  (*_Q)[2][2]   = _wheelRadius * (*_Q)[0][0];       // translatoric vel
  (*_Q)[3][3]	= _wheelRadius * (*_Q)[0][0];       // translatoric vel
  (*_Q)[4][4]   = _wheelRadius/_la * (*_Q)[0][0];   // omega
  (*_Q)[5][5]   = (*_Q)[2][2] * (*_Q)[2][2];        // acceleration
  (*_Q)[6][6]   = (*_Q)[2][2] * (*_Q)[2][2];        // acceleration
  (*_Q)[7][7]   = (*_Q)[4][4];		            // acc. omega
  (*_Q)[8][8]   = (*_Q)[2][2] + (*_Q)[5][5];        // position
  (*_Q)[9][9]   = (*_Q)[2][2] + (*_Q)[5][5];        // position
  (*_Q)[10][10] = (*_Q)[4][4] + (*_Q)[7][7];        // orientation
#else
  for (int i=0; i < 11; i++)
    {
      (*_Q)[i][i] = _systemVariance;
    }
#endif

  for (int i=11; i<_x->GetSize(); i++)
    {
      (*_Q)[i][i] = _laserMarkVariance;
    }

  // show Q to the extended Kalman Filter
  if (_ekf != 0x0)
    {
      _ekf->setQ (_Q);
    }
}


///////////////////////////////////////////////////////////////////////////////
//
// set the measurement noise covariance matrix
//
// ++itschere20030122: `scale' is an attempt to weigh the odometry
// variance. using a smaller variance when the platform is standing still is
// supposed to put more emphasis on the odometry (compared to gyro and laser)
// and thus keep it from drifting because of noise in the other measurements.
//
///////////////////////////////////////////////////////////////////////////////

void Localisation::setMeasNoiseCovMatrix (const GenBaseMeasurementModel::UpdateType ut,
					  const double scale)
{
  _R->Create (_z->GetSize(), _z->GetSize());

  if (ut == GenBaseMeasurementModel::ODOMETRY)
    {
      (*_R)[0][0] = _odometryVariance * scale;
      (*_R)[1][1] = _odometryVariance * scale;
      (*_R)[2][2] = _gyroVariance;
    }
  else
    {
      for (int i=0; i < _z->GetSize()/2; i++)
	{
	  (*_R)[i*2][i*2]         = _laserDistanceVariance;
	  (*_R)[(i*2)+1][(i*2)+1] = _laserAngleVariance;
	}
    }

  // show R to the extended Kalman Filter
  if (_ekf != 0x0)
    {
      _ekf->setR (_R);
    }
}


/**
   returns the system state vector x
 */
const void Localisation::getSystemState (CVEC &newState)
{
  _mutex.Lock ();

#ifdef INTERNAL_PREDICT
  predict ();
#endif

  newState = *_x;

  _mutex.Unlock ();
}


const void Localisation::setSystemStatePos (const double x,
					    const double y,
					    const double a)
{
  _mutex.Lock ();

#ifdef INTERNAL_PREDICT
  predict ();
#endif

  // reset state

  _x->operator[] (0) = 0.0;
  _x->operator[] (1) = 0.0;

  _x->operator[] (2) = 0.0;
  _x->operator[] (3) = 0.0;
  _x->operator[] (4) = 0.0;

  _x->operator[] (5) = 0.0;
  _x->operator[] (6) = 0.0;
  _x->operator[] (7) = 0.0;

  _x->operator[] (8) = x;
  _x->operator[] (9) = y;
  _x->operator[] (10) = a;

#if 1

  *_P = *_Q;

#if MOBILEVERSION == HAMBURG

  // ++westhoff20040621: Q is zero for the landmarks (there is no noise),
   //                     but the estimated covariance should be > 0.
  //                     Otherwise: In figure 3.10 (diploma thesis) equation 2
  //                     of the prediction is always zero.
  for (int i = 11; i < _x->GetSize(); i++)
  {
     (*_P)[i][i] = 25.0;
  }

#endif


#endif

  _mutex.Unlock ();
}



//
//  returns the system state covariance matrix P
//

void Localisation::getStateCovMatrix (CMAT &cov)
{
  _mutex.Lock ();

#ifdef INTERNAL_PREDICT
  predict ();
#endif

  cov = *_P;

  _mutex.Unlock ();
}


//
// returns the current position and orientation
//

void Localisation::GetPos (double &x, double &y, double &phi)
{
  _mutex.Lock();

  x = (*_x)[8];
  y = (*_x)[9];
  phi = (*_x)[10];

  _mutex.Unlock();
}


void Localisation::GetPos (double &x, double &y, double &phi,
			   double &vxx, double &vxy, double &vyx, double &vyy,
			   double &vphi)
{
  _mutex.Lock();

  x = (*_x)[8];
  y = (*_x)[9];
  phi = (*_x)[10];

#if 0
  const int rows = _P->GetRows ();
  const int cols = _P->GetCols ();

  fprintf (stderr, "Localisation::GetPos() -- %i*%i\n", rows, cols);
  fflush (stderr);
#endif

  vxx = (_P->operator[] (8))[8];
  vxy = (_P->operator[] (8))[9];
  vyx = (_P->operator[] (9))[8];
  vyy = (_P->operator[] (9))[9];

  vphi = (_P->operator[] (10))[10];

  _mutex.Unlock();
}


void Localisation::setDesiredVelocities (const double velLeft,
					 const double velRight)
{
  _mutex.Lock ();

#ifdef INTERNAL_PREDICT
  predict ();
#endif

  _u.Create (velRight, velLeft);

#if 0

  //
  // ++itschere20030121: this was an attempt to reduce the system variance when
  // the platform is standing still. the idea was that if it is standing still
  // we don't want the position to jump around because of noise in the
  // measurements. reducing the system variance in this case means to put more
  // emphasis on the system, and therefore the prediction rather than the
  // update.
  //
  // it DOES work in terms of keeping the platform still, but it also leads to
  // putting less emphasis on the measurements when moving, and thus it looses
  // the association of the laser marks rather quickly (at least when there are
  // only two around).
  //

  double f = fmax (fabs (velRight), fabs (velRight));

  setSysNoiseCovMatrix (f * _systemVariance + 0.001);

#endif

  _mutex.Unlock ();
}


double Localisation::predict (CVEC &newState)
{
  //

  struct timeval now;

  gettimeofday (&now, 0x0);

  double deltaT =
    (now.tv_sec - _lastPredict.tv_sec) +
    (now.tv_usec - _lastPredict.tv_usec) / 1000000.0;

  _lastPredict = now;

  //

  _ekf->predict((*_x), (*_P), _u, deltaT);

  newState = *_x;

  //

  return deltaT;
}


void Localisation::predict (const double deltaT, CVEC &newState)
{
  _ekf->predict((*_x), (*_P), _u, deltaT);

  newState = *_x;
}



/**
   read the coordinates of each landmark from the given file
 */
CVEC Localisation::readFeaturePositions(ifstream &featPosFile) const
{
  // *** TODO: Feature position file must have correct syntax! Fix it! ***

  // read the parameter file line by line
  string line;
  double x, y;
  int i = 0;
  CVEC features;

#if DEBUG_LOCALISATION
  cout << "# Feature positions:" << endl;
#endif

  while (getline(featPosFile, line))
    {
      if ((line[0]!='#') && (line != ""))
	{
	  int n = 0;
	  CVEC help(++i * 2);

	  // build stream with line
	  istringstream streamLine(line);

	  // read parameter name
	  streamLine >> x >> y;

	  // add x and y to the features vector
	  for (n = 0; n < (i*2)-2 ; n++)
	    {
	      help[n] = features[n];
	    }
	  help[n++] = x;
	  help[n++] = y;
	  features = help;

#if DEBUG_LOCALISATION
	  cout << "# Mark " << i-1 << ": " << x << "\t" << y << endl;
#endif
	}
    }
  return features;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#ifdef VARIANCE_LOGGING

static void varianceLogging (const int marks,
			     const CVEC &state,
			     const CMAT &cov)
{
  static FILE *fp = 0x0;

  if (!fp)
    {
      if (!(fp = fopen ("localisation.out", "w")))
	{
	  fprintf (stderr, "failed to create file \"localisation.out\"\n");
	  abort ();
	}
    }

  struct timeval tv;
  gettimeofday (&tv, 0x0);

  struct tm tm;
  localtime_r (&tv.tv_sec, &tm);

  fprintf (fp, "%04i%02i%02i%02i%02i%02i%06i %2i %8.3f %8.3f %-7.4f %9.3f %+9.3f %+9.3f %9.3f %7.4f\n",
	   tm.tm_year + 1900,
	   tm.tm_mon + 1,
	   tm.tm_mday,
	   tm.tm_hour,
	   tm.tm_min,
	   tm.tm_sec,
	   (int)tv.tv_usec,
	   marks,
	   state[8],
	   state[9],
	   state[10],
	   cov[8][8],
	   cov[8][9],
	   cov[9][8],
	   cov[9][9],
	   cov[10][10]);

  fflush (fp);
}

#endif


///////////////////////////////////////////////////////////////////////////////
//
// compute update step and return estimation of system state
//
///////////////////////////////////////////////////////////////////////////////

void Localisation::updateOdometry (const CVEC &z, CVEC &newState)
{
#ifdef INTERNAL_PREDICT
  predict ();
#endif

  // tell measurement model to do an odometry
  // update step
  _h->setUpdateFlag (GenBaseMeasurementModel::ODOMETRY);

  *_z = z;

#if 1

  double f = fmax (fabs (z[0]), fabs (z[1]));   // f=0..2*PI

  f = 0.9 * (1.0 - exp (-f)) + 0.1;  // f=0..1

  setMeasNoiseCovMatrix (GenBaseMeasurementModel::ODOMETRY, f);

#else

  setMeasNoiseCovMatrix (GenBaseMeasurementModel::ODOMETRY);

#endif

  _ekf->update ((*_x), (*_P), z);

#ifdef VARIANCE_LOGGING
  varianceLogging (0, *_x, *_P);
#endif

  newState = *_x;
}


///////////////////////////////////////////////////////////////////////////////
//
// compute the update step and return estiamtion of system state
//
///////////////////////////////////////////////////////////////////////////////

int Localisation::updateLaser (CVEC &z, CVEC &newState)
{
#ifdef INTERNAL_PREDICT
  predict ();
#endif

#if 0
  // tell measurement model to do an odometry update step
  _h->setUpdateFlag (GenBaseMeasurementModel::LASER);
#endif

  // erase bad measurements, change z vector and show the model which it
  // receives
  findCorrectMeasurements (z);

  int marks = z.GetSize () >> 1;

#if 1
  if (marks >= 1)
#else
  if (marks >= 3)
#endif
    {
      // tell measurement model to do an odometry update step
      _h->setUpdateFlag (GenBaseMeasurementModel::LASER);

      _h->setHintZ (z);

      // update the extended Kalman filter
      *_z = z;

      setMeasNoiseCovMatrix (GenBaseMeasurementModel::LASER);

      _ekf->update ((*_x), (*_P), z);

#ifdef VARIANCE_LOGGING
      varianceLogging (marks, *_x, *_P);
#endif
    }

  newState = *_x;

  return marks;
}


///////////////////////////////////////////////////////////////////////////////
//
// like updateLaser(), only that a vector of matching IDs is returned
//
///////////////////////////////////////////////////////////////////////////////

int Localisation::updateLaser (CVEC &z, CVEC &newState, int *matched)
{
#ifdef INTERNAL_PREDICT
  predict ();
#endif

#if 0
  // tell measurement model to do an odometry update step
  _h->setUpdateFlag (GenBaseMeasurementModel::LASER);
#endif

  // erase bad measurements, change z vector and show the model which it
  // receives

  findCorrectMeasurements (z, matched);

  int marks = z.GetSize () >> 1;

#if 1
  if (marks >= 1)
#else
  if (marks >= 3)
#endif
    {
      // tell measurement model to do an odometry update step
      _h->setUpdateFlag (GenBaseMeasurementModel::LASER);

      _h->setHintZ (z);

      // update the extended Kalman filter
      *_z = z;

      setMeasNoiseCovMatrix (GenBaseMeasurementModel::LASER);

      _ekf->update ((*_x), (*_P), z);

#ifdef VARIANCE_LOGGING
      varianceLogging (marks, *_x, *_P);
#endif
    }

  newState = *_x;

  return marks;
}


///////////////////////////////////////////////////////////////////////////////
//
// tell the measurement model which marks the robot sees
//
///////////////////////////////////////////////////////////////////////////////

void Localisation::findCorrectMeasurements (CVEC &z)
{
  // correlate measurements with known marks
  correlateMarkAndMeasurement (z);

  // erase easurement which are to far away from a known mark or where
  // there are better measurement
  eraseBadMeasurements ();

  // change measurement vector
  z = changeMeasurement ();
}


void Localisation::findCorrectMeasurements (CVEC &z, int *matched)
{
  // correlate measurements with known marks
  correlateMarkAndMeasurement (z);

  // erase easurement which are to far away from a known mark or where
  // there are better measurement
  eraseBadMeasurements (matched);

  // change measurement vector
  z = changeMeasurement ();
}


///////////////////////////////////////////////////////////////////////////////
//
// clears the old measurements and correlates the new
//
///////////////////////////////////////////////////////////////////////////////

void Localisation::correlateMarkAndMeasurement (CVEC &z)
{
  // erase old measurements
  _measurement->clear();

  for (int i=0; i<z.GetSize()/2; i++)
    {
      // create measurement vector element
      Measurement m;
      m.markNumber     = -1;
      m.distanceToMark = 99999;

      // get distance and angle measurement of mark
      m.distance = z[i*2];
      m.phi      = z[i*2+1];

      // compute world coordinates
      m.x = (*_x)[8] + m.distance * cos(m.phi + (*_x)[10]);
      m.y = (*_x)[9] + m.distance * sin(m.phi + (*_x)[10]);

      // compare distance between measured mark and all known marks

      for (int n=0; n<_maxMarksNumber; n++)
	{
	  double dist = hypot( (m.x - (*_x)[11+(n*2)]) , (m.y - (*_x)[12+(n*2)])  );
	  if (dist < m.distanceToMark)
	    {
	      m.distanceToMark = dist;
	      m.markNumber = n;
	    }
	}

      // save measurement
      _measurement->push_back(m);
    }
}


///////////////////////////////////////////////////////////////////////////////
//
// erase all Measurements with distance > '_maxDistanceMarkMeasurement' leave
// only the best correlation to a mark in '_measurement'
//
///////////////////////////////////////////////////////////////////////////////

void Localisation::eraseBadMeasurements (int *matched)
{
  vector<Measurement>::iterator it, closest, erase;

  // check each known mark

  for (int i=0; i < _maxMarksNumber; i++)
    {
      closest = _measurement->end();

      // find closest measurement that is mapped to this mark

      for (it = _measurement->begin(); it != _measurement->end(); it++)
	{
	  if ((*it).markNumber == i)
	    {
	      // this measurement is mapped to the mark

	      //
	      // ++itschere20031117: what does this calculation do?
	      //

	      if (closest != _measurement->end ())   // if a previous closest has been found...
		{
		  if ((*closest).distanceToMark < (*it).distanceToMark)   // ...check for this new one
		    {
		      (*it).markNumber = -1;
		    }
		  else
		    {
		      (*closest).markNumber = -1;
		    }
		}
	      else
		{
		  closest = it;
		}
	    }
	}

      // is it in the catch radius?

      if ((closest != _measurement->end ()) &&
	  ((*closest).distanceToMark > _maxDistanceMarkMeasurement))
	{
	  (*closest).markNumber = -1;
	}

      //printMeasurement ();
    }

  //
  //
  //

  for (it = _measurement->begin(); it != _measurement->end(); it++)
    {
      if (matched)
	{
	  *matched++ = (*it).markNumber;
	}

      if ((*it).markNumber < 0)
	{
	  erase = it;
	  it--;
	  _measurement->erase (erase);
	}
    }
}


/**
  create new measurement vector from '_measurements'
  notify measurement model about seen marks
 */
CVEC Localisation::changeMeasurement (void)
{
  CVEC zNew((int) (2 * _measurement->size()) );
  CVEC markNumbers( (int) _measurement->size() );
  vector<Measurement>::const_iterator it = _measurement->begin();
  int n = 0;

  while (!(it == _measurement->end ()))
    {
      // add measurement to 'zNew'
      zNew[n*2]     = (*it).distance;
      zNew[(n*2)+1] = (*it).phi;

      // add number of mark to list of numbers
      markNumbers[n] = (*it).markNumber;

#if DEBUG_LOCALISATION
      cout << "#    MARK " << markNumbers[n] << " ->";
      cout << " distance: " << zNew[n*2];
      cout << " phi: " << zNew[(n*2)+1] << endl;
#endif

      // increase iterator and counter
      it++;
      n++;
    }

  // tell measurement model which marks to use
  _h->setSeenMarksNumbers (markNumbers);

  return zNew;
}



void Localisation::printMeasurement()
{

#if DEBUG_LOCALISATION
  cout << " ---> ";
  vector<Measurement>::const_iterator i;
  for(i = _measurement->begin(); i != _measurement->end(); i++)
  {
    cout << (*i).markNumber;
  }
  cout << endl;
#endif

}


//
// ++itschere20030123
//
// map a value `arg' in the range 0..`range'. this is basically
// `fmod', except for negative values.
//

static double fmap (const double arg, const double range)
{
  if (arg < 0.0)
    {
      return range - fmod (-arg, range);
    }

  return fmod (arg, range);
}

//
//
//


double Localisation::alignAngle (void)
{
  _mutex.Lock();

  double angle = _x->operator[] (10);

  //
  // now try to align `angle' to 0
  //

#if 1

  angle = fmap (angle + M_PI, 2.0 * M_PI) - M_PI;

#else

  while (angle > M_PI)
    {
      angle -= 2.0 * M_PI;
    }

  while (angle < -M_PI)
    {
      angle += 2.0 * M_PI;
    }

#endif

  _x->operator[] (10) = angle;

  _mutex.Unlock();

  return angle;
}


double Localisation::alignAngle (const double refAngle)
{
  _mutex.Lock();

  double angle = _x->operator[] (10);
  //double oldAngle = angle;

  //
  // try to align `angle' to `refAngle' (-PI..PI)
  //

  angle -= refAngle;

  //
  // now try to align `angle' to 0
  //

#if 1

  angle = fmap (angle + M_PI, 2.0 * M_PI) - M_PI;

#else

  while (angle > M_PI)
    {
      angle -= 2.0 * M_PI;
    }

  while (angle < -M_PI)
    {
      angle += 2.0 * M_PI;
    }

#endif

  angle += refAngle;

  _x->operator[] (10) = angle;

  _mutex.Unlock();

  return angle;
}
