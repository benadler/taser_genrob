// correspondence.cc
// (c) 07/2006, Daniel Westhoff 
// westhoff@informatik.uni-hamburg.de

// Die folgenden drei Zeilen kommentieren, wenn Debugging-
// Informationen ausgegeben werden sollen.
#ifdef DEBUG
#undef DEBUG
#endif

#include <string.h>
#include <math.h>
#include "thread/tracelog.h"
#include "correspondence.h"

////////////////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////////////

static inline float sqr (const float &a)
{
  return a*a;
}

////////////////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////////////

CCORRESPONDENCE::CCORRESPONDENCE (const int numOfKnownMarks,
				  const float* knownMarkX,
				  const float* knownMarkY)
{
  GTL ("CCORRESPONDENCE::CCORRESPONDENCE");

  // ANzahl der Marken merken
  _numOfKnownMarks = numOfKnownMarks;

  // x- und y-Koordinaten initialisieren
  _knownMarkX = new float [numOfKnownMarks];
  _knownMarkY = new float [numOfKnownMarks];

  GTLPRINT (("Kopieren Marken.\n"));

  // x-und y-Koordinaten kopieren
  memcpy (_knownMarkX, knownMarkX, numOfKnownMarks * sizeof(float)); 
  memcpy (_knownMarkY, knownMarkY, numOfKnownMarks * sizeof(float));
}

////////////////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////////////

CCORRESPONDENCE::~CCORRESPONDENCE (void)
{
  GTL ("CCORRESPONDENCE::~CCORRESPONDENCE");

  // x-  und y-Koordinaten löschen
  delete [] _knownMarkX;
  delete [] _knownMarkY;
}
 
////////////////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////////////

void CCORRESPONDENCE::PrintKnownMarks (void) const
{
  GTL("CCORRESPONDENCE::PrintKnownMarks()");

  for (int i=0; i < _numOfKnownMarks; ++i)
    {
      GTLPRINT (("Marke %03i : (%.02f, %.02f)\n", i, _knownMarkX[i], _knownMarkY[i]));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////////////

bool CCORRESPONDENCE::MarkMatched (const CVEC &pose, 
				   const millimeter_t &distance, 
				   const radiant_t &angle,
				   meter_t &markX,
				   radiant_t &markY)
{
  GTL("CCORRESSPONDENCE::MarkMatched()");

  // Pose des Roboters
  millimeter_t x = pose[0] * 1000.0;
  millimeter_t y = pose[1] * 1000.0;
  radiant_t theta = pose[2];

  //  GTLPRINT (("Pose: %f, %f, %f\n", x, y, theta));

  // Hilfsvariablen
  millimeter_t d, dx, dy;
  radiant_t a, a_105, a_095;

  // Alle Marken überprüfen
  for (int mark=0; mark < _numOfKnownMarks; ++mark)
    {
      //      GTLPRINT (("Überprüfe Marke %03i (%.02f, %.02f)\n", 
      //		 mark, 
      //		 _knownMarkX[mark], 
      //		 _knownMarkY[mark]));

      // Distanz zum Roboter berechnen 

      dx = _knownMarkX[mark] - x;
      dy = _knownMarkY[mark] - y;

#warning BERECHNUNG DER WURZEL KANN NOCH WEGOPTIMIERT WERDEN

      d = sqrt (sqr (dx) + sqr (dy));

      //      GTLPRINT (("Distanz: %f  Gemessen: %f\n", d, distance));

      // Nur weitermachen, wenn die Entfernungsmessung ähnlich der
      // geschätzen Entfernung (+- 10%) ist und die geschätzte Entfernung kleiner 
      // als 7.5m ist
      if ((d < 7500.0) && (d * 1.1 > distance) && (distance > d * 0.9))
	{
	  //GTLPRINT (("Entfernung passt zur Messung, Abstand: %.02f mm\n", fabs (d - distance)));
 
	  // Winkel vergleichen

	  a = atan2 (dy, dx) - theta;

	  //GTLPRINT (("Winkel: soll=%.05f  ist=%.05f\n", a, angle));

	  // Nur weitermachen, wenn die Winkelmessung ähnlich des geschätzten
	  // Winkels ist (+- 5°);
	  float grad5 = 5.0 * M_PI /180.0;

	  a_105 = a + grad5;
	  a_095 = a - grad5;

	  if ( ((a_105 > angle) && (angle > a_095)) 
	      || ((a_105 < angle) && (angle < a_095)) )
	  {
	    //GTLPRINT (("Winkel passt zur Messung, Abstand: %.02f°\n", fabs (a - angle) * 180.0 / M_PI));

	    GTLPRINT (("Korrespondenz: Marke: (%.1f, %.1f)  Gemessen: (%.1f, %.1f)\n",
		       _knownMarkX[mark], 
		       _knownMarkY[mark],
		       x + distance * cos (angle+theta),
		       y + distance * sin (angle+theta)
		       ));

	    // korrespondierende Marke in Metern zurückgeben
	    markX = _knownMarkX[mark] * 0.001;
	    markY = _knownMarkY[mark] * 0.001;
	    
#warning ES KÖNNTE WEITER BESSERE MARKEN GEBEN

	    return true;
	  }
	}
    }

  return false;
}
