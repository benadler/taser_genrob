#include "translation.h"

Translation::Translation(const float distance) : Motion()
{
	this->distance = distance;
	start = Localization::instance()->getPose();
	target.setX(start.getX() + cos(start.getAngleRad()) * distance);
	target.setY(start.getY() + sin(start.getAngleRad()) * distance);
	
	// Assign a value thats far too high. When moving, we compare this distance
	// (which was set in the last iteration) with the currrent distance. If
	// the currect distance is HIGHER than this, we're moving away from the
	// target, which means we must stop. Easy.
	minDistanceToTarget = fabs(distance) + 1.0;
}

MotionType Translation::getType(void) const
{
	return translation;
}

bool Translation::step(const Pose &pose, float &speedL, float &speedR)
{
	const float distanceToTarget = Conversion::distanceBetween(pose.toPoint(), target);
	
// 	logger->Motion("Translation::step(): T from %s to x%.2F, y%.2F, we are at %s, distance is %.2F",
// 		qPrintable(start.toString()),
// 		target.x(), target.y(),
// 		qPrintable(pose.toString()),
// 		distanceToTarget);
	
	// If the robot has come closer to the target, make sure we know about that.
	if(distanceToTarget < minDistanceToTarget)
		minDistanceToTarget = distanceToTarget;
	
	// If we are further away from the target than (minDistanceToTarget + safety)
	// then we have passed the target and should stop. The safety is necessary
	// because of float-noise.
	if(distanceToTarget > (minDistanceToTarget + 0.01))
	{
		
		// In the last iteration, we were closer to the target than we are now.
		// This means we've moved past the target already. Stop!
		
		// We're done translating!
		logger->Motion("Translation::step(): PASSED end point!");
		speedL = 0.0;
		speedR = 0.0;
		finished = true;
		return false;
	}

	if(fabs(distanceToTarget) > 0.01)
	{
		const double maxTranslationSpeed = config->getMaximumWheelSpeed() * config->getOverlaySpeedFactor();
		const double distanceToStartBraking = pow(maxTranslationSpeed+0.2, 1.5);
		
		logger->Motion("Translation::step(): distance to target is %.2F, maxSpeed %.2F, brakingStart at %.2F", distanceToTarget, maxTranslationSpeed, distanceToStartBraking);

		// This way, we get a value between 0.005 (distanceToTarget is 0) and 1.0 (distanceToTarget is > 20cm).
		double wheelSpeeds = std::min(fabs(distanceToTarget /  (6*distanceToStartBraking)) + 0.005, maxTranslationSpeed);

		if(distance < 0.0) wheelSpeeds *= -1;

		speedL = speedR = wheelSpeeds;

		// TODO: We might want to correct wheelspeeds when the robot slowly moves off-course!

		logger->Motion("Translation::step(): setting wheelspeeds to %.2F / %.2F.", speedL, speedR);
		return true;
	}
	else
	{
		// We're done translating!
		// TODO: we might be able to delete this, as the first IF takes care of ending this motion.
		logger->Motion("Translation::step(): reached end point.");
		speedL = 0.0;
		speedR = 0.0;
		finished = true;
		return false;
	}
}

QList<QPointF> Translation::getPathPoints(const float lookAheadDistance, const float lookAheadInterval) const
{
	QList<QPointF> points;

	if(lookAheadInterval < 0.0)
	{
		// parameters not specified, just return the current pose and target.
		points << Localization::instance()->getPose().toPoint();
		points << target;
	}
	else
	{
		// Called by CollisionAvoidance. Simply use a line from current to target position.
		QLineF line(Localization::instance()->getPose().toPoint(), target);

		// If the line is longer than our lookAheadDistance, shorten it.
		if(line.length() > lookAheadDistance)
			line.setLength(lookAheadDistance);

		// We have the line's length and the interval. How many points are to be generated?
		// If the translation is shorter than the interval, return at least ONE point.
		const int numberOfPoints = (int)std::max((line.length() / lookAheadInterval), 1.0);
		
// 		qDebug("distance %.2F, interval %.2F,  numberOfPoints is %d\n", lookAheadDistance, lookAheadInterval, numberOfPoints);

		// Feed the points into the list.
		for(int i=1;i<=numberOfPoints;i++)
		{
			points << line.pointAt(i * (1.0/numberOfPoints));
// 			qDebug("linelength %.2F, pointAt %.2F,  returning point %.2F / %.2F\n", line.length(), (i * (1.0/numberOfPoints)), points.last().x(), points.last().y());
		}
	}

	return points;
}
