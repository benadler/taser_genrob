#ifndef TRANSLATION_H
#define TRANSLATION_H

#include <QPointF>
#include <QLineF>

#include "motion.h"

/// @brief This class represents a translation.

class Translation : public Motion
{
	Q_OBJECT

	private:
		float distance;			///< The overall distance of our translation, set in c'tor
		float minDistanceToTarget;	///< The smallest distance to the translation's target we've ever encountered while moving
		QPointF target;			///< Where we want to go
		Pose start;			///< Where this translation started

	public:
		/// Constructor.
		Translation(const float distance);

		/// This method returns the motion's type
		/// @return the motion's type
		MotionType getType(void) const;

		bool isFinished(void);

		/// This method is called in subclasses. It gets a pose and is supposed to set the wheelSpeeds.
		bool step(const Pose &pose, float &speedL, float &speedR);

		/// See base class Motion for documentation
		QList<QPointF> getPathPoints(const float lookAheadDistance = -1.0, const float lookAheadInterval = -1.0) const;
};

#endif
