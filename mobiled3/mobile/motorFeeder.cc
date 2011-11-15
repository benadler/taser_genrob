//
//
//

// Die folgenden drei Zeilen kommentieren, wenn Debugging-
// Informationen ausgegeben werden sollen.
#ifdef DEBUG
#undef DEBUG
#endif

#include <unistd.h>
#include "thread/tracelog.h"
#include "./genBase.h"
#include "./defines.h"

//#ifdef TEMP_PLOT
//#undef TEMP_PLOT
//#endif
#define TEMP_PLOT

///////////////////////////////////////////////////////////////////////////////
//
// Berechnet das Vorzeichen eines Wertes.
//
// Antwort: -1.0, wenn Wert kleiner 0, sonst +1.0
//
///////////////////////////////////////////////////////////////////////////////

static inline double fsgn (const double arg)
{
  if (arg < 0.0)
    {
      return -1.0;
    }

  return 1.0;
}


///////////////////////////////////////////////////////////////////////////////
//
// Konstruktor
//
///////////////////////////////////////////////////////////////////////////////

CMOTORFEEDER::CMOTORFEEDER (CGENBASE *genBase)
{
  GTL ("CMOTORFEEDER::CMOTORFEEDER()");

  _genBase = genBase;

  // Rotations- und Translationsgeschwindigkeit initialisieren
  _transVel = 0.0;
  _rotVel = 0.0;

  // Motoren starten;
  _canPlatform.StartMotors ();

  // Thread starten
  Start ();
}


///////////////////////////////////////////////////////////////////////////////
//
// Destruktor
//
///////////////////////////////////////////////////////////////////////////////

CMOTORFEEDER::~CMOTORFEEDER (void)
{
  GTL ("CMOTORFEEDER::~CMOTORFEEDER()");

  Stop ();
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CMOTORFEEDER::Trigger (void)
{
  _trigger.Lock ();
  _trigger.Signal ();
  _trigger.Unlock ();
}

///////////////////////////////////////////////////////////////////////////////
//
// Liefert die Werte des Joystick-Empfängers.
//
///////////////////////////////////////////////////////////////////////////////

void CMOTORFEEDER::GetJoyValues (int &valX, int &valY)
{
  valX = _joyValX;
  valY = _joyValY;
}

///////////////////////////////////////////////////////////////////////////////
//
// Liefert die Werte der Batteriespannung.
//
///////////////////////////////////////////////////////////////////////////////

double CMOTORFEEDER::GetBatteryVoltage (void)
{
  return _canPlatform.GetBatteryVoltage ();
}

///////////////////////////////////////////////////////////////////////////////
//
// Liefert die Werte der Batteriespannung.
//
///////////////////////////////////////////////////////////////////////////////

double* CMOTORFEEDER::GetDriveTemperatures (void)
{
  return _canPlatform.GetDriveTemperatures ();
}
 

///////////////////////////////////////////////////////////////////////////////
//
// Setzt die Geschwindigkeiten des linken und rechten Antriebsrades.
//
///////////////////////////////////////////////////////////////////////////////

void CMOTORFEEDER::SetWorldVelocity (double &velLeft,
				     double &velRight)
{
  GTL ("CMOTORFEEDER::SetWorldVelocity()");

  GTLPRINT (("velLeft=%f, velRight=%f (before conversion)\n", velLeft, velRight));

  _canPlatform.SetWorldVelocity (&velLeft, &velRight);

  GTLPRINT (("velLeft=%f, velRight=%f (after conversion)\n", velLeft, velRight));
}

///////////////////////////////////////////////////////////////////////////////
//
// Setzten und lösen der Bremsen.
//
///////////////////////////////////////////////////////////////////////////////

void CMOTORFEEDER::SetBrakes (const bool enable)
{
  GTL ("CMOTORFEEDER::SetBrakes()");

  if (enable)
    {
      _canPlatform.SetEmStop ();
    }
  else
    {
      _canPlatform.ResetEmStop ();
    }
}

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

void CMOTORFEEDER::GetVelocities (float &transVel, float &rotVel) const
{
  GTL ("CMOTORFEEDER::GetVelocities()");

  transVel = _transVel;
  rotVel = _rotVel;
}

///////////////////////////////////////////////////////////////////////////////
//
// CMOTORFEEDER::Fxn()
//
// separate thread to read in odometry data (encoders and gyro) and update()
// the kalman filter. with the CAN bus interface requesting odometry data can
// be done separately from sending (velocity) commands, which allows the update
// to run at a higher rate than the path interpolator.
//
// currently it is still triggered from the path interpolator and therefore
// still runs with the same rate. the reason for this is that we have to ensure
// that the update() fits to the last predict(), which is done by the path
// interpolator. future improvements include the idea to have an implicit
// predict() inside the update(), so that each update() by definition fits to
// the predicted state the filter is in and we don't have to take care of this
// any more.
//
///////////////////////////////////////////////////////////////////////////////

void CMOTORFEEDER::Fxn (void)
{
  GTL ("CMOTORFEEDER::Fxn()");

  GTLPRINT (("pid=%i\n", getpid ()));

  ConfirmRunning ();

  ScheduleRealtime (10);

  nice (-10);   // for `top' statistics only, not used in rt-scheduling

  //

  struct timeval last = {0, 0};

  //

  while (!_stopRequested)
    {
      _trigger.Lock ();

      if (!_trigger.WaitTimeout (100))
	{
	  _trigger.Unlock ();
	  continue;
	}

      _trigger.Unlock ();

      GTLPRINT (("motor feeder triggered\n"));

      //
      // the following sequence takes about 2.6ms (2.1?), basically waiting for
      // pakets on the CAN bus and therefore not blocking the rest of the
      // system. it does, however, block the CAN bus.
      //

      double deltaAngleRight, deltaAngleLeft, deltaAngleGyro;

      _canPlatform.GetWorldState (deltaAngleLeft,   // in rad
				  deltaAngleRight,  // in rad
				  deltaAngleGyro);  // in rad

      //

      struct timeval now;

      gettimeofday (&now, 0x0);

      if (last.tv_sec || last.tv_usec)   // except for the first cycle
	{
	  // vergangene Zeit seit dem letzten Aufruf bestimmen (in Sekunden)
	  const double t =
	    (now.tv_sec - last.tv_sec) +
	    (now.tv_usec - last.tv_usec) / 1000000.0;

	  // Messintervall ausgeben (sollte etwa 32ms betragen)

	  GTLPRINT (("Messintervall: %.6f [s]\n", t));

	  // Rotationsgeschwindigkeit des linken und rechten Rades bestimmen
	  // in [rad/s]
	  // TODO: Braucht vielleicht nicht extra berechnet zu werden!

	  const double velLeft = deltaAngleLeft / t;   
	  const double velRight = deltaAngleRight / t; 

	  // Translationsgeschwindigkeit des linken und rechten Rades bestimmen

#warning "Radius der Antriebsraeder ist hard-kodiert (74mm/74mm)."
 
	  const double transVelLeft  = 0.998 * 0.074 * deltaAngleLeft / t; 
	  const double transVelRight = 1.002 * 0.074 * deltaAngleRight / t; 

	  // Berechnung der Translationsgeschwindigkeit der Platform

	  _transVel = (transVelRight + transVelLeft) / 2.0;

	  // Berechnung der Rotationsgeschwindigkeit der Platform in [rad/s]

#warning "Durchmesser der Antriebsachse des Roboters ist hard-kodiert (603mm)."

	  _rotVel = (transVelRight - transVelLeft) / 0.602; // nach UMBmark

	  // Rotationsgeschwindigkeit der Plattform anhand der Gyroskopmessung berechnen

#warning "Geschwindigkeit des Gyro wird spaeter 'gefaked'."

	  double velGyro = deltaAngleGyro / t;

	  // Geschwindigkeiten ausgeben

	  GTLPRINT (("Platformgeschwindigkeiten: transVel = %.3f [m/s],  rotVel = %.3f [rad/s]  = %.3f [Grad/s]\n",
		     _transVel,
		     _rotVel,
		     _rotVel * 180.0 / M_PI));

	  //
	  //
	  //

#ifdef FAKE_GYRO

	  static double scaleMoveSum = 0.0;
	  static double scaleMoveCnt = 0.0;
	  static double absStillSum = 0.0;
	  static double absStillCnt = 0.0;

	  const double velLoc = _genBase->GetState () [4];

	  if (fabs (velLoc) < 0.1 * M_PI / 180.0)   // standing still
	    {
	      absStillSum += velGyro;
	      absStillCnt += 1.0;
	    }
	  else if (fabs (velLoc) > 45.0 * M_PI / 180.0)   // moving
	    {
	      double scale = velGyro / velLoc;
	      scaleMoveSum += scale;
	      scaleMoveCnt += 1.0;
	    }

	  GTLPRINT (("gyro calibration: offset=%f, scale=%f\n",
		     absStillSum / absStillCnt,
		     scaleMoveSum / scaleMoveCnt));

	  //

	  velGyro = velLoc;

	  GTLPRINT (("t[s]=%.3f, motor: delta[°]=%.3f,%.3f vel[°/s]=%.3f,%.3f  gyro: vel[°/s]=%.3f (faked)\n",
		     t,
		     180.0 * deltaAngleLeft / M_PI,
		     180.0 * deltaAngleRight / M_PI,
		     180.0 * velLeft / M_PI,
		     180.0 * velRight / M_PI,
		     180.0 * velGyro / M_PI));

#else

	  GTLPRINT (("t[s]=%.3f, motor: delta[°]=%.3f,%.3f vel[°/s]=%.3f,%.3f  gyro: delta[°]=%.3f vel[°/s]=%.3f\n",
		     t,
		     180.0 * deltaAngleLeft / M_PI,
		     180.0 * deltaAngleRight / M_PI,
		     180.0 * velLeft / M_PI,
		     180.0 * velRight / M_PI,
		     180.0 * deltaAngleGyro / M_PI,
		     180.0 * velGyro / M_PI));

#endif

	  _genBase->UpdateOdometry (velLeft, velRight, velGyro);

#if MOBILEVERSION == HAMBURG

	  // ++westhoff20041125: log motor velocities
          // ++westhoff20050202: Started or stoped through TCP/IP protocol
          // ++westhoff20050208: Date and time attached to name of logfile,
          //                     time in logfile in milliseconds

          static bool isLogging;
          static FILE *fp;

	  if (_genBase->_logOdometry)
	    {
	      if (!fp)
		{
                   isLogging = true;

                   // get time and date
                   struct timeval now;
                   gettimeofday (&now, 0x0);
                   struct tm *tm = localtime (&now.tv_sec);

                   // create name of logfile
                   char filename [60];

                   sprintf (filename,
                            "/usr/local/src/service_robot/log/odometry-%04i%02i%02i-%02i%02i%02i.out",
                            tm->tm_year + 1900,
                            tm->tm_mon + 1,
                            tm->tm_mday,
                            tm->tm_hour,
                            tm->tm_min,
                            tm->tm_sec);

                   // open logfile
	  	   fp = fopen (filename, "a");
   	        }

	      if (fp)
		{
                  // get time
		  struct timeval now;
		  gettimeofday (&now, 0x0);

                  long long msec = (long long) now.tv_sec * 1000
                                   + (long long) (now.tv_usec / 1000);

                  // write to log
		  fprintf (fp,
			   "%Ld\t%f\t%f\t%f\t%f\t%f\n",
			   msec,
                           180.0 * velLeft / M_PI,
			   180.0 * velRight / M_PI,
			   _genBase->GetState () [8],
			   _genBase->GetState () [9],
			   (_genBase->GetState() [10]) * 180.0 / M_PI);
		  fflush (fp);
		}
	    }
          else
          {
            if (isLogging)
            {
              isLogging = false;

              fclose (fp);
              fp = 0x0;
           }
          }
#endif

	}

      last = now;
    }
}
