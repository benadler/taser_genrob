#ifndef ROTATION_H
#define ROTATION_H

#include "motion.h"

/// @brief This class represents a rotation.

class Rotation : public Motion
{
	Q_OBJECT

	private:
		float orientationEnd;

	public:
		/// Constructor.
		Rotation(const float angleRad);

		/// Destructor.
		~Rotation();

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
