#ifndef MOTION_H
#define MOTION_H

#include "configuration.h"
#include "localization.h"
#include "logger.h"

/// @brief This class is an interface for any motion

enum MotionType
{
		spline,
		rotation,
		translation
};

class Motion : public QObject
{
	Q_OBJECT

	protected:
		Configuration* config;
		Logger* logger;
		bool finished;

	public:
		/// Constructor.
		Motion(void);

		/// Destruktor.
		// http://de.wikibooks.org/wiki/C++-Programmierung:_Polymorphie
		virtual ~Motion(void);

		/// This method returns the motion's type. See enum MotionType in this file.
		/// @return the motion's type
		virtual MotionType getType(void) const = 0;

		/// This method is called in subclasses. It gets a pose and is supposed to set the wheelSpeeds.
		/// @param pose the robot's current pose
		/// @param speedL the method will write the desired speed into this reference.
		/// @param speedR the method will write the desired speed into this reference.
		/// @return true when the motion is not finished, false when it is.
		virtual bool step(const Pose &pose, float &speedL, float &speedR) = 0;

		bool isFinished(void) const;
		void setFinished(const bool finished);

		/// CollisionAvoidance calls this method to know where the robot wants to go ahead of time. If no
		/// parameters are given, they both default to -1.0. That will cause this method to only return
		/// motion-defined path-points. In a rotation, that is the rotations's center. For a translation,
		/// that is its current pose and target point, and for a spline-motion, these are its waypoints.
		///
		/// @param lookAheadDistance up to which distance the returned points should lead.
		/// @param lookAheadInterval the desired distance between the points (granularity).
		/// @return a list of points in WORLD COORDINATES representing the robot's future path.
		virtual QList<QPointF> getPathPoints(const float lookAheadDistance = -1.0, const float lookAheadInterval = -1.0) const = 0;
};

#endif
