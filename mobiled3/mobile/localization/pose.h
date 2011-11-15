#ifndef POSE_H
#define POSE_H

#include "iotypes.h"
#include "math/vec.h"

/**
 * @ingroup Localization
 * @brief Pose eines mobilen Roboters.
 *
 * Die Pose eines mobilen ist das Tripel ais x-, y-Koordinate und Orientierung.
 *
 * @section author Author
 *   Daniel Westhoff (westhoff@informatik.uni-hamburg.de)
 * @section date Copyright
 *   (c) 2000-2002
 */
class CPOSE 
{
	public:
		double x;	///< x-Koordinate der Pose in Meter.
		double y;	///< y-Koordinate der Pose in Meter.
		double theta;	///< Orientierung der Pose in Radiant.
  
		///
		/// Konstruktor.
		///
		CPOSE(const meter_t xKoord, const meter_t yKoord, const radiant_t thetaOrient);

		///
		/// Destruktor.
		///
		virtual ~CPOSE(void);

		////////////////////////////////////////////////////////////////////////////////////////////
		/// Liefert die Pose as CVEC.
		////////////////////////////////////////////////////////////////////////////////////////////
		CVEC getAsVector(void);
};

#endif
