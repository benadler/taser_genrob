// Localisation.cc
// (c) 06/2006, Daniel Westhoff 
// westhoff@informatik.uni-hamburg.de

// Die folgenden drei Zeilen kommentieren, wenn Debugging-
// Informationen ausgegeben werden sollen.
#ifdef DEBUG
#undef DEBUG
#endif

#include <assert.h>
#include "thread/tracelog.h"
#include "localization.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Berechnet das Quadrat der Zahl 'a'
//

static inline float sqr (const float &a)
{
  return a*a;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Konstruktor
//

CLOCALIZATION::CLOCALIZATION (const CVEC pose, const CMOTORFEEDER* motorFeeder)
{
  GTL ("CLOCALIZATION::CLOCALIZATION()");

  // Zugriff auf den CAN
  _motorFeeder = motorFeeder;

  // Initale Pose für denRoboter festlegen
  _pose = new CPOSE ((meter_t) pose[0], 
		     (meter_t) pose[1], 
		     (radiant_t) pose[2]);

  // Kovariance-Matrix
  InitCovariance ();
}

////////////////////////////////////////////////////////////////////////////////////////////
// Destruktor
//

CLOCALIZATION::~CLOCALIZATION (void)
{
  GTL ("CLOCALIZATION::~CLOCALIZATION()");

  delete _pose;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Setzt die Position und Orientierung des Roboters neu.
//

void CLOCALIZATION::SetPose (const CVEC pose)
{
  // Mutex
  Lock ();

  // Initale Pose für denRoboter festlegen
  _pose = new CPOSE ((meter_t) pose[0],
		     (meter_t) pose[1], 
		     (radiant_t) pose[2]);

  // Kovarianzmatrix initialisieren
  InitCovariance ();

  // Mutex
  Unlock ();
}

////////////////////////////////////////////////////////////////////////////////////////////
// Liefert die aktuelle Schätzung der Position und Orientierung des Roboters.
//

const CVEC CLOCALIZATION::GetPose (void)
{
  return _pose->getAsVector();
}

////////////////////////////////////////////////////////////////////////////////////////////
// Vorhersage der Position und Orientierung. 
//

void CLOCALIZATION::Predict (const microseconds_t dt)
{
  GTL ("CLOCALIZATION::Predict()");

  // Mutex
  Lock ();

  // Zeitintervall in Sekunden umrechnen
  const float dt_in_sec = (float) dt * 0.000001;

  GTLPRINT (("Zeitintervall: %i [us]; %f [s]\n", dt, dt_in_sec));
  
  // Translationsgeschwindigkeit (v) in [m/s] und Rotations-
  // geschwindigkeit (w) in [rad/s] des Roboters 
  static float v, w;  
  
  // v ist translatorische geschwindigkeit
  // w ist rotatorische geschwindigkeit
  _motorFeeder->GetVelocities (v, w);
  
  // Geschwindigkeiten ausgeben
  GTLPRINT (("Geschwindigkeiten: v=%.08f [m/s], w=%.08f [rad/s]\n", v, w));

  // vector x = (x, y, theta);
  const float theta = _pose->theta;

  // f (x,u)
  const float cos_theta = cos (theta);
  const float sin_theta = sin (theta);

  assert (dt_in_sec >= 0.0);

  const float dist = v * dt_in_sec;
  const float gamma = w * dt_in_sec;

  GTLPRINT (("Distance: %f [m]  Winkel: %f [rad]\n", dist, gamma)); 

  // errechnen der neuen position
  // 1. formel
  _pose->x += dist * cos_theta;
  _pose->y += dist * sin_theta;
  _pose->theta += gamma;

  _pose->theta -= 2*M_PI * floor(_pose->theta/(2*M_PI));      // 0..2*PI
  if (_pose->theta > M_PI) _pose->theta -= 2*M_PI;            // -PI..+PI

  // A, i.e. Df/dx
  // A beschreibt die Weiternetwicklung des Systems, A verschiebt und dreht die Gaussglocke
  double A[3][3];
  A[0][0] = 1;
  A[0][1] = 0;
  A[0][2] = -dist * sin_theta;
  A[1][0] = 0;
  A[1][1] = 1;
  A[1][2] = dist * cos_theta;
  A[2][0] = 0;
  A[2][1] = 0;
  A[2][2] = 1;

  // PNew = APA^T
  double PNew[3][3];	// Kovarianzmatrix, beschreibt die Gaussglocke, ihre Varianz und die Pose, d.h. deren Zentrum
  for (int i=0; i<3; i++)
    {
      for (int j=0; j<3; j++)
	{
	  PNew[i][j] = 0;
	  for (int k=0; k<3; k++)
	    {
	      for (int l=0; l<3; l++)
		{
		  //_P ist die Kovarianzmatrix vom vorherigen Zeitpunkt
		  // Rechnung ist eine Näherung, deswegen ist die 
		  PNew[i][j] += A[i][k] * _P[k][l] * A[j][l];
		}
	    }
	}
    }

  // B, i.e. df/du
#warning "Parameter hard-kodiert"

  // B beschreibt, wie gut A mit der Wirklichkeit übereinstimmt, bzw. den Fehler, den man durch Verwendung von A bekommt.
  double B[3][2];
  B[0][0] = cos_theta * 0.037;
  B[0][1] = cos_theta * 0.037;
  B[1][0] = sin_theta * 0.037;
  B[1][1] = sin_theta * 0.037;
  B[2][0] = - 0.074 / 0.6;
  B[2][1] = 0.074 / 0.6;

  // Q: noise in wheel measurement
  double Q[2][2];
  Q[0][0] = sqr (0.05 * 0.074); // small(!) numbers, which is why we shouldn't call predict() in too short intervals
  Q[0][1] = 0;
  Q[1][0] = 0;
  Q[1][1] = sqr (0.05 * 0.074);

  // PNew += BQB^t, "^t" heisst transponiert.
  for (int i=0; i<3; i++)
    {
      for (int j=0; j<3; j++)
	{
	  for (int k=0; k<2; k++)
	    {
	      for (int l=0; l<2; l++)
		{
			// Add noise to kovarianzmatrix.
			PNew[i][j] += B[i][k] * Q[k][l] * B[j][l];
		}
	    }
	}
    }

  // _P = PNew
  for (int i=0; i<3; i++)
    {
      for (int j=0; j<3; j++)
	{
	  _P[i][j] = PNew[i][j];
	}
    }  

  // P muss positiv semidefinit sein: Symmetrisch an der Diagonalen und die Werte müssen >0 sein.
  // Minimum für Diagonalwerte überprüfen
  if (_P[0][0] < 0.00001) _P[0][0] = 0.00001;
  if (_P[1][1] < 0.00001) _P[1][1] = 0.00001;
  if (_P[2][2] < 0.00001) _P[2][2] = 0.00001;

  // Kovarianzmatrix nicht-negativ halten
  if (_P[0][1] < 0.0) _P[0][1] = 0.0;
  if (_P[0][2] < 0.0) _P[0][2] = 0.0;
  if (_P[1][2] < 0.0) _P[1][2] = 0.0;

  // Kovarianzmatrix symmetrisch halten
  _P[1][0] = _P[0][1];
  _P[2][0] = _P[0][2];
  _P[2][1] = _P[1][2];

  // Pose ausgeben
  GTLPRINT (("Pose: x=%.06f [m], y=%.06f [m], theta=%.03f [°] **  varX=%f [m²], varY=%f [m²], varT=%f [rad²]\n",
	     _pose->x,
	     _pose->y,
	     _pose->theta * 180.0 / M_PI,
	     _P[0][0],
	     _P[1][1],
	     _P[2][2]));
  
  assert ((_P[0][0] >= 0.0) && (_P[1][1] >= 0.0) && (_P[2][2] >= 0.0));

  // Mutex
  Unlock ();
}

////////////////////////////////////////////////////////////////////////////////////////////
// Positions- und Orientierungsschätzung durch Messung aktualisieren.
//


// Predict() nimmt die aktuelle Pose und sagt die Position zu einem Zeitpunkt in der Zukunft voraus.
// Update() nimmt Messungen und 
// Update() wird für jede gemessene Lasermarke aufgerufen () die ersten beiden parameter).
//    Ausserdem werden die Koordinaten einer auf der Map gefundenen Marke, die dazu passt, mit übergeben (markx/marky).
void CLOCALIZATION::Update (meter_t measuredDistanceToMark, 
			    radiant_t measuredAngleToMark,
			    meter_t markX,
			    meter_t markY)
{
  GTL ("CLOCALIZATION::Update()");

  // Mutex
  Lock ();

  float x = _pose->x;
  float y = _pose->y;
  float theta = _pose->theta;

  float dx = markX - x;
  float dy = markY - y;

  // 'estimated' ist die Schätzung der Messung 
  // "unter welchem winkel müsste ich die marke eigentlich sehen?"
  float estimated = atan2 (dy, dx) - theta;

  GTLPRINT (("Messung: measured=%0.3f [°], estimated=%0.3f [°]\n", 
	    measuredAngleToMark * 180.0 / M_PI,
	    estimated * 180.0 / M_PI));

  // H: dh/dx
  // H ist die 
  float H[3];
  // r2, und somit das komplette H beschreiben die Messgüte
  float r2 = sqr (dx) + sqr (dy);
  H[0] = dy / r2;
  H[1] = - dx / r2;
  H[2] = -1;

  // Fehler normalisieren
  // error ist unterschied zwischen vorhergesagter und tatsächlicher messung
  float  error = measuredAngleToMark - estimated;
  error -= 2*M_PI * floor(error/(2*M_PI));      // 0..2*PI
  if (error > M_PI) error -= 2*M_PI;            // -PI..+PI

  GTLPRINT (("Innovation: %.06f [°]\n", error * 180.0 / M_PI));

  // Anschließend werden Messwert und Prediction fusioniert. Es werden nur
  // die Winkel zu den Marken verwendet, ist genau genug.
  // PH^T
  double PHt[3];
  for (int i=0; i<3; i++)
    {
      PHt[i] = 0;
      for (int k=0; k<3; k++) PHt[i] += _P[i][k] * H[k];
    }

  // HPH^T
  double innovCov = 0;
  for (int k=0; k<3; k++) innovCov += H[k] * PHt[k];

  // +R
  innovCov += sqr (5 * M_PI / 180.0);

  // ()^-1
  float innovCovInv = 1/innovCov;

  // Kalman-Gain, der Gewichtungsfaktor zwischen der Messung und der Prediction.
  float K[3];
  // innovCovInv ist die invertierte 1x1-Kovarianzmatrix der Innovation.
  // Würde man die Entfernungen zu den Marken hinzurechnen, hätte man eine 2x2 Matrix.
  for (int i=0; i<3; i++) K[i] = PHt[i] * innovCovInv;

  // _P: _P -= KPH^T
  for (int i=0; i<3; i++)
    for (int j=0; j<3; j++)
      _P[i][j] -= K[i] * PHt[j];

  // _pose: _pose += K * error
  _pose->x += K[0] * error;
  _pose->y += K[1] * error;
  _pose->theta += K[2] * error;

  _pose->theta -= 2*M_PI * floor(_pose->theta/(2*M_PI));      // 0..2*PI
  if (_pose->theta > M_PI) _pose->theta -= 2*M_PI;            // -PI..+PI

  
  // Minimum für Diagonalwerte überprüfen
  if (_P[0][0] < 0.00001) _P[0][0] = 0.00001;
  if (_P[1][1] < 0.00001) _P[1][1] = 0.00001;
  if (_P[2][2] < 0.00001) _P[2][2] = 0.00001;

  // Kovarianzmatrix positiv halten
  if (_P[0][1] < 0.0) _P[0][1] = 0.0;
  if (_P[0][2] < 0.0) _P[0][2] = 0.0;
  if (_P[1][2] < 0.0) _P[1][2] = 0.0;

  // Kovarianzmatrix symmetrisch halten
  _P[1][0] = _P[0][1];
  _P[2][0] = _P[0][2];
  _P[2][1] = _P[1][2];
  

  // Pose ausgeben
  GTLPRINT (("Pose: x=%.03f [m], y=%.03f [m], theta=%.01f [°]   varX=%f [mm²], varY=%f [mm²], varT=%f [rad²]\n",
	     _pose->x,
	     _pose->y,
	     _pose->theta * 180.0 / M_PI,
	     _P[0][0],
	     _P[1][1],
	     _P[2][2]));

  assert ((_P[0][0] >= 0.0) && (_P[1][1] >= 0.0) && (_P[2][2] >= 0.0));

  // Mutex
  Unlock ();
}

////////////////////////////////////////////////////////////////////////////////////////////
// Winkel zwischen -180° und +180° halten.
//

void CLOCALIZATION::ClipAngle (radiant_t &angle)
{
  // Winkel im Intervall [-PI..PI] halten
  if (angle >= M_PI) 
    {
      angle -= (2 * M_PI);
    }
  else if (angle < (-1.0 * M_PI))
    {
      angle += (2 * M_PI);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////
// Covarianzmatrix initialisieren
//

void CLOCALIZATION::InitCovariance (void)
{
  // Kovariance-Matrix
  _P[0][0] = 0.1; 
  _P[0][1] = 0.0;
  _P[0][2] = 0.0;
  _P[1][0] = 0.0;
  _P[1][1] = 0.1;
  _P[1][2] = 0.0;
  _P[2][0] = 0.0;
  _P[2][1] = 0.0;
  _P[2][2] = 0.1;  
}
