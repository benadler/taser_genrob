#ifndef POSE_H
#define POSE_H

#include <QDebug>

#include <math.h>
#include "configuration.h"
#include "conversion.h"

/// @class Pose
/// @brief This class represents the pose of the robot, which is made up of a position and a rotation.
///
/// Distances are stored as meters. Positive rotations turn counterclockwise (mathematically positive).
///
/// A rotation of 0 degrees/radians means that the robot is facing to the east, with the left wheel
/// extending to the north and the right wheel extending to the south. The angle/theta is stored as
/// degrees for higher resolution, but can also be altered using radiant methods.
///
/// The underlying coordinate system starts in the lower left with 0/0. X-values grow from left to
/// right and Y-values grow from bottom to top. Note that some other coordinate systems (like computer
/// screens) have their Y-values grow from top to bottom, so you might have to invert the Y-values
/// in order to adapt to these.

class Configuration;

class Pose
{
	private:
		double x, y;		///< The x and y coordinates of the pose
		double angleDeg;	///< The poses angle in degrees, always between +0 and +360
		bool valid;		///< Whether this is a valid pose. This is usually true.

		Configuration* config;

	public:
		// Constructors
		Pose(void);
		Pose(const double x, const double y, const double angleRad = 0.0);
		Pose(const QPointF point, const double angleRad = 0.0);
		Pose(const Pose &other);

		void test(void);

		/// This method rotates the pose by an angle.
		/// @param delta the angle which should be rotated in degrees. Positive deltas rotate counterclockwise.
		void rotateByDeg(const double delta);

		/// This method rotates the pose by an angle.
		/// @param delta the angle which should be rotated in radiants. Positive deltas rotate counterclockwise.
		void rotateByRad(double delta);

		/// This method moves the pose by an offset.
		/// @param x the x-offset. A positive value moves the robot to the right.
		/// @param y the y-offset. A positive value moves the robot to the top.
		void moveBy(const double x, const double y);

		/// This method returns a QPointF representing the position of this Pose.
		/// @return a QPointF containing the x/y values of the pose
		QPointF toPoint(void) const;

		/// This method sets the x-value of the pose
		/// @param x the new x value of the pose
		void setX(const double x);

		/// This method sets the y-value of the pose
		/// @param y the new y value of the pose
		void setY(const double y);

		/// This method sets the angle/orientation/theta of the pose
		/// @param angle the new angle of the pose in radiants
		void setAngleRad(const double angle);

		/// This method sets the angle/orientation/theta of the pose
		/// @param angle the new angle of the pose in degrees
		void setAngleDeg(const double angle);

		/// This method sets the position and orientation of the pose
		/// @param x the new x value of the pose
		/// @param y the new y value of the pose
		/// @param angle the new angle of the pose in radiants
		void setPose(const double x, const double y, const double angleRad = 0.0);

		/// This method sets the position and orientation of the pose
		/// @param position the new position of the pose
		/// @param angle the new angle of the pose in radiants
		void setPose(const QPointF position, const double angle = 0.0);

		/// This method sets only the position of the pose, doesn't change orientation
		/// @param position the new position of the pose
		void setPosition(const QPointF position);

		/// This method returns the current x-value of the pose
		/// @return the current x-value of the pose
		double getX(void) const;

		/// This method returns the current y-value of the pose
		/// @return the current y-value of the pose
		double getY(void) const;

		/// This method returns the current angle of the pose
		/// @return the current angle of the pose in radiants
		double getAngleRad(void) const;

		/// This method returns the current angle of the pose
		/// @return the current angle of the pose in degrees
		double getAngleDeg(void) const;

		/// This method returns whether the pose is valid or not
		/// @return true when the pose is valid, else false
		bool isValid(void) const;

		/// This method sets the validity of the pose
		/// @param valid true when the pose should be valid, else false
		void setValid(bool valid);

		/// This method returns the pose of the robot's left wheel. When the robot's
		/// pose is 0|0|0, the left wheel's pose is 0|+robotAxisLength/2|0
		/// @return the pose of the robot's left wheel
		Pose getWheelL(void) const;

		/// This method returns the pose of the robot's right wheel. When the robot's
		/// pose is 0|0|0, the right wheel's pose is 0|-robotAxisLength/2|0
		/// @return the pose of the robot's right wheel
		Pose getWheelR(void) const;

		/// This method returns the distance to another pose
		/// @param other the pose to which the distance shall be found
		/// @return the distance to the other pose
		double distanceTo(const Pose &other) const;

		/// This method returns the difference to another pose
		/// @param other the pose to which the difference shall be found
		/// @return a pose representing the difference to the other pose
		Pose differenceTo(const Pose &other) const;

		/// This method advances the pose depending on how much the left/right wheels have moved.
		/// The algorithm used is quite precise and probably rather inefficient.
		/// @param advanceL how many micrometers the left wheel advanced
		/// @param advanceR how many micrometers the left wheel advanced
		/// @return how many micrometers the robot('s center) has moved because of these advances
		double advance(const int advanceL, const int advanceR);

		bool operator=(const Pose other);

		/// This method returns a text representation of the pose
		/// @param inclduingWheels whether information about the poses of the robot's wheels should be included
		/// @param precision indicates how precise the numbers should be
		/// @return the text representation of the pose
		QString toString(const bool includingWheels = false, const unsigned int precision = 4) const;
};

#endif
