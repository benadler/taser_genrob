//
//
//

#ifndef _CANPLATFORM_H_
#define _CANPLATFORM_H_

#include <sys/time.h>

#include "thread/mutex.h"

#include "canDongle.h"

#if MOBILEVERSION == HAMBURG
// ++westhoff20040205: need these for the log file of mtor states
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#endif

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class CCANPLATFORM : protected CCANDONGLE
{
#if MOBILEVERSION == HAMBURG

private:

  FILE *_datafile;

public:

  int _stateCtrlLeft, _stateCtrlRight;
  int _tempMeasLeft, _tempMeasRight;

#endif

private:

  // not even implemented
  CCANPLATFORM (const CCANPLATFORM &);
  CCANPLATFORM &operator= (const CCANPLATFORM &);
  bool operator== (const CCANPLATFORM &);

  CMUTEX _mutex;

protected:

  //
  // ++itschere20030527: the following are only used by `StartMotors()'
  //

  void DisableBrakeLeft (const bool on);
  void DisableBrakeRight (const bool on);

  void EnableMotorLeft (const bool on, const int torque);
  void EnableMotorRight (const bool on, const int torque);

  void SyncMotorLeft (const int torque);
  void SyncMotorRight (const int torque);

  void EnableCommLeft (void);
  void EnableCommRight (void);

  //
  // the `InitializeAngle()' methods are only meant to be called from the
  // appropriate `SyncMotor()' methods.
  //

  int _iAngleLeft, _iAngleRight;
  void InitializeAngleLeft (void);
  void InitializeAngleRight (void);

  short _iGyro;
  void InitializeAngleGyro (void);

  //
  // the `Disconnect()', `Connect()' and `SyncMotors()' methods are only meant
  // to be called from the constructor (except `Disconnect()', which is also
  // called from the destructor).
  //

  void Disconnect (void);
  void Connect (void);

  //

  void Lock (void)
    {
      _mutex.Lock ();
    }

  void Unlock (void)
    {
      _mutex.Unlock ();
    }

public:

  CCANPLATFORM (void);

  virtual ~CCANPLATFORM (void);

  //

  void StartMotors (void);

  void GetStatus (bool abortIfFatal = false);

  void GetJoyVal (void);

  int GetGyroVal (void);
  int GetGyroValAbs (void);

  //
  //
  //

private:

  int _voltageCount;
  int _temperatureLeft, _temperatureRight;
  double* _driveTemperatures;


public:

  int GetBatteryVoltageRaw (void);   // returns battery voltage (in counts)
  double ConvertBatteryVoltage (const int raw) const;
  double GetBatteryVoltage (void);   // returns battery voltage (in volt)

  // ++westhoff20050330
  double* GetDriveTemperatures (void);  // returns the temperatures of the drives

  /////////////////////////////////////////////////////////////////////////////
  //
  // velocity output related methods. all angles, delta angles and velocities
  // are in rad or rad/s.
  //
  /////////////////////////////////////////////////////////////////////////////

  void SetVelLeft  (double *vel);
  void SetVelRight (double *vel);

  void SetVelLeft  (const double vel);
  void SetVelRight (const double vel);

  //
  // SetWorldVelocity()
  //
  // sets a new world velocity for the motors. note that in one method the
  // arguments are pointers. this is because both methods convert the world
  // velocity to encoder velocities, which includes rounding (< 1%). you may be
  // interested in the real velocities, and therefore one method returns the
  // them.
  //

  void SetWorldVelocity (double *velLeft, double *velRight);

  void SetWorldVelocity (const double velLeft, const double velRight)
    {
      // ++itschere20030828: not used by mobiled

      double tmpVelLeft  = velLeft;
      double tmpVelRight = velRight;

      SetWorldVelocity (&tmpVelLeft, &tmpVelRight);
    }

protected:

  double _lastVelLeft, _lastVelRight;

public:

  /////////////////////////////////////////////////////////////////////////////
  /// odometry input related methods.
  /////////////////////////////////////////////////////////////////////////////

  void GetDeltaAngleLeft (double &deltaAngle, double &velAngle);
  void GetDeltaAngleRight (double &deltaAngle, double &velAngle);

  void RequestDeltaAngles (void);
  void ReceiveDeltaAngles (double &deltaAngleLeft, double &deltaAngleRight);

  void GetWorldState (double &deltaAngleLeft,
		      double &deltaAngleRight);

protected:

  struct timeval _lastGetWorldState;

  double _maxDiffLeft, _maxDiffRight;
  double _lastErrLeft, _lastErrRight;

public:

  /////////////////////////////////////////////////////////////////////////////
  /// Liefert die gemessenen Winkeländerungen der Antriebsmotoren und
  /// des Gyroskops in <b>Radiant</b>. DIe Spannungswerte der Batterie werden 
  /// ebenfalls ausgelesen.
  /////////////////////////////////////////////////////////////////////////////

  void GetWorldState (double &deltaAngleLeft,
		      double &deltaAngleRight,
		      double &deltaAngleGyro);

  //
  // ++itschere20030527
  //

private:

  bool _emStop;

public:

  void SetEmStop (void);
  void ResetEmStop (void);
};


#endif
