#include "rotation.h"

Rotation::Rotation(const float angleDiffRad) : Motion()
{
	this->orientationEnd = Localization::instance()->getPose().getAngleRad() + angleDiffRad;

	while(orientationEnd < -M_PI) orientationEnd += 2*M_PI;
	while(orientationEnd > +M_PI) orientationEnd -= 2*M_PI;
}

Rotation::~Rotation()
{
	// nothing to do, we just need a d'tor because its virtual in the base class.
}

MotionType Rotation::getType(void) const
{
	return rotation;
}

bool Rotation::step(const Pose &pose, float &speedL, float &speedR)
{
	double angleDiff = Conversion::rad2deg(orientationEnd - pose.getAngleRad());

	// these two lines make sure we use the shorter turn
	if(angleDiff < -180.0) angleDiff += 360.0;
	if(angleDiff > +180.0) angleDiff -= 360.0;

	logger->Motion("Rotation::step(): rotating to %.2F deg, we are at %.2F deg, difference is %.2F",
		Conversion::rad2deg(orientationEnd),
		pose.getAngleDeg(),
		angleDiff);

	if(fabs(angleDiff) > 0.05)
	{
		// We want to slow down when we are closer to the end. Unfortunately, the
		// "when to start slowing down" depens on the maximum rotational speed...
		const double maxRotationSpeed = config->getMaximumWheelSpeed() * config->getOverlaySpeedFactor() * config->getSpeedFactorWhenOnlyRotating();
		
		// With maxRotationSpeed=2.0, we start braking at 540 degrees...
		// With maxRotationSpeed=1.0, we start braking at 270 degrees...
		// With maxRotationSpeed=0.5, we start braking at 135 degrees...
		double wheelSpeeds = std::min(fabs(angleDiff / 270.0) + 0.02, maxRotationSpeed);

		// At this point, the robot would turn counterclockwise, because wheelSpeeds
		// is always positive. But if the angleDiff is negative, we need to turn right.
		if(angleDiff < 0.0) wheelSpeeds *= -1;

		speedR = wheelSpeeds;
		speedL = -speedR;

		logger->Motion("Rotation::step(): setting wheelspeeds to %.2F / %.2F.", speedL, speedR);
		return true;
	}
	else
	{
		// We're done rotating!
		logger->Motion("Rotation::step(): reached end orientation.");
		speedL = 0.0;
		speedR = 0.0;
		finished = true;
		return false;
	}
}

QList<QPointF> Rotation::getPathPoints(const float lookAheadDistance, const float lookAheadInterval) const
{
	// This one is easy. As the robot only rotates on its own axis here, we can always return
	// a single point, which is the robot's current position. Thats even independent of whether
	// the given parameters are specidifed or -1.0.
	Q_UNUSED(lookAheadDistance);
	Q_UNUSED(lookAheadInterval);

	QList<QPointF> points;
	points << Localization::instance()->getPose().toPoint();
	return points;
}
