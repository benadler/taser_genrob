//
// canPlatform.cc
// (C) 01/2003 by TeSche (Torsten Scherer)
// <itschere@techfak.uni-bielefeld.de>
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#ifdef DEBUG
#undef DEBUG
#endif
#include "mobile/canPlatform.h"

#include "thread/tracelog.h"

#include "./defines.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#undef EXTENDED_LOGGING


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#include "oli/Cmd_IOBoard.h"
#include "oli/Cmd_MotCtrl.h"

#if 0
// left and right exchanged
#define CAN_ID_IO_CMD 0x0101
#define CAN_ID_IO_REPLY 0x0100
#define CAN_ID_LEFT_CMD 0x0201
#define CAN_ID_LEFT_REPLY 0x0200
#define CAN_ID_RIGHT_CMD 0x0301
#define CAN_ID_RIGHT_REPLY 0x0300
#else
// original
#define CAN_ID_IO_CMD 0x0101
#define CAN_ID_IO_REPLY 0x0100
#define CAN_ID_LEFT_CMD 0x0301
#define CAN_ID_LEFT_REPLY 0x0300
#define CAN_ID_RIGHT_CMD 0x0201
#define CAN_ID_RIGHT_REPLY 0x0200
#endif


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static const int timeout = 200;   // timeout

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CCANPLATFORM::CCANPLATFORM (void)
  : CCANDONGLE ()
{
  GTL ("CCANPLATFORM::CCANPLATFORM()");

  _driveTemperatures = new double [2];

#if MOBILEVERSION == HAMBURG

  GTLPRINT (("Create 'motor_states.out'...\n"));

  // open log file for motor states
  _datafile = fopen ("motor_states.out", "w");

  if (!_datafile)
  {
    GTLFATAL (("could not create file\n"));
    abort ();
  }

  GTLPRINT (("done\n"));

#endif

  //

  Disconnect ();

  Connect ();

  //
  // ++itschere20030527: if I call GetStatus() after Connect() the *left* motor
  // reports "not accepted" when issueing GETPOSVEL. this first happended after
  // I changed GetStatus() to use a loop like some other functions... how
  // comes???
  //
  // ++itschere20030807: does not seem to be true any more. the controller
  // update after the motor repair fixed it.
  //

  GetStatus ();

  //

  _voltageCount = -1;
  _temperatureLeft = _temperatureRight = -1;

#ifdef EXTENDED_LOGGING

  _lastVelLeft  = 0.0;
  _lastVelRight = 0.0;

  _maxDiffLeft  = 0.0;
  _maxDiffRight = 0.0;

  _lastErrLeft  = 0.0;
  _lastErrRight = 0.0;

#endif

  _emStop = false;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CCANPLATFORM::~CCANPLATFORM (void)
{
  GTL ("CCANPLATFORM::~CCANPLATFORM");

  Disconnect ();

#if MOBILEVERSION == HAMBURG

  fclose (_datafile);

#endif

  delete [] _driveTemperatures;

}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCANPLATFORM::Disconnect (void)
{
  GTL ("CCANPLATFORM::Disconnect()");

#if 0
  EnableMotorLeft (false, 0);

  DisableBrakeLeft (false);

  EnableMotorRight (false, 0);

  DisableBrakeRight (false);
#endif

  Send (CAN_ID_IO_CMD,    CMD_IOBOARD_DISCONNECT);
  Send (CAN_ID_LEFT_CMD,  CMD_MOTCTRL_DISCONNECT);
  Send (CAN_ID_RIGHT_CMD, CMD_MOTCTRL_DISCONNECT);

  sleep (1);
}


///////////////////////////////////////////////////////////////////////////////
//
// connect to controllers
//
///////////////////////////////////////////////////////////////////////////////

void CCANPLATFORM::Connect (void)
{
  GTL ("CCANPLATFORM::Connect()");

  Send (CAN_ID_IO_CMD,    CMD_IOBOARD_CONNECT);
  Send (CAN_ID_LEFT_CMD,  CMD_MOTCTRL_CONNECT);
  Send (CAN_ID_RIGHT_CMD, CMD_MOTCTRL_CONNECT);

  struct reply replies[] =
    {
      {CAN_ID_IO_REPLY,    CMD_IOBOARD_CONNECT},
      {CAN_ID_LEFT_REPLY,  CMD_MOTCTRL_CONNECT},
      {CAN_ID_RIGHT_REPLY, CMD_MOTCTRL_CONNECT}
    };

  if (ReceiveMultipleTimeout (3, replies, timeout) < 0)
    {
      GTLFATAL (("Fehler bei der Kommunikation mit dem CAN.\n"));
      abort ();
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCANPLATFORM::GetStatus (bool abortIfFatal)
{
  Lock ();

  //

  GTL ("CCANPLATFORM::GetStatus()");

  //
  //
  //

  Send (CAN_ID_IO_CMD,    CMD_IOBOARD_GETSTATUS);
  Send (CAN_ID_LEFT_CMD,  CMD_MOTCTRL_GETSTATUS);
  Send (CAN_ID_RIGHT_CMD, CMD_MOTCTRL_GETSTATUS);

  //
  //
  //

  struct reply replies[] =
    {
      {CAN_ID_IO_REPLY,    CMD_IOBOARD_GETSTATUS},
      {CAN_ID_LEFT_REPLY,  CMD_MOTCTRL_GETSTATUS},
      {CAN_ID_RIGHT_REPLY, CMD_MOTCTRL_GETSTATUS},
      {CAN_ID_IO_REPLY, CMD_IOBOARD_GETDIGIN}

    };

  if (ReceiveMultipleTimeout (4, replies, timeout) < 0)
    {
      GTLFATAL (("Fehler bei der Kommunikation mit dem CAN\n"));
    }

#if MOBILEVERSION != BIELEFELD

  //
  // ++itschere20030828: per motor reply on GETSTATUS
  //
  //  transmitMsg (event.iSource,
  //       g_iStateCtrl >> 8,
  //       g_iStateCtrl,
  //       g_iTempMeas >> 8,
  //       g_iTempMeas,
  //       0,
  //       0,
  //       0,
  //       (iCmd << 2) | Msg_OK);
  //

#if MOBILEVERSION != HAMBURG
  const int stateCtrlLeft = (replies[1].buf[0] << 8) | replies[1].buf[1];
  const int tempMeasLeft  = (replies[1].buf[2] << 8) | replies[1].buf[3];
#else
  _stateCtrlLeft = (replies[1].buf[0] << 8) | replies[1].buf[1];
  _tempMeasLeft  = (replies[1].buf[2] << 8) | replies[1].buf[3];
#endif

#if MOBILEVERSION != HAMBURG
  const int stateCtrlRight = (replies[2].buf[0] << 8) | replies[2].buf[1];
  const int tempMeasRight  = (replies[2].buf[2] << 8) | replies[2].buf[3];
#else
  _stateCtrlRight = (replies[2].buf[0] << 8) | replies[2].buf[1];
  _tempMeasRight  = (replies[2].buf[2] << 8) | replies[2].buf[3];
#endif

#if MOBILEVERSION == HAMBURG

  // ++ westhoff20040603: do plotting in MotorFeeder

#if 0

  if (_datafile)
  {
    GTLPRINT (("write to 'motor_states.out'...\n"));

    // add current time to log file
    struct timeval now;
    gettimeofday (&now, 0x0);
    struct tm *tm = localtime (&now.tv_sec);

    fprintf (_datafile,
             "#%04i/%02i/%02i-%02i:%02i:%02i.%06i\n#  LEFT : answer: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x state: %i temp: %i\n#  RIGHT: answer: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x state: %i temp: %i\n",
             tm->tm_year + 1900,
             tm->tm_mon,
             tm->tm_mday,
             tm->tm_hour,
             tm->tm_min,
             tm->tm_sec,
             (int) now.tv_usec,
             replies[1].buf[0],
             replies[1].buf[1],
             replies[1].buf[2],
             replies[1].buf[3],
             replies[1].buf[4],
             replies[1].buf[5],
             replies[1].buf[6],
             replies[1].buf[7],
             _stateCtrlLeft,
             _tempMeasLeft,
             replies[2].buf[0],
             replies[2].buf[1],
             replies[2].buf[2],
             replies[2].buf[3],
             replies[2].buf[4],
             replies[2].buf[5],
             replies[2].buf[6],
             replies[2].buf[7],
             _stateCtrlRight,
             _tempMeasRight);

    fprintf (_datafile,
             "%02i:%02i:%02i.%06i %i %i\n",
             tm->tm_hour,
             tm->tm_min,
             tm->tm_sec,
             (int) now.tv_usec,
             _tempMeasLeft,
             _tempMeasRight);

    fflush (_datafile);
  }

#endif

#endif

#endif

  //

  Unlock ();
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCANPLATFORM::GetJoyVal (void)
{
  Lock ();

  GTL ("CCANPLATFORM::GetJoyVal()");

  Send (CAN_ID_IO_CMD, CMD_IOBOARD_GETJOYVAL);

  unsigned char buf[8];

  Receive (8, buf);

  Unlock ();
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CCANPLATFORM::GetGyroVal (void)
{
  Lock ();

  GTL ("CCANPLATFORM::GetGyroVal()");

  Send (CAN_ID_IO_CMD, CMD_IOBOARD_GETGYROVAL);

  struct reply replies[] =
    {
      {CAN_ID_IO_REPLY, CMD_IOBOARD_GETGYROVAL}
    };

  if (ReceiveMultipleTimeout (1, replies, timeout) < 0)
    {
      GTLFATAL (("Fehler bei der Kommunikation mit dem CAN.\n"));
      abort ();
    }

  const short iGyro = (replies[0].buf[0] << 8) | (replies[0].buf[1]);
  short delta = iGyro - _iGyro;   // safe of wraps (must be `short')
  _iGyro = iGyro;

  Unlock ();

  return delta;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CCANPLATFORM::GetGyroValAbs (void)
{
  Lock ();

  GTL ("CCANPLATFORM::GetGyroValAbs()");

  Send (CAN_ID_IO_CMD, CMD_IOBOARD_GETGYROVAL);

  struct reply replies[] =
    {
      {CAN_ID_IO_REPLY, CMD_IOBOARD_GETGYROVAL}
    };

  if (ReceiveMultipleTimeout (1, replies, timeout) < 0)
    {
      GTLFATAL (("Fehler bei der Kommunikation mit dem CAN.\n"));
      abort ();
    }

  const short iGyro = (replies[0].buf[0] << 8) | (replies[0].buf[1]);

  Unlock ();

  return iGyro;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCANPLATFORM::DisableBrakeLeft (const bool on)
{
  GTL ("CCANPLATFORM::DisableBrakeLeft()");

  Send (CAN_ID_LEFT_CMD, CMD_MOTCTRL_DISABLEBRAKE, on);

  unsigned char buf[8];

  if (ReceiveTimeout (8, buf, 1000) < 0)
    {
      GTLFATAL (("Fehler bei der Kommunikation mit dem CAN. Möglicherweise Notaus gedrückt?\n"));
      abort ();
    }
}


void CCANPLATFORM::DisableBrakeRight (const bool on)
{
  GTL ("CCANPLATFORM::DisableBrakeRight()");

  Send (CAN_ID_RIGHT_CMD, CMD_MOTCTRL_DISABLEBRAKE, on);

  unsigned char buf[8];

  if (ReceiveTimeout (8, buf, 1000) < 0)
    {
      GTLFATAL (("Fehler bei der Kommunikation mit dem CAN. Möglicherweise Notaus gedrückt?\n"));
      abort ();
    }
}


void CCANPLATFORM::EnableMotorLeft (const bool on, const int torque)
{
  GTL ("CCANPLATFORM::EnableMotorLeft()");

  Send (CAN_ID_LEFT_CMD, CMD_MOTCTRL_ENABLEMOTOR, on, torque);

  unsigned char buf[8];

  if (ReceiveTimeout (8, buf, 1000) < 0)
    {
      GTLFATAL (("Fehler bei der Kommunikation mit dem CAN. Möglicherweise Notaus gedrückt?\n"));
      abort ();
    }
}


void CCANPLATFORM::EnableMotorRight (const bool on, const int torque)
{
  GTL ("CCANPLATFORM::EnableMotorRight()");

  Send (CAN_ID_RIGHT_CMD, CMD_MOTCTRL_ENABLEMOTOR, on, torque);

  unsigned char buf[8];

  if (ReceiveTimeout (8, buf, 1000) < 0)
    {
      GTLFATAL (("Fehler bei der Kommunikation mit dem CAN. Möglicherweise Notaus gedrückt?\n"));
      abort ();
    }
}


void CCANPLATFORM::SyncMotorLeft (const int torque)
{
  GTL ("CCANPLATFORM::SyncMotorLeft()");

  Send (CAN_ID_LEFT_CMD, CMD_MOTCTRL_SYNCHMOTOR, torque);

  unsigned char buf[8];

  if (ReceiveTimeout (8, buf, 5000) < 0)
    {
      GTLFATAL (("Fehler bei der Kommunikation mit dem CAN. Möglicherweise Notaus gedrückt?\n"));
      abort ();
    }

  int iEncPosMeas = (buf[0] << 8) | buf[1];
  int iEncPosExp = (buf[2] << 8) | buf[3];

  GTLPRINT (("enc pos meas = %i, exp = %i\n", iEncPosMeas, iEncPosExp));

  //return data[7] & 2;
}


void CCANPLATFORM::SyncMotorRight (const int torque)
{
  GTL ("CCANPLATFORM::SyncMotorRight()");

  Send (CAN_ID_RIGHT_CMD, CMD_MOTCTRL_SYNCHMOTOR, torque);

  unsigned char buf[8];

  if (ReceiveTimeout (8, buf, 5000) < 0)
    {
      GTLFATAL (("Fehler bei der Kommunikation mit dem CAN. Möglicherweise Notaus gedrückt?\n"));
      abort ();
    }

  int iEncPosMeas = (buf[0] << 8) | buf[1];
  int iEncPosExp = (buf[2] << 8) | buf[3];

  GTLPRINT (("enc pos meas = %i, exp = %i\n", iEncPosMeas, iEncPosExp));

  //return data[7] & 2;
}


void CCANPLATFORM::EnableCommLeft (void)
{
  GTL ("CCANPLATFORM::EnableCommLeft()");

  Send (CAN_ID_LEFT_CMD, CMD_MOTCTRL_ENABLECOMM);

  unsigned char buf[8];

  Receive (8, buf);
}


void CCANPLATFORM::EnableCommRight (void)
{
  GTL ("CCANPLATFORM::EnableCommRight()");

  Send (CAN_ID_RIGHT_CMD, CMD_MOTCTRL_ENABLECOMM);

  unsigned char buf[8];

  Receive (8, buf);
}


void CCANPLATFORM::StartMotors (void)
{
  GTL ("CCANPLATFORM::StartMotors()");

  const int torque = 70;

  // left

  DisableBrakeLeft (true);
  usleep (500000);

  EnableMotorLeft (true, torque);
  usleep (500000);

  SyncMotorLeft (torque);

  EnableCommLeft ();

  InitializeAngleLeft ();

  // right

  DisableBrakeRight (true);
  usleep (500000);

  EnableMotorRight (true, torque);
  usleep (500000);

  SyncMotorRight (torque);

  EnableCommRight ();

  InitializeAngleRight ();

  //

  InitializeAngleGyro ();

  //

  GetStatus ();

  //

#if 1

  int id;
  unsigned char buf[8];

  while ((id = ReceiveTimeout (8, buf, timeout)) > 0)
    {
      unsigned int error = buf[7] & 0x03;
      unsigned int cmd = buf[7] >> 2;

      GTLFATAL (("bogus reply 0x%02x from 0x%04x with error 0x%02x received\n",
		 cmd, id, error));
    }

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static const int iEncIncrPerRevMotor = 4096;
static const double dVelMeasPeriod = 500.0;
static const double dRevMotorPerRevGear = 1.0;
static const double dGearRatio = 37.0;

static const double dAngGearRadToAngEncIncr =
  iEncIncrPerRevMotor * dRevMotorPerRevGear * dGearRatio / (2.0 * M_PI);

static inline int VelGearRadS_2_VelEncIncrPeriod (double dVelGearRadS)
{
  return (int)(dVelGearRadS * dAngGearRadToAngEncIncr / dVelMeasPeriod);
}

static inline double AngEncIncrToAngGearRad (int iEncIncr)
{
  return (double)iEncIncr / dAngGearRadToAngEncIncr;
}

static inline double VelEncIncrPeriod_2_VelGearRadS (int iEncVelIncrPeriod)
{
  return (double)iEncVelIncrPeriod / dAngGearRadToAngEncIncr * dVelMeasPeriod;
}


///////////////////////////////////////////////////////////////////////////////
//
// gyro specific conversions. claims to have "2004 increments / 180°", which is
// a resolution of 0.09°.
//
///////////////////////////////////////////////////////////////////////////////

static inline double gyroEnc2Rad (const int enc)
{
  // ++itschere20031024: new "calibration" (against the odometry)

#if MOBILEVERSION == BIELEFELD
  static const double gyroScale = -1948;   // -2004.0
  static const double gyroOffset = +0.01 * 0.03125;
#elif MOBILEVERSION == MUNICH
  static const double gyroScale = -1782.5;
#elif MOBILEVERSION == HAMBURG
  // ++westhoff20040331: ??? keine Ahnung, steht sowieso noch auf FAKE_GYRO
  static const double gyroScale = -1782.5;
#else
  static const double gyroScale = ???;
#endif

#if MOBILEVERSION == BIELEFELD
  return enc * M_PI / gyroScale + gyroOffset;
#else
  return enc * M_PI / gyroScale;
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

//#undef MAGIC
#define MAGIC 0.987


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCANPLATFORM::SetVelLeft (double *vel)
{
  GTL ("CCANPLATFORM::SetVelLeft()");

#ifdef MAGIC
  int iVelEncIncrPeriod = VelGearRadS_2_VelEncIncrPeriod (-*vel / MAGIC);
#else
  int iVelEncIncrPeriod = VelGearRadS_2_VelEncIncrPeriod (-*vel);
#endif

  if (iVelEncIncrPeriod > 32767)
    {
      iVelEncIncrPeriod = -32767;
    }

  if (iVelEncIncrPeriod < -32768)
    {
      iVelEncIncrPeriod = -32768;
    }

#ifdef MAGIC
  *vel = -VelEncIncrPeriod_2_VelGearRadS (iVelEncIncrPeriod) * MAGIC;
#else
  *vel = -VelEncIncrPeriod_2_VelGearRadS (iVelEncIncrPeriod);
#endif

  //

  unsigned int hi = iVelEncIncrPeriod >> 8;
  unsigned int lo = iVelEncIncrPeriod & 0xff;

  Send (CAN_ID_LEFT_CMD,
	CMD_MOTCTRL_SETCMDVAL,
	0,
	0,
	0,
	0,
	hi,
	lo);

  //

#if MOBILEVERSION != BIELEFELD

  struct reply replies[] =
    {
      {CAN_ID_LEFT_REPLY,  CMD_MOTCTRL_GETSTATUS},
      {CAN_ID_LEFT_REPLY,  CMD_MOTCTRL_GETPOSVEL}
    };

  if (ReceiveMultipleTimeout (2, replies, timeout) < 0)
    {
      GTLFATAL (("Fehler bei der Kommunikation mit dem CAN.\n"));
      abort ();
    }

#endif
}


void CCANPLATFORM::SetVelLeft (const double vel)
{
  GTL ("CCANPLATFORM::SetVelLeft()");

#ifdef MAGIC
  int iVelEncIncrPeriod = VelGearRadS_2_VelEncIncrPeriod (-vel / MAGIC);
#else
  int iVelEncIncrPeriod = VelGearRadS_2_VelEncIncrPeriod (-vel);
#endif

  if (iVelEncIncrPeriod > 32767)
    {
      iVelEncIncrPeriod = -32767;
    }

  if (iVelEncIncrPeriod < -32768)
    {
      iVelEncIncrPeriod = -32768;
    }

  //

  unsigned int hi = iVelEncIncrPeriod >> 8;
  unsigned int lo = iVelEncIncrPeriod & 0xff;

  Send (CAN_ID_LEFT_CMD,
	CMD_MOTCTRL_SETCMDVAL,
	0,
	0,
	0,
	0,
	hi,
	lo);

  //

#if MOBILEVERSION != BIELEFELD

  struct reply replies[] =
    {
      {CAN_ID_LEFT_REPLY,  CMD_MOTCTRL_GETSTATUS},
      {CAN_ID_LEFT_REPLY,  CMD_MOTCTRL_GETPOSVEL}
    };

  if (ReceiveMultipleTimeout (2, replies, timeout) < 0)
    {
      GTLFATAL (("Fehler bei der Kommunikation mit dem CAN.\n"));
      abort ();
    }

#endif
}


void CCANPLATFORM::SetVelRight (double *vel)
{
  GTL ("CCANPLATFORM::SetVelRight()");

#ifdef MAGIC
  int iVelEncIncrPeriod = VelGearRadS_2_VelEncIncrPeriod (*vel / MAGIC);
#else
  int iVelEncIncrPeriod = VelGearRadS_2_VelEncIncrPeriod (*vel);
#endif

  if (iVelEncIncrPeriod > 32767)
    {
      iVelEncIncrPeriod = -32767;
    }

  if (iVelEncIncrPeriod < -32768)
    {
      iVelEncIncrPeriod = -32768;
    }

#ifdef MAGIC
  *vel = VelEncIncrPeriod_2_VelGearRadS (iVelEncIncrPeriod) * MAGIC;
#else
  *vel = VelEncIncrPeriod_2_VelGearRadS (iVelEncIncrPeriod);
#endif

  unsigned int hi = iVelEncIncrPeriod >> 8;
  unsigned int lo = iVelEncIncrPeriod & 0xff;

  Send (CAN_ID_RIGHT_CMD,
	CMD_MOTCTRL_SETCMDVAL,
	0,
	0,
	0,
	0,
	hi,
	lo);

  //

#if MOBILEVERSION != BIELEFELD

  struct reply replies[] =
    {
      {CAN_ID_RIGHT_REPLY,  CMD_MOTCTRL_GETSTATUS},
      {CAN_ID_RIGHT_REPLY,  CMD_MOTCTRL_GETPOSVEL}
    };

  if (ReceiveMultipleTimeout (2, replies, timeout) < 0)
    {
      GTLFATAL (("Fehler bei der Kommunikation mit dem CAN.\n"));
      abort ();
    }

#endif
}


void CCANPLATFORM::SetVelRight (const double vel)
{
  GTL ("CCANPLATFORM::SetVelRight()");

#ifdef MAGIC
  int iVelEncIncrPeriod = VelGearRadS_2_VelEncIncrPeriod (vel / MAGIC);
#else
  int iVelEncIncrPeriod = VelGearRadS_2_VelEncIncrPeriod (vel);
#endif

  if (iVelEncIncrPeriod > 32767)
    {
      iVelEncIncrPeriod = -32767;
    }

  if (iVelEncIncrPeriod < -32768)
    {
      iVelEncIncrPeriod = -32768;
    }

  unsigned int hi = iVelEncIncrPeriod >> 8;
  unsigned int lo = iVelEncIncrPeriod & 0xff;

  Send (CAN_ID_RIGHT_CMD,
	CMD_MOTCTRL_SETCMDVAL,
	0,
	0,
	0,
	0,
	hi,
	lo);

  //

#if MOBILEVERSION != BIELEFELD

  struct reply replies[] =
    {
      {CAN_ID_RIGHT_REPLY,  CMD_MOTCTRL_GETSTATUS},
      {CAN_ID_RIGHT_REPLY,  CMD_MOTCTRL_GETPOSVEL}
    };

  if (ReceiveMultipleTimeout (2, replies, timeout) < 0)
    {
      GTLFATAL (("Fehler bei der Kommunikation mit dem CAN.\n"));
      abort ();
    }

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCANPLATFORM::InitializeAngleLeft (void)
{
  GTL ("CCANPLATFORM::InitializeAngleLeft()");

  //

  Send (CAN_ID_LEFT_CMD, CMD_MOTCTRL_GETPOSVEL);

  struct reply replies[] =
    {
      {CAN_ID_LEFT_REPLY, CMD_MOTCTRL_GETPOSVEL}
    };

  ReceiveMultipleTimeout (1, replies, timeout);

  _iAngleLeft = -((replies[0].buf[0] << 24) |
		  (replies[0].buf[1] << 16) |
		  (replies[0].buf[2] << 8) |
		   replies[0].buf[3]);

  //

  GTLPRINT (("angle = %i (0x%08x)\n", _iAngleLeft, _iAngleLeft));
}


void CCANPLATFORM::InitializeAngleRight (void)
{
  GTL ("CCANPLATFORM::InitializeAngleRight()");

  //

  Send (CAN_ID_RIGHT_CMD, CMD_MOTCTRL_GETPOSVEL);

  struct reply replies[] =
    {
      {CAN_ID_RIGHT_REPLY, CMD_MOTCTRL_GETPOSVEL}
    };

  ReceiveMultipleTimeout (1, replies, timeout);

  _iAngleRight = ((replies[0].buf[0] << 24) |
		  (replies[0].buf[1] << 16) |
		  (replies[0].buf[2] << 8) |
		   replies[0].buf[3]);

  //

  GTLPRINT (("angle = %i (0x%08x)\n", _iAngleRight, _iAngleRight));
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCANPLATFORM::InitializeAngleGyro (void)
{
  Lock ();

  GTL ("CCANPLATFORM::InitializeAngleGyro()");

  //
  //
  //

#if MOBILEVERSION == BIELEFELD

  Send (CAN_ID_IO_CMD, CMD_IOBOARD_ZEROGYRO);

  {
    struct reply replies[] =
      {
	{CAN_ID_IO_REPLY, CMD_IOBOARD_GETGYROVAL}   // hmm...
      };

    ReceiveMultipleTimeout (1, replies, timeout);
  }

#endif

  //
  //
  //

  Send (CAN_ID_IO_CMD, CMD_IOBOARD_GETGYROVAL);

  {
    struct reply replies[] =
      {
	{CAN_ID_IO_REPLY, CMD_IOBOARD_GETGYROVAL}
      };

    ReceiveMultipleTimeout (1, replies, timeout);

    _iGyro = (replies[0].buf[0] << 8) | replies[0].buf[1];
  }

  GTLPRINT (("angle = %i (0x%04x)\n", _iGyro, _iGyro));

  Unlock ();
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#if 1

void CCANPLATFORM::RequestDeltaAngles (void)
{
  GTL ("CCANPLATFORM::RequestDeltaAngles()");

  Send (CAN_ID_LEFT_CMD,  CMD_MOTCTRL_GETPOSVEL);
  Send (CAN_ID_RIGHT_CMD, CMD_MOTCTRL_GETPOSVEL);
}


void CCANPLATFORM::ReceiveDeltaAngles (double &deltaAngleLeft,
				       double &deltaAngleRight)
{
  GTL ("CCANPLATFORM::ReceiveDeltaAngles()");

  bool gotLeft = false;
  bool gotRight = false;

  unsigned char buf[8];

  while (!gotLeft || !gotRight)
    {
      //

      int id = ReceiveTimeout (8, buf, 100);

      if (id < 0)
	{
	  GTLFATAL (("motors failed to reply within 100ms timeout\n"));
	  abort ();
	}

      //

      int iAngle = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];

      //unsigned int ident = buf[6];
      //unsigned int error = buf[7] & 0x03;
      unsigned int cmd = buf[7] >> 2;

      if (cmd != CMD_MOTCTRL_GETPOSVEL)
	{
	  GTLFATAL (("reply doesn't match to command\n"));
	  abort ();
	}

      int iAngleIncr;

      switch (id)
	{
	case CAN_ID_LEFT_REPLY:
	  if (!gotLeft)
	    {
	      iAngleIncr = -iAngle - _iAngleLeft;   // safe of wraps
	      _iAngleLeft = -iAngle;
	      deltaAngleLeft = AngEncIncrToAngGearRad (iAngleIncr);
	      gotLeft = true;
	    }
	  break;

	case CAN_ID_RIGHT_REPLY:
	  if (!gotRight)
	    {
	      iAngleIncr = iAngle - _iAngleRight;   // safe of wraps
	      _iAngleRight = iAngle;
	      deltaAngleRight = AngEncIncrToAngGearRad (iAngleIncr);
	      gotRight = true;
	    }
	  break;

	default:
	  GTLFATAL (("reply from unexpected controller\n"));
	  abort ();
	}
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////

void CCANPLATFORM::GetWorldState (double &deltaAngleLeft,
				  double &deltaAngleRight)
{
  GTL ("CCANPLATFORM::GetWorldState()\n");

  GTLFATAL (("this function should not be used\n"));

#if 1

  //
  // this takes about 1887+-144us...
  //

  RequestDeltaAngles ();
  ReceiveDeltaAngles (deltaAngleLeft, deltaAngleRight);

#else

  //
  // ...whereas this takes about 2993+-220us
  //

  double dummy;
  GetDeltaAngleLeft (deltaAngleLeft,  dummy);
  GetDeltaAngleLeft (deltaAngleRight, dummy);

#endif
}


#endif


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCANPLATFORM::GetDeltaAngleLeft (double &deltaAngle,
				      double &velAngle)
{
  GTL ("CCANPLATFORM::GetDeltaAngleLeft()");

  //

  Send (CAN_ID_LEFT_CMD, CMD_MOTCTRL_GETPOSVEL);

  unsigned char buf[8];

  Receive (8, buf);

  //

  int iAngle = -((buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3]);

  GTLPRINT (("angle = %i (0x%08x)\n", iAngle, iAngle));

#if 1

  int deltaEncIncr = iAngle - _iAngleLeft;   // safe of wraps

  _iAngleLeft = iAngle;

  double dAngleGearRad = AngEncIncrToAngGearRad (deltaEncIncr);

  deltaAngle = dAngleGearRad;

#else

  double dAngleGearRad = AngEncIncrToAngGearRad (iAngleEncIncr);

  static double last = 0.0;

  deltaAngle = dAngleGearRad - last;  // unsafe

  last = dAngleGearRad;

#endif

  //

  short iVelEncIncrPeriod = -((buf[4] << 8) | buf[5]);

#ifdef MAGIC
  velAngle = VelEncIncrPeriod_2_VelGearRadS (iVelEncIncrPeriod) * MAGIC;
#else
  velAngle = VelEncIncrPeriod_2_VelGearRadS (iVelEncIncrPeriod);
#endif

  //

  GTLPRINT (("deltaAngle=%f, velocity=%f\n", deltaAngle, velAngle));
}


void CCANPLATFORM::GetDeltaAngleRight (double &deltaAngle,
				       double &velAngle)
{
  GTL ("CCANPLATFORM::GetDeltaAngleRight()");

  //

  Send (CAN_ID_RIGHT_CMD, CMD_MOTCTRL_GETPOSVEL);

  unsigned char buf[8];

  Receive (8, buf);

  //

  int iAngle = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];

  GTLPRINT (("angle = %i (0x%08x)\n", iAngle, iAngle));

  int deltaEncIncr = iAngle - _iAngleRight;   // safe of wraps

  _iAngleRight = iAngle;

  double dAngleGearRad = AngEncIncrToAngGearRad (deltaEncIncr);

  deltaAngle = dAngleGearRad;

  //

  int iVelEncIncrPeriod = (buf[4] << 8) | buf[5];

#ifdef MAGIC
  velAngle = VelEncIncrPeriod_2_VelGearRadS (iVelEncIncrPeriod) * MAGIC;
#else
  velAngle = VelEncIncrPeriod_2_VelGearRadS (iVelEncIncrPeriod);
#endif

  //

  GTLPRINT (("deltaAngle=%f, velocity=%f\n", deltaAngle, velAngle));
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCANPLATFORM::SetWorldVelocity (double *velLeft,
				     double *velRight)
{
  Lock ();

  GTL ("CCANPLATFORM::SetWorldVelocity()");

  //

  if (_emStop)
    {
      *velLeft = 0.0;
      *velRight = 0.0;

      Unlock ();

      return;
    }

  //

  int iVelEncIncrPeriodLeft, iVelEncIncrPeriodRight;

#ifdef MAGIC
  iVelEncIncrPeriodLeft  = VelGearRadS_2_VelEncIncrPeriod (-*velLeft / MAGIC);
  iVelEncIncrPeriodRight = VelGearRadS_2_VelEncIncrPeriod (*velRight / MAGIC);
#else
  iVelEncIncrPeriodLeft  = VelGearRadS_2_VelEncIncrPeriod (-*velLeft);
  iVelEncIncrPeriodRight = VelGearRadS_2_VelEncIncrPeriod (*velRight);
#endif

  if (iVelEncIncrPeriodRight > 32767)
    {
      iVelEncIncrPeriodRight = -32767;
    }

  if (iVelEncIncrPeriodRight < -32768)
    {
      iVelEncIncrPeriodRight = -32768;
    }

  if (iVelEncIncrPeriodLeft > 32767)
    {
      iVelEncIncrPeriodLeft = -32767;
    }

  if (iVelEncIncrPeriodLeft < -32768)
    {
      iVelEncIncrPeriodLeft = -32768;
    }

#ifdef MAGIC
  *velLeft  = -VelEncIncrPeriod_2_VelGearRadS (iVelEncIncrPeriodLeft) * MAGIC;
  *velRight = VelEncIncrPeriod_2_VelGearRadS (iVelEncIncrPeriodRight) * MAGIC;
#else
  *velLeft  = -VelEncIncrPeriod_2_VelGearRadS (iVelEncIncrPeriodLeft);
  *velRight = VelEncIncrPeriod_2_VelGearRadS (iVelEncIncrPeriodRight);
#endif

#ifdef EXTENDED_LOGGING

  _lastVelLeft = *velLeft;
  _lastVelRight = *velRight;

#endif

  Send (CAN_ID_LEFT_CMD,
	CMD_MOTCTRL_SETCMDVAL,
	0,
	0,
	0,
	0,
	iVelEncIncrPeriodLeft >> 8,
	iVelEncIncrPeriodLeft & 0xff);

  Send (CAN_ID_RIGHT_CMD,
	CMD_MOTCTRL_SETCMDVAL,
	0,
	0,
	0,
	0,
	iVelEncIncrPeriodRight >> 8,
	iVelEncIncrPeriodRight & 0xff);

#if MOBILEVERSION != BIELEFELD

  static bool first = true;

  if (first)
    {
      first = false;
      GTLFATAL (("replies from SETCMDVAL ignored (msg only printed once)\n"));
    }

  //
  // ++itschere200307: newer protocol versions return all measurements as reply
  // to the SETCMDVAL command, but for now we just ignore them. the differences
  // between the way BIELEFELD and MUNICH read back these values (in different
  // threads) would be to great to not wait until they all speak the same
  // protocol.
  //

  struct reply replies[] =
    {
      {CAN_ID_LEFT_REPLY,  CMD_MOTCTRL_GETSTATUS},
      {CAN_ID_LEFT_REPLY,  CMD_MOTCTRL_GETPOSVEL},
      {CAN_ID_RIGHT_REPLY, CMD_MOTCTRL_GETSTATUS},
      {CAN_ID_RIGHT_REPLY, CMD_MOTCTRL_GETPOSVEL}
    };

  if (ReceiveMultipleTimeout (4, replies, timeout) < 0)
    {
      GTLFATAL (("Fehler bei der Kommunikation mit dem CAN.\n"));
      abort ();
    }

  //
  // ++itschere20030828: per motor reply on SETCMDVAL
  //
  //  transmitMsg (event.iSource,
  //       g_iStateCtrl >> 8,
  //       g_iStateCtrl,
  //       g_iTempMeas >> 8,
  //       g_iTempMeas,
  //       g_lTorqueCmd >> 8,
  //       g_lTorqueCmd,
  //       g_iDebug++,
  //       (CMD_MOTCTRL_GETSTATUS << 2) | Msg_OK);
  //
  //  transmitMsg (event.iSource,
  //       g_lPosMeas >> 24,
  //       g_lPosMeas >> 16,
  //       g_lPosMeas >> 8,
  //       g_lPosMeas,
  //       g_iVelMeas >> 8,
  //       g_iVelMeas, 0,
  //       (CMD_MOTCTRL_GETPOSVEL << 2) | Msg_OK);
  //

  const int stateCtrlLeft = (replies[0].buf[1] << 8) | replies[0].buf[2];
  const int tempMeasLeft  = (replies[0].buf[3] << 8) | replies[0].buf[4];
  const int torqueCmdLeft = (replies[0].buf[5] << 8) | replies[0].buf[6];

  const int stateCtrlRight = (replies[2].buf[1] << 8) | replies[2].buf[2];
  const int tempMeasRight  = (replies[2].buf[3] << 8) | replies[2].buf[4];
  const int torqueCmdRight = (replies[2].buf[5] << 8) | replies[2].buf[6];

  _temperatureLeft  = tempMeasLeft;
  _temperatureRight = tempMeasRight;

#endif

  //

  Unlock ();
}


///////////////////////////////////////////////////////////////////////////////
//
// CCANPLATFORM::GetWorldState()
//
// reads left/right encoder velocity, gyro velocity and battery count. returns
// the first three in arguments and the last in `_voltageCount'.
//
// takes about 3.1 milliseconds (++itschere20030828: info outdated)
//
///////////////////////////////////////////////////////////////////////////////

void CCANPLATFORM::GetWorldState (double &deltaAngleLeft,
				  double &deltaAngleRight,
				  double &deltaAngleGyro)
{
  Lock ();

  GTL ("CCANPLATFORM::GetWorldState()");

  struct timeval start;
  gettimeofday (&start, 0x0);

  //
  // request state
  //

  //
  // ++itschere200307: do not send two commands to ID_IO too quickly.
  //

  Send (CAN_ID_IO_CMD,    CMD_IOBOARD_GETGYROVAL);
  Send (CAN_ID_LEFT_CMD,  CMD_MOTCTRL_GETPOSVEL);
  Send (CAN_ID_RIGHT_CMD, CMD_MOTCTRL_GETPOSVEL);
  Send (CAN_ID_IO_CMD,    CMD_IOBOARD_GETVBATT);

  //
  // receive state
  //

  struct reply replies[] =
    {
      {CAN_ID_IO_REPLY,    CMD_IOBOARD_GETGYROVAL},
      {CAN_ID_IO_REPLY,    CMD_IOBOARD_GETVBATT},
      {CAN_ID_LEFT_REPLY,  CMD_MOTCTRL_GETPOSVEL},
      {CAN_ID_RIGHT_REPLY, CMD_MOTCTRL_GETPOSVEL}
    };

  if (ReceiveMultipleTimeout (4, replies, timeout) < 0)
    {
      GTLFATAL (("Fehler bei der Kommunikation mit dem CAN.\n"));
      abort ();
    }

  // Gyroskopmessung

  {
    const short iGyro = (replies[0].buf[0] << 8) | (replies[0].buf[1]);
    short delta = iGyro - _iGyro;   // safe of wraps (must be `short')
    _iGyro = iGyro;
    deltaAngleGyro = gyroEnc2Rad (delta);
  }

  // Spannung

  _voltageCount = (replies[1].buf[0] << 8) | (replies[1].buf[1]);

  //

  int iAngleLeft;

  {
    iAngleLeft = ((replies[2].buf[0] << 24) |
		  (replies[2].buf[1] << 16) |
		  (replies[2].buf[2] << 8) |
		  replies[2].buf[3]);

    iAngleLeft = -iAngleLeft;
    // ++itschere20031022: shortly over 0x08000000 the encoder jumps to
    // 0xf8000000, resulting in a very large delta in one cycle. try to detect
    // the jump and modify the previous value so that the delta becomes
    // reasonable again.

    if ((_iAngleLeft > 0x07000000) &&   // positive
	(iAngleLeft <= 0))
      {
	_iAngleLeft |= 0xf0000000;
	GTLFATAL (("************* JUMP CAUGHT (positive left) *************\n"));
      }
    else if ((_iAngleLeft < (int)0xf9000000) && // negative
	     (iAngleLeft >= 0))
      {
	_iAngleLeft &= ~0xf0000000;
	GTLFATAL (("************* JUMP CAUGHT (negative left) *************\n"));
      }

    int delta = iAngleLeft - _iAngleLeft;   // safe of wraps (must be `int')
    _iAngleLeft = iAngleLeft;
    deltaAngleLeft = AngEncIncrToAngGearRad (delta);
  }

  //

  int iAngleRight;

  {
    iAngleRight = ((replies[3].buf[0] << 24) |
		   (replies[3].buf[1] << 16) |
		   (replies[3].buf[2] << 8) |
		   replies[3].buf[3]);

#if 1
    if ((_iAngleRight > 0x07000000) &&   // positive
	(iAngleRight <= 0))
      {
	_iAngleRight |= 0xf0000000;
	GTLFATAL (("************* JUMP CAUGHT (positive right) *************\n"));
      }
    else if ((_iAngleRight < (int)0xf9000000) && // negative
	     (iAngleRight >= 0))
      {
	_iAngleRight &= ~0xf0000000;
	GTLFATAL (("************* JUMP CAUGHT (negative right) *************\n"));
      }
#endif
    int delta = iAngleRight - _iAngleRight;   // safe of wraps (must be `int')
    _iAngleRight = iAngleRight;
    deltaAngleRight = AngEncIncrToAngGearRad (delta);
  }

#if 0
  GTLPRINT (("absLeft = %i (0x%08x), absRight = %i (0x%08x)\n",
	     iAngleLeft, iAngleLeft,
	     iAngleRight, iAngleRight));
#endif

  //
  //
  //

  struct timeval stop;
  gettimeofday (&stop, 0x0);

  double ms =
    (stop.tv_sec - start.tv_sec) * 1000.0 +
    (stop.tv_usec - start.tv_usec) / 1000.0;

  GTLPRINT (("took %.3fms  [rad]  deltaLeft=%f  deltaRight=%f  deltaGyro=%f  voltage=%i\n",
	     ms,
	     deltaAngleLeft,
	     deltaAngleRight,
	     deltaAngleGyro,
	     _voltageCount));

  //

  Unlock ();
}


///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

#if 1

int CCANPLATFORM::GetBatteryVoltageRaw (void)
{
  Lock ();

  GTL ("CCANPLATFORM::GetBatteryVoltageRaw()");

  //

  Send (CAN_ID_IO_CMD, CMD_IOBOARD_GETVBATT);

  //

  struct reply replies[] =
    {
#if MOBILEVERSION != BIELEFELD
      {CAN_ID_IO_REPLY,    CMD_IOBOARD_GETVBATT}
#else
#warning "was für eine scheisse ist das? er antwortet mit einer falschen id???"
      {CAN_ID_IO_REPLY,    CMD_IOBOARD_GETJOYVAL}   // replies with wrong id!!!
#endif
    };

  if (ReceiveMultipleTimeout (1, replies, timeout) < 0)
    {
      GTLFATAL (("Fehler bei der Kommunikation mit dem CAN.\n"));
      return -1;
    }

  //

  Unlock ();

  return (replies[0].buf[0] << 8) | replies[0].buf[1];
}

#endif

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

double CCANPLATFORM::ConvertBatteryVoltage (const int raw) const
{
  GTL ("CCANPLATFORM::ConvertBatteryVoltage(int)");

#if MOBILEVERSION == BIELEFELD

  // return (raw - 13600) / 490.0;
  return (raw - 12500) / 510.0;

#elif MOBILEVERSION == HAMBURG

  return (-0.00000010537*raw*raw) + (0.012578*raw) - 314;

#else

  #error "needs battery conversion for this software version"

#endif
}


double CCANPLATFORM::GetBatteryVoltage (void)
{
  GTL ("CCANPLATFORM::GetBatteryVoltage()");

  if (_voltageCount < 0)
    {
      return -1.0;
    }

  return ConvertBatteryVoltage (_voltageCount);
}

double* CCANPLATFORM::GetDriveTemperatures (void)
{
  GTL ("CCANPLATFORM::GetDriveTemperatures()")

  GetStatus (false);

  Lock ();

  _driveTemperatures[0] = (double)(((-1) * _tempMeasLeft) + 500) / 4.0 + 20.0;
  _driveTemperatures[1] = (double)(((-1) * _tempMeasRight) + 500) / 4.0 + 20.0;

  Unlock();

  return _driveTemperatures;
 }


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCANPLATFORM::SetEmStop (void)
{
  Lock ();

  GTL ("CCANPLATFORM::SetEmStop()");

  //
  //
  //

  if (!_emStop)
  {
    Send (CAN_ID_LEFT_CMD,  CMD_MOTCTRL_SETEMSTOP);
    Send (CAN_ID_RIGHT_CMD, CMD_MOTCTRL_SETEMSTOP);

    //
    //
    //

    struct reply replies[] =
      {
        {CAN_ID_LEFT_REPLY,  CMD_MOTCTRL_SETEMSTOP},
        {CAN_ID_RIGHT_REPLY, CMD_MOTCTRL_SETEMSTOP}
      };

    if (ReceiveMultipleTimeout (2, replies, timeout) < 0)
      {
        GTLFATAL (("Fehler bei der Kommunikation mit dem CAN.\n"));
	abort ();
      }

    //
    //
    //

    _emStop = true;
  }

  Unlock ();
}


void CCANPLATFORM::ResetEmStop (void)
{
  Lock ();

  GTL ("CCANPLATFORM::ResetEmStop()");

  //
  //
  //

  if (_emStop)
  {
    Send (CAN_ID_LEFT_CMD,  CMD_MOTCTRL_RESETEMSTOP);
    Send (CAN_ID_RIGHT_CMD, CMD_MOTCTRL_RESETEMSTOP);

    //
    //
    //

    struct reply replies[] =
      {
        {CAN_ID_LEFT_REPLY,  CMD_MOTCTRL_RESETEMSTOP},
        {CAN_ID_RIGHT_REPLY, CMD_MOTCTRL_RESETEMSTOP}
      };

    if (ReceiveMultipleTimeout (2, replies, timeout) < 0)
      {
        GTLFATAL (("Fehler bei der Kommunikation mit dem CAN.\n"));
      }

    //
    //
    //

    _emStop = false;
  }

  Unlock ();
}
