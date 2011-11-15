//
// genBase.cc
// (C) 02/2003 by TeSche (Torsten Scherer)
// <itschere@techfak.uni-bielefeld.de>
//

// Die folgenden drei Zeilen einkommentieren, wenn Debugging-
// Informationen ausgegeben werden sollen.
//#ifdef DEBUG
//#undef DEBUG
//#endif

#include <unistd.h>
#include <assert.h>
#include "thread/tracelog.h"
#include "./genBase.h"   // defines.h
#include "./motion/rotation.h"
#include "./motion/translation.h"
#include "./motion/forward.h"
#include "./motion/move.h"

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

static int globCycle = 0;

///////////////////////////////////////////////////////////////////////////////
// Ein Piepton auf der Konsole ausgeben.
///////////////////////////////////////////////////////////////////////////////

static inline void beep (void)
{
  write (fileno (stderr), "\007", 1);
}

///////////////////////////////////////////////////////////////////////////////
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
///////////////////////////////////////////////////////////////////////////////

static inline int deadband (int arg, int bandSize)
{
  if (abs (arg) <= bandSize)
    {
      return 0;
    }

  if (arg > bandSize)
    {
      return arg - bandSize;
    }

  // else if (arg < -bandSize)

  return arg + bandSize;
}


///////////////////////////////////////////////////////////////////////////////
//
// Konstruktor
//
///////////////////////////////////////////////////////////////////////////////

CGENBASE::CGENBASE (void)
  : CPERIODICTIMER (true),
    _transVelMax (0.5),                          // maximale Translationsgeschwindigkeit (0.25 [m/s])
    _transAccMax (0.1),                          // maximale Beschleunigung (translatorisch)
    _rotVelMax (30.0 * M_PI / 180.0),            // maximale Rotationsgeschwindigkeit (30 [°/s])
    _rotAccMax (M_PI * 10.0 / 180.0)             // maximale Beschleunigung (rotatorisch)
{
  GTL ("CGENBASE::CGENBASE()");

  // Anfangsposition festlegen

  CVEC pose (3);

  pose[0] = 0.0;
  pose[1] = 0.0;
  pose[2] = 0.0;

  // Starten der alten Selbst-Lokalisierung

  _localisation = new Localisation ((long int) pose[0], 
  				    (long int) pose[1], 
  				    (long int) pose[2],
  				    "/vol/biorob/share/robot.prm",
  				    MARKFILE);

  try
    {
      // Starten der Threads zur Überwachung des CAN
      _motorFeeder = new CMOTORFEEDER (this);

      // Bremsen anziehen
      _motorFeeder->SetBrakes (true); 

      // Bremsen-Flag setzen
      _brakesEnabled = true; 

      // Starten der neuen Selbst-Lokalisierung
      _localization = new CLOCALIZATION (pose, _motorFeeder);

      // Marken aus Datei laden
      CVEC marks = _localisation->loadFeaturePositions ("/vol/biorob/share/map.marks");

      int size = marks.GetSize () / 2;

      float markX [size];
      float markY [size];

      for (int i=0; i < size; ++i)
	{
	  markX[i] = marks[2*i];
	  markY[i] = marks[(2*i)+1];
	}

      // Klasse zur Berechnung der Korrespondenzen zwischen Messung und
      // bekannten Marken
      _correspondence = new CCORRESPONDENCE (size, markX, markY);
      _correspondence->PrintKnownMarks ();

      //

      _logOdometry = false; 

      _laserFeeder = new CLASERFEEDER (this, false);
    }
  catch (...)
    {
      GTLFATAL (("failed to initialize CGENBASE\n"));
      abort ();
    }

  GTLPRINT (("localisation=%p, motorFeeder=%p, laserFeeder=%p\n",
	     _localization, _motorFeeder, _laserFeeder));

  //

  _transVelScale = 1.0;
  _rotVelScale = 1.0;
  _motion = 0x0;
  _abortCurrentMotion = false;
  _isStalled = false;
  _lastEvent.tv_sec = 0;
  _lastEvent.tv_usec = 0;
  _mode = TARGET;

  SignalIdleCondition ();

  //

  Start ();   // CPERIODICTIMER

}


///////////////////////////////////////////////////////////////////////////////
//
// Destruktor
//
///////////////////////////////////////////////////////////////////////////////

CGENBASE::~CGENBASE (void)
{
  GTL ("CGENBASE::~CGENBASE()");

  Stop ();

  delete _laserFeeder;
  delete _motorFeeder;
  //  delete _localisation;
  delete _localization;
  delete _correspondence;
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static double rampDown (const double distance)
{
  //
  // this one starts to ramp down from 1.0 at roughly 1.5, passing 0.0 at 0.7.
  //
  //return tanh (3.0 * (distance - 0.7));

  //
  // this one starts to ramp down from 1.0 at roughly 1.1, passing 0.0 at 0.6.
  //
  return tanh (7.0 * (distance - 0.6));

  //
  // this one starts to ramp down from 1.0 at roughly 1.2, passing 0.0 at 0.5.
  //
  // return tanh (5.0 * (distance - 0.5));
}

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

const microseconds_t CGENBASE::GetEventInterval (void)
{
  // Zeitstempel ermitteln

  struct timeval now;

  gettimeofday (&now, 0x0);

  // Beim ersten Mal den alten Zeitstemepl auf jetztsetzen
  if (!_lastEvent.tv_sec && !_lastEvent.tv_usec)
    {
      _lastEvent = now;
    }

  // Microsekunden seit dem letzten Zeitstempel ermitteln

  const microseconds_t us =
    (now.tv_usec - _lastEvent.tv_usec) +
    ((now.tv_sec - _lastEvent.tv_sec) > 0 ? 1000000 : 0);

  // Aktuellen Zeitstempel dür den nächsten Aufruf als alt speichern

  _lastEvent = now;

  return us;
}

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

void CGENBASE::Event (void)
{
  GTL ("CGENBASE::Event()");

  // Zeit seit dem letzten Aufruf von Event ermitteln

  const microseconds_t us = GetEventInterval ();

  // Wenn Zeit vergangen ist...

  if (us > 0)
    {
      //
      //
      //

      bool motionDone = false;
      double transVel = 0.0, rotVel = 0.0;

      _CMOTION *motion = _motion ? (_CMOTION *)_motion : 0x0;
            
      GTLPRINT (("_motion = %p\n", _motion));

      static bool avoidCollision = true;

      if (motion)
	{

	  if (motion->GetType () == _CMOTION::MOTION_FORWARD)
	    {
	      if (avoidCollision)
		{
		  GTLFATAL (("MOTION_FORWARD: switching collision avoidance off\n"));
		  avoidCollision = false;
		}
	    }
	  else if (motion->GetType () == _CMOTION::MOTION_ROTATEANGLE)
	    {
	      GTLPRINT (("MOTION_ROTATE: Kollisionsvermeidung abfragen.\n"));

	      CROTATION* rotation = (CROTATION *) motion;

	      GTLPRINT (("%p : MOTION_ROTATE: collision avoidance = %i\n",  
			 rotation,
                         rotation->_collisionAvoidance));

	      avoidCollision = rotation->_collisionAvoidance;  
	    }

	  // feed position/velocity information to motion

	  static CVEC pose;
	  static CVEC velocities (3);
	  static float v, w, vx, vy;

	  // Pose des Roboters

	  pose = _localization->GetPose ();
	  
	  // Geschwindigkeiten

	  _motorFeeder->GetVelocities (v, w);

	  vx = cos (pose[2]) * v;
	  vy = sin (pose[2]) * v;

	  velocities[0] = vx;
	  velocities[1] = vy;
	  velocities[2] = w;

	  // do we have to abort?

	  if (_abortCurrentMotion)
	    {
	      motion->Abort ();
	    }

	  // call motion step function

	  motionDone = motion->Step (pose, velocities, transVel, rotVel);

	  //
	}
      else // Es wird keine Bewegung ausgeführt.
	{
	  // Kollisionsvermeidung anschalten.
	  if (!avoidCollision)
	    {
	      GTLFATAL (("switching collision avoidance on\n"));
	      avoidCollision = true;
	    }
	}

      //
      // collision avoidance
      //

      const double maxTransVel = 0.5 * _transVelScale;
      const double maxRotVel = M_PI * 90.0 / 180.0 * _rotVelScale;

      bool hasBeenClipped = false;

      if (avoidCollision)
	{
	  //
	  // translational velocity clipping
	  //

	  if (transVel >= 0.0)   // forward
	    {
	      double dist;
	      _laserFeeder->GetClosestObstacleDistance (0, dist);

	      double limit = maxTransVel * rampDown (dist);

	      if (limit < 0.0)
		{
		  limit = 0.0;
                  _isStalled = true; 
		}
              else
                {
                  _isStalled = false;
                }

	      if (transVel > limit)
		{
		  GTLPRINT (("clipping transvel from %f to %f in forward mode because of %fm obstacle distance at front scanner\n", transVel, limit, dist));
		  transVel = limit;
		  hasBeenClipped = true;

		  // Piepton
		  beep ();
		}
	    }
	  else   // backward
	    {
	      double dist;
	      _laserFeeder->GetClosestObstacleDistance (1, dist);

	      double limit = -maxTransVel * rampDown (dist);

	      if (limit > 0.0)
		{
		  limit = 0.0;
                  _isStalled = true; 
		}
              else
                {
                  _isStalled = false;
                }

	      if (transVel < limit)
		{
		  GTLPRINT (("clipping transvel from %f to %f in backward mode because of %fm obstacle distance at rear scanner\n", transVel, limit, dist));
		  transVel = limit;
		  hasBeenClipped = true;

		  // Piepton
		  beep ();
		}
	    }

	  //
	  // rotational velocity clipping
	  //

	  if (fabs (rotVel) >= 0.0)
	    {
	      double d1, d2;
	      _laserFeeder->GetClosestObstacleDistance (0, d1);
	      _laserFeeder->GetClosestObstacleDistance (1, d2);
	      double dist = d1 < d2 ? d1 : d2;

	      double limit = maxRotVel * rampDown (dist);

	      GTLPRINT (("rotational velocity clipping: limit = %f\n", limit));

	      if (limit < 0.0)
		{
		  limit = 0.0;
                  _isStalled = true; // ++westhoff20050506
		}
              else
               {
                  _isStalled = false;
               }

	      if (rotVel < 0)
		{
		  if (rotVel < -limit)
		    {
		      GTLPRINT (("clipping rotvel from %f to %f in left mode because of %fm obstacle distance\n", rotVel, limit, dist));
		      rotVel = -limit;
		      hasBeenClipped = true;

		      // Piepton
		      beep ();
		    }
		}
	      else
		{
		  if (rotVel > limit)
		    {
		      GTLPRINT (("clipping rotvel from %f to %f in right mode because of %fm obstacle distance\n", rotVel, limit, dist));
		      rotVel = limit;
		      hasBeenClipped = true;

		      // Piepton
		      beep ();
		    }
		}
	    }

	  // so that the next CMOTION::Step() sees the clipping

	  if (motion && hasBeenClipped)
	    {
	      motion->SetVelocities (transVel, rotVel);   // clipped velocities
	    }
	}

      //
      // inverse kinematics
      //
      // `transVel' is in m/s along heading direction, `rotVel' is in rad/s.
      //

      GTLPRINT (("transVel=%f, rotVel=%f\n", transVel, rotVel));

      double velLeft = 0.0, velRight = 0.0;

#warning "TUNEABLE PARAMETER"

      {
	const double l = 0.2975;   // dist from center to wheels [m] (measured)
	const double u = 0.4770;   // circumference of wheels [m] (measured)

	// translation

	velLeft  += 2.0 * M_PI * (transVel / u);
	velRight += 2.0 * M_PI * (transVel / u);

	// rotation

	const double arcLen = rotVel * l;   // (va1/(2.0*M_PI)) * (2.0*M_PI*l)

	const double rotLeft  = -arcLen / u;   // fractions of rotations
	const double rotRight =  arcLen / u;

	const double angleLeft  = 2 * M_PI * rotLeft;   // world velocities
	const double angleRight = 2 * M_PI * rotRight;

	velLeft  += angleLeft;
	velRight += angleRight;
      }

      GTLPRINT (("wheel output vel[°/s]=%.3f,%.3f\n",
		 180.0 * velLeft / M_PI,
		 180.0 * velRight / M_PI));

      //
      // go!
      //

      const double velLeftOrig = velLeft, velRightOrig = velRight;

      // Setzen der Geschwindigkeiten für die Motoren
      _motorFeeder->SetWorldVelocity (velLeft, velRight);   // modifies arguments

      GTLPRINT (("wheel output vel[°/s]=%.3f,%.3f (granularized)\n",
		 180.0 * velLeft / M_PI,
		 180.0 * velRight / M_PI));

      // if rounding means that we've stopped a motor tell the motion about
      // it. this is absolutely necessary to detect the end of the motion.

      if (((velLeftOrig  != 0.0) && (velLeft  == 0.0)) ||
	  ((velRightOrig != 0.0) && (velRight == 0.0)))
	{
	  if (motion)
	    {
	      motion->SetMotorStopped ();   // set flag
	    }
	}

      // Setzen der Sollgeschwindigkeiten für die Motoren
      //      _localisation->setDesiredVelocities (velLeft, velRight);

      // 
      _motorFeeder->Trigger ();  

      // Poses ausgeben
      /*
      const CVEC pose = _localization->GetPose ();

      CVEC st;

      _localisation->getSystemState (st);

      static float x, y;

      x = st[8] * 0.001;
      y = st[9] * 0.001;

      printf ("%f %f %f  %f %f %f   %f %f\n", 
	      pose[0], pose[1], pose[2], 
	      x, y, st[10],
	      x - pose[0], y - pose[1]);
      */
      // Wenn die Bewegung beendet ist...
      if (motionDone)
	{
	  delete motion;

	  _motion = 0x0;

	  SignalIdleCondition ();
	}
    }

  //

  globCycle++;

  //

  GTLPRINT (("Event() done.\n"));
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CVEC CGENBASE::GetState (void)
{
  CVEC state;

  _localisation->getSystemState (state);

  return state;
}


///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

void CGENBASE::UpdateOdometry (const double deltaPosLeft,
			       const double deltaPosRight,
			       const double deltaPosGyro)
{
  GTL ("CGENBASE:UpdateOdometry()");

  /*
  //

  _localisation->Lock ();

  //

  CVEC z (deltaPosRight,
	  deltaPosLeft,
	  deltaPosGyro);   // meßgröße

  GTLPRINT (("updateOdometry: w[°/s]=(%+6.3f,%+6.3f) gyro[°/s]=%+6.3f\n",
	     180.0 * z[0] / M_PI,
	     180.0 * z[1] / M_PI,
	     180.0 * z[2] / M_PI));

#if 0 // mit Zeitmessung

	  struct timeval start;
	  gettimeofday (&start, 0x0);

	  _localisation->updateOdometry (z, _kalmanState);

	  struct timeval stop;
	  gettimeofday (&stop, 0x0);

	  int us =
	    (stop.tv_usec - start.tv_usec) +
	    ((stop.tv_sec - start.tv_sec) > 0 ? 1000000 : 0);

	  static int usSum = 0, usCnt = 0;

	  usSum += us;
	  usCnt += 1;

	  int avg = usSum / usCnt;

	  GTLFATAL (("updateOdometry took %i us, %i average\n", us, avg));

#else // ohne Zeitmessung

  _localisation->updateOdometry (z, _kalmanState);

#endif

  GTLPRINT (("updateOdometry: w[°/s]=(%+6.3f,%+6.3f)  vel[mm/s]=(%+7.2f,%+7.2f,%+6.2f)  acc[mm/s^2]=(%+7.2f,%+7.2f,%+6.2f)  pos=(%+8.2f,%+8.2f,%+7.2f)\n",
	     _kalmanState[0] * 180.0 / M_PI,
	     _kalmanState[1] * 180.0 / M_PI,
	     _kalmanState[2],
	     _kalmanState[3],
	     _kalmanState[4] * 180.0 / M_PI,
	     _kalmanState[5],
	     _kalmanState[6],
	     _kalmanState[7] * 180.0 / M_PI,
	     _kalmanState[8],
	     _kalmanState[9],
	     _kalmanState[10] * 180.0 / M_PI));

  //

  _localisation->Unlock ();
  */
}



///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

void CGENBASE::UpdateLaser (CVEC &marks)
{
  GTL ("CGENBASE:UpdateLaser()");
  /*
  _localisation->Lock ();

  CVEC x;

  int associated = _localisation->updateLaser (marks, x);

  GTLPRINT (("updateLaser: w[/s]=(%+6.3f°,%+6.3f°)  vel[/s]=(%+7.2fmm,%+7.2fmm,%+6.2f°)  acc[/s^2]=(%+7.2fmm,%+7.2fmm,%+6.2f°)  pos=(%+8.2fmm,%+8.2fmm,%+7.2f°)  associated=%i\n",
	     x[0] * 180.0 / M_PI,
	     x[1] * 180.0 / M_PI,
	     x[2],
	     x[3],
	     x[4] * 180.0 / M_PI,
	     x[5],
	     x[6],
	     x[7] * 180.0 / M_PI,
	     x[8],
	     x[9],
	     x[10] * 180.0 / M_PI,
	     associated));
  
  _localisation->Unlock ();
  */
}

// Die folgende Methode wird aus CLASERFEEDER aufgerufen

void CGENBASE::UpdateLaser (CVEC &marks, int *matched)
{
  GTL ("CGENBASE:UpdateLaser()");

  // Zeitstempel ermitteln
  static struct timeval lastUpdate;
  static struct timeval now;

  gettimeofday (&now, 0x0);

  // Beim ersten Mal den alten Zeitstempel auf jetztsetzen
  if (!lastUpdate.tv_sec && !lastUpdate.tv_usec)
    {
      lastUpdate = now;
    }

  // Microsekunden seit dem letzten Zeitstempel ermitteln

  const microseconds_t us =
    (now.tv_usec - lastUpdate.tv_usec) +
    ((now.tv_sec - lastUpdate.tv_sec) > 0 ? 1000000 : 0);

  // Aktuellen Zeitstempel dür den nächsten Aufruf als alt speichern

  lastUpdate = now;

  // ACHTUNG: Anscheinend kann es sein, dass hier negative Werte auftreten ??????
  assert (us >= 0.0);

  GTLPRINT (("Zeitintervall: %i [us]\n", us));

  // Predict der neuen Lokalisierung
  _localization->Predict (us);
  
  // Update der neuen Lokalisierung
  meter_t markX, markY;
  millimeter_t distance; 
  radiant_t angle;
  
  int numMatched = 0;
  
  int numOfMarks = marks.GetSize() >> 1; // GetSize() sollte immer ein Vielfaches von 2 liefern
  
  // Gescannte Marken der Reihe nach für einen Update-Schritt verwenden
  for (int mark=0; mark<numOfMarks; ++mark)
    {
      distance = marks[mark*2];
      angle    = marks[(mark*2)+1];
      
      if (_correspondence->MarkMatched (_localization->GetPose (),
					distance, 
					angle,
					markX, 
					markY))
	{
	  numMatched++;
	  
	  _localization->Update (distance * 0.001, 
				 angle, 
				 markX, 
				 markY);
	  
	}
    }
  
  GTLPRINT (("Marken gematched: %i/%i\n", numMatched, numOfMarks));

  // alte Lokalisierung
  /*
  _localisation->Lock ();
  
  CVEC x;
  
  const int associated = _localisation->updateLaser (marks, x, matched);
  
  GTLPRINT (("updateLaser: w[/s]=(%+6.3f°,%+6.3f°)  vel[/s]=(%+7.2fmm,%+7.2fmm,%+6.2f°)  acc[/s^2]=(%+7.2fmm,%+7.2fmm,%+6.2f°)  pos=(%+8.2fmm,%+8.2fmm,%+7.2f°)  associated=%i\n",
	     x[0] * 180.0 / M_PI,
	     x[1] * 180.0 / M_PI,
	     x[2],
	     x[3],
	     x[4] * 180.0 / M_PI,
	     x[5],
	     x[6],
	     x[7] * 180.0 / M_PI,
	     x[8],
	     x[9],
	     x[10] * 180.0 / M_PI,
	     associated));
  
  _localisation->Unlock ();
  */
}


///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

static void solve (double &x1,
		   double &x2,
		   const double a,
		   const double b,
		   const double c,
		   const double d)
{
  fprintf (stderr, "FEHLER: solve() - Diese Funktion sollte nie aufgerufen werden.\n");

  // solves a*x1+b*x2 = c*x1+d*x2 = 0
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CGENBASE::GetPos (meter_t &x, meter_t &y, radiant_t &a)
{
  GTL ("CGENBASE::GetPos()");

  CVEC pose = _localization->GetPose ();

  x = pose[0]; // Meter
  y = pose[1]; // Meter
  a = pose[2]; // Radiant

  return 0;
}


int CGENBASE::GetPosOdo (meter_t &x, meter_t &y, radiant_t &a)
{
  GTL ("CGENBASE::GetPosOdo()");

  GTLFATAL (("Diese Funktion sollte nie aufgerufen werden."));

  return -1;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CGENBASE::GetPosAndVar (meter_t &x, meter_t &y, radiant_t &a,
			     double &varXX, double &varXY,
			     double &varYX, double &varYY,
			     double &varAA)
{
  GTL ("CGENBASE::GetPosAndVar()");

  millimeter_t px = 0.0, py = 0.0;
  radiant_t pa = 0.0;;

#warning "Hier muss noch aus der neuen Lokalisierung abgefragt werden."
  GTLFATAL (("!!! KANN NICHT ABGEFRAGT WERDEN !!!\n"));

  //  _localisation->GetPos ((double) px, (double) py, (double) pa,
  //			 varXX, varXY, varYX, varYY, varAA);

  x = px / 1000.0;
  y = py / 1000.0;
  a = pa;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CGENBASE::SetPos (const meter_t x, const meter_t y, const radiant_t a)
{
  GTL ("CGENBASE::SetPos()");

  //  _localisation->setSystemStatePos (x * 1000.0, y * 1000.0, a);

  CVEC pose;

  pose.Create (3);
  pose[0] = x;
  pose[1] = y;
  pose[2] = a;

  _localization->SetPose (pose);
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CGENBASE::MotionRotateAngle (const double ae, const bool collisionAvoidance)
{
  GTL ("CGENBASE::MotionRotateAngle()");

  if (_brakesEnabled)
    {
      GTLFATAL (("Keine Bewegung möglich, Bremsen sind angezogen..\n"));
      return 1;
    }

  //

  WaitIdleCondition ();

  GTLPRINT (("is idle\n"));

  GTLPRINT (("rotating to: %f°\n", ae * 180.0 / M_PI)); // ae = -INF..INF

  // Dreh-Bewegung erzeugen und losfahren
  // ANMERKUNG: Die mit 'new' erzeugte Bewegung wird in der Methode Event gelöscht,
  //            wenn die Bewegung zu Ende ist.

  _motion = (_CMOTION *) new CROTATION (_rotVelMax * _rotVelScale,
					_rotAccMax * _rotVelScale,
					0.03125,                     // Intervall (TODO: Parameter als Konstante definieren)
					ae,
					true,                        // Fine-Tuning
					collisionAvoidance);         // Kollisionsvermeidung

  //

  UnlockIdleCondition ();

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

_CMOTION* CGENBASE::createMotionToPoint (const double x,
					 const double y,
					 const double theta,
					 const double xe, 
					 const double ye)
{
  GTL ("CGENBASE::createMotionToPoint()");

  // Wenn die Orientierung des Roboters noch nicht auf den Zielpunkt
  // zeigt (+- 5°), dann zuerst den Roboter drehen, ansonsten eine
  // gerade Bewegung auf das Ziel erzeugen

  double ae = atan2 (ye - y, xe - x);

  GTLPRINT (("Winkel zum Zeilpunkt: ae = %f°\n", ae * 180.0 / M_PI)); // ae = -PI..PI

  _CMOTION* finalMotion;

  if (fabs (theta - ae) > 5.0 * M_PI / 180.0)
    {
      GTLPRINT (("Der Roboter muss sich vor der Translation erst in Richtung Zielpunkt drehen.\n"));

      GTLPRINT (("Zuerst Rotation auf Orientierung %.3f°.\n",
		 ae * 180.0 / M_PI));
      GTLPRINT (("Dann Translation zu (%.3fm,%.3fm).\n",
		 xe, ye));

      // Speicher für zwei Bewegungen
      _CMOTION **motion = (_CMOTION **) malloc (2 * sizeof (_CMOTION *));

      _CMOTION **m = &motion[0];

      // Rotation erzeugen
      *m++ = (_CMOTION *) new CROTATION (_rotVelMax * _rotVelScale,
					 _rotAccMax * _rotVelScale,
					 0.03125,                     // Intervall (TODO: Parameter als Konstante definieren)
					 ae,
					 false,                       // Fine-Tuning
					 true);                       // Kollisionsvermeidung
					
      // Translation erzeugen
      *m++ = (_CMOTION *) new CTRANSLATION (_transVelMax * _transVelScale,
					    _transAccMax * _transVelScale,
					    0.03125,                        // Intervall (TODO: Als Konstante definieren)
					    xe,
					    ye);

      // Bewegung starten
      finalMotion = (_CMOTION *) new CMOVE (2, motion);
    }
  else
    {
      GTLPRINT (("Translation zu (%.3m,%.3m).\n",
		 xe, ye));

      // Bewegung starten
      finalMotion = (_CMOTION *) new CTRANSLATION (_transVelMax * _transVelScale,
						   _transAccMax * _transVelScale,
						   0.03125,                        // Intervall (TODO: Als Konstante definieren)
						   xe,
						   ye);
    }

  return finalMotion;
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CGENBASE::MotionTranslate (const double xe, const double ye)
{
  GTL ("CGENBASE::MotionTranslate()");

  if (_brakesEnabled)
    {
      GTLFATAL (("Keine Bewegung möglich. Bremsen nicht gelöst.\n"));
      return 1;
    }

  //

  GTLPRINT (("Warte auf das Ende einer möglicherweise gerade ausgeführten Bewegungen.\n"));

  WaitIdleCondition ();

  GTLPRINT (("Roboter ist frei.\n"));

  //

  CVEC pose = _localization->GetPose ();  // TODO: Pose nicht nur als Vektor zurückgeben

  const double x     = pose[0];
  const double y     = pose[1];
  const double theta = pose[2];

  GTLPRINT (("Translation von (%.3fm,%.3fm,%.3f°) zu (%.3fm,%.3fm)\n",
	     x, y, theta * 180.0 / M_PI,
	     xe, ye));
 
  _motion = createMotionToPoint (x, y, theta, xe, ye);

  //

  UnlockIdleCondition ();

  return 0;
}



///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CGENBASE::MotionMove (CVEC &path, double ae)
{
  GTL ("CGENBASE::MotionMove()");

  if (_brakesEnabled)
    {
      GTLFATAL (("Keine Bewegung möglich. Bremsen nicht gelöst.\n"));

      UnlockIdleCondition();

      return 1;
    }

  //
  // 'path' is supposed to consist of a number of duplets, containing 'x,y'
  // information for all intermediate points, starting with the current
  // position (as given by the time the path planner was called) and ending
  // with the final target (as given by the user). so the `path size' is:
  //

  const int pathSize = path.GetSize ();   // 4,6,8...

  if ((pathSize < 4) || (pathSize % 2))
    {
      GTLFATAL (("Länge des übergebenen Pfades ist falsch.\n"));
      return -1;
    }

  //
  // better replace the current position with where we really are. in case the
  // time of computation of the path and the time of calling move() were
  // different and we've got other clients or motions running they may be
  // different.
  //

  CVEC pose = _localization->GetPose ();

  path[0] = pose[0];
  path[1] = pose[1];

  const double theta = pose[2];

  GTLPRINT (("Fahre von (%.3fm,%.3fm,%.2f°) nach (%.3fm,%.3fm,%.2f°).\n",
	     path[0],
	     path[1],
	     theta * 180.0 / M_PI,
	     path[pathSize-2],
	     path[pathSize-1],
	     ae * 180.0 / M_PI));

  // set up list of targets for CMOVE

  const int numPoints = pathSize / 2; // 2,3,4... (including start)

  const int numTargets = numPoints - 1; // 1,2,3...  (excluding start)

  // Speicher für die Bewegungen zum Zielpunkt 
  // (Anzahl der Pfadpunkte mit Ziel plus abschließende Rotation auf Zielorientierung)

  _CMOTION **motion = (_CMOTION **) malloc ((numTargets + 1)  * sizeof (_CMOTION *));
  
  _CMOTION **m = &motion[0];

  // Bewegung aus Teilbewegungen zusammensetzen
  double a1 = theta;

  for (int i=0; i < numTargets; i++)
  {
    // Startpunkt der Teilbewegung
    const double x0 = path[2*i+0];
    const double y0 = path[2*i+1];
    const double a0 = a1;

    // Zielpunkt der Teilbewegung
    const double x1 = path[2*(i+1)+0];
    const double y1 = path[2*(i+1)+1];

    // Orientierung am Zielpunkt der Teilbewegung  (-PI..PI)
    a1 = atan2 (y1 - y0, x1 - x0);  

    // Teilbewegung erzeugen
    *m++ = createMotionToPoint (x0, y0, a0, x1, y1);
  }

  // Rotation auf Zielorientierung

  *m++ = (_CMOTION *) new CROTATION (_rotVelMax * _rotVelScale,
				     _rotAccMax * _rotVelScale,
				     0.03125,                     // Intervall (TODO: Parameter als Konstante definieren)
				     ae,
				     true,                        // Fine-Tuning
				     true);                       // Kollisionsvermeidung);

  // Bewegung starten

  _motion = (_CMOTION *) new CMOVE ((numTargets + 1), motion);

  //

  UnlockIdleCondition ();

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CGENBASE::MotionForward (const double de)
{
  GTL ("CGENBASE::MotionForward()");

  if (_brakesEnabled)
    {
      GTLFATAL (("Keine Bewegung möglich. Bremsen nicht gelöst.\n"));
      return 1;
    }

  //

  GTLPRINT (("Warte auf das Ende einer möglicherweise gerade ausgeführten Bewegungen.\n"));

  WaitIdleCondition ();

  GTLPRINT (("Roboter ist frei.\n"));

  // forward motion

  CFORWARD *forward = new CFORWARD (_transVelMax * _transVelScale,
				    _transAccMax * _transVelScale,
				    0.03125,                        // Intervall (TODO: Als Konstante definieren)
				    de);

  // Bewegung starten

  _motion = (_CMOTION *) forward;   

  //

  UnlockIdleCondition ();

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

int CGENBASE::GetNumScanners (void) const
{
  return _laserFeeder ? _laserFeeder->GetNumScanners () : 0;
}

int CGENBASE::GetScannerPosition (meter_t &x, meter_t &y, radiant_t &a, const int idx) const
{
  return _laserFeeder ? _laserFeeder->GetScannerPosition (x, y, a, idx) : -1;
}

int CGENBASE::GetLaserScan (const int idx, CRADIALSCANMATCHED &scan)
{
  GTL ("CGENBASE::GetLaserScan()");

  // ++westhoff20050808: wir wollen den Scan in Scanner-Koordinaten
  //  return _laserFeeder->GetLaserScanPlatformMatched (idx, scan);
  return _laserFeeder ? _laserFeeder->GetLaserScanScanner (idx, scan) : -1;
}

int CGENBASE::GetLaserScanPlatform (const int idx, CRADIALSCANMATCHED &scan)
{
  GTL ("CGENBASE::GetLaserScanPlatform()");

  return _laserFeeder ? _laserFeeder->GetLaserScanPlatformMatched (idx, scan) : -1;
}

///////////////////////////////////////////////////////////////////////////////
//
// GetMarks()
//
// get the current marks. in simulation mode they are unlikely to be different
// from the initial marks, but that's no reason for not reporting them.
//
///////////////////////////////////////////////////////////////////////////////

int  CGENBASE::GetMarks (CVEC &marks)
{
  GTL ("CGENBASE::GetMarks()");

  CVEC state;
  _localisation->getSystemState (state);

  int size = state.GetSize () - 11;

  marks.Create (size);

  for (int i=0; i<size; i++)
    {
      marks[i] = state[11+i] / 1000.0;
    }

  return size >> 1;
}


int CGENBASE::GetMarks (CVEC &marks, CVEC &variances)
{
  GTL ("CGENBASE::GetMarks()");

  CVEC state;
  CMAT var;

  _localisation->getSystemState (state);
  _localisation->getStateCovMatrix (var);

  const int numMarks = (state.GetSize () - 11) / 2;

  marks.Create (2 * numMarks);
  variances.Create (4 * numMarks);

  for (int i=0; i<numMarks; i++)
    {
#warning "oh boy, this is waaayyy from being optimal..."

      marks[2*i+0] = state[11+2*i+0] / 1000.0;
      marks[2*i+1] = state[11+2*i+1] / 1000.0;

      variances[i*4+0] = var[11+2*i+0][11+2*i+0];
      variances[i*4+1] = var[11+2*i+0][11+2*i+1];
      variances[i*4+2] = var[11+2*i+1][11+2*i+0];
      variances[i*4+3] = var[11+2*i+1][11+2*i+1];
    }

  return numMarks;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

double CGENBASE::GetBatteryVoltage (void)
{
  GTL ("CGENBASE::GetBatteryVoltage()");

  return _motorFeeder->GetBatteryVoltage ();
}

///////////////////////////////////////////////////////////////////////////////
//
// ++westhoff20050330
//
///////////////////////////////////////////////////////////////////////////////

double* CGENBASE::GetDriveTemperatures (void)
{
  GTL ("CGENBASE::GetDriveTemperatures()");

  return _motorFeeder->GetDriveTemperatures ();
}

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

MOBILEMODE CGENBASE::SetMode (const MOBILEMODE mode)
{
  GTL ("CGENBASE::SetMode()");

  MOBILEMODE oldMode = _mode;

  switch (mode)
    {
    case STANDBY:
      _mode = STANDBY;
      break;

    case TARGET:
      _mode = TARGET;
      break;

    default:
      GTLFATAL (("CGENBASE::SetMode(): Ungültiger oder nicht unterstützter Modus.\n"));
      return ERROR;
    }

  if ((oldMode != _mode) && _motorFeeder)
    {
      switch (_mode)
	{
	case STANDBY:
	  _motorFeeder->SetBrakes (true);
	  break;

	case TARGET:
	  _motorFeeder->SetBrakes (false);
	  break;

	default:  
	  break;
	}
    }

  return oldMode;
}

MOBILEMODE CGENBASE::GetMode (void) const
{
  return _mode;
}

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

int CGENBASE::SetBrakes (const bool enable)
{
  GTL ("CGENBASE::SetBrakes()");

  // Bremsen anziehen (hier werden gleichzeitig die Motoren stromlos gemacht)
  // oder lösen
  _motorFeeder->SetBrakes (enable);

  // Bremsen als angezogen oder gelöst markieren
  _brakesEnabled = enable;

  if (enable)
    {
      GTLPRINT (("Bremsen angezogen.\n"));
    }
  else
    {
      GTLPRINT (("Bremsen gelöst.\n"));
    }

  // Status
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

bool CGENBASE::AreBrakesEnabled (void)
{
  GTL ("CGENBASE::AreBrakesEnabled()");

  return _brakesEnabled;
}

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

bool CGENBASE::Stalled (void)
{
  return _isStalled;
}

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

bool CGENBASE::IsCompleted (void) const
{
  return !_motion;
}

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

void CGENBASE::AbortCurrentMotion (void)
{
  _abortCurrentMotion = true;
}

// Wird in clientProtocol.cc->StopMotion() nach beenden
// der Bewegung aufgerufen, um neue Bewegungen wieder zu erlauben.
void CGENBASE::EnableMotion (void)
{
  _abortCurrentMotion = false;
}

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

void CGENBASE::GetScale (double &transScale, double &rotScale) const
{
  transScale = _transVelScale;
  rotScale = _rotVelScale;
}

void CGENBASE::SetScale (const double transScale,
			 const double rotScale,
			 double &transScaleRet,
			 double &rotScaleRet)
{
  if ((transScale > 0.0) && (transScale <= 1.0))
    {
      _transVelScale = transScale;
    }
  
  if ((rotScale > 0.0) && (rotScale <= 1.0))
    {
      _rotVelScale = rotScale;
    }

  transScaleRet = _transVelScale;
  rotScaleRet = _rotVelScale;
}

void CGENBASE::ModifyScale (const double transScaleFactor,
			    const double rotScaleFactor,
			    double &transScaleOld,
			    double &rotScaleOld)
{
  transScaleOld = _transVelScale;
  rotScaleOld = _rotVelScale;

  if (transScaleFactor > 0.0)
    {
      if ((_transVelScale *= transScaleFactor) > 1.0)
	{
	  _transVelScale = 1.0;
	}
    }

  if (rotScaleFactor > 0.0)
    {
      if ((_rotVelScale *= rotScaleFactor) > 1.0)
	{
	  _rotVelScale = 1.0;
	}
    }
}

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

void CGENBASE::WaitIdleCondition (void)
{
  _idleCondition.Lock ();
  
  while (_motion)
    {
      _idleCondition.Wait ();
    }
}

void CGENBASE::UnlockIdleCondition (void)
{
  _idleCondition.Unlock ();
}

void CGENBASE::SignalIdleCondition (void)
{
  _idleCondition.Lock ();
  _idleCondition.Broadcast ();
  _idleCondition.Unlock ();
}
