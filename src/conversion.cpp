#include "conversion.h"

double Conversion::deg2rad(double degree)
{
	return (degree * M_PI / 180);
}

double Conversion::rad2deg(double radiant)
{
	return (radiant * 180 / M_PI);
}

void Conversion::curvature2wheelSpeeds(const double curvature, float &speedL, float &speedR)
{
	Configuration* config = Configuration::instance();
	double maximumSpeed = config->getMaximumWheelSpeed();

	// start braking when the curve has a radius of less than e.g. 4m
	double smallestHighSpeedCurveRadius = config->getSmallestHighSpeedCurveRadius();

	// brake down to which fraction when the curveRadius has become (almost) 0?
	double speedFactorWhenOnlyRotating = config->getSpeedFactorWhenOnlyRotating();

	// OverlaySpeedFactor: how fast should the robot go? "1.0" means go with maximumSpeed (on straight segments)
	double overlaySpeedFactor = config->getOverlaySpeedFactor();

	// prevent division by zero on straight segments
	if(fabs(curvature) < 0.000001)
	{
		speedL = maximumSpeed * overlaySpeedFactor;
		speedR = maximumSpeed * overlaySpeedFactor;
		return;
	}

	// curveRadius. Unit is meters. positive? right curve! negative? left curve!
	double curveRadius = 1.0 / curvature;

	double wheelSpeedRatio =
		(double)(curveRadius - (config->getAxisLength() / 2.0))
		/
		(double)(curveRadius + (config->getAxisLength() / 2.0));

	// determine wheelSpeeds, not taking minimumCurveRadius into account
	// There are 4 if-sections, can be merged into 2, but this is easier
	// on my brain for now.
	if(wheelSpeedRatio < 0.0)
	{
		// we are here because one wheel needs to go backwards!

		if(wheelSpeedRatio > -1.0)
		{
			// WSR between -1 and 0: right curve, the right wheel needs to go backwards!
			speedL = maximumSpeed;
			speedR = maximumSpeed * wheelSpeedRatio;
		}
		if(wheelSpeedRatio < -1.0)
		{
			// WSR between -1 and smaller: left curve, the left wheel needs to go backwards!
			speedL = maximumSpeed / wheelSpeedRatio;
			speedR = maximumSpeed;
		}
	}
	else if(wheelSpeedRatio < 1.0)
	{
		// positive curveRadius, a right curve, make the left wheel go maximumSpeed
		speedL = maximumSpeed;
		speedR = maximumSpeed * wheelSpeedRatio;
	}
	else if(wheelSpeedRatio > 1.0)
	{
		// negative curveRadius, a left curve, make the right wheel go maximumSpeed
		speedL = maximumSpeed / wheelSpeedRatio;
		speedR = maximumSpeed;
	}
	else
	{
		abort("wheelSpeedRatio is neither smaller nor bigger than 1, I should not be here.");
	}

	// determine how much to break because the curve is getting too tight.
	double curvatureSpeedFactor = fabs(curveRadius) / smallestHighSpeedCurveRadius;

	if(curvatureSpeedFactor > 1.0)
		curvatureSpeedFactor = 1.0;

	if(curvatureSpeedFactor < speedFactorWhenOnlyRotating)
		curvatureSpeedFactor = speedFactorWhenOnlyRotating;

	speedL = speedL * overlaySpeedFactor * curvatureSpeedFactor;
	speedR = speedR * overlaySpeedFactor * curvatureSpeedFactor;

	const float temp = speedL;
	speedL = speedR;
	speedR = temp;

// 	qDebug("Conversion::c2w(): curvature %.4F, radius %.2F, ratio %.8F, curveSpdFactor %.2F, spd %.4F %.4F.",
// 	       curvature,
// 	       curveRadius,
// 	       wheelSpeedRatio,
// 	       curvatureSpeedFactor,
// 	       speedL,
// 	       speedR);
}

void Conversion::correctWheelSpeeds(const Pose poseCurrent, const QPointF positionShould, float &speedL, float &speedR)
{
	// By what factor are we allowed to change a wheel's speed at most?
	const double maximumSpeedCorrectionFactor = Configuration::instance()->getMaximumSpeedCorrectionFactor();

	// In which direction does positionShould lie?
	QPointF difference = poseCurrent.toPoint() - positionShould;
	double angleRobotToDesiredPosition = atan2(difference.y(), difference.x());
	if(angleRobotToDesiredPosition < M_PI) angleRobotToDesiredPosition += M_PI;
	angleRobotToDesiredPosition -= poseCurrent.getAngleRad();
	if(angleRobotToDesiredPosition < 0.0) angleRobotToDesiredPosition = 2 * M_PI + angleRobotToDesiredPosition;

	Logger::instance()->Conversion("Conversion::correctWheelSpeeds(): robot is at %.2F / %.2F, should be %.2F / %.2F, angleDiff is %.2F deg.",
		poseCurrent.getX(),
		poseCurrent.getY(),
		positionShould.x(),
		positionShould.y(),
		rad2deg(angleRobotToDesiredPosition));

	Q_ASSERT(angleRobotToDesiredPosition >= 0.0);
	Q_ASSERT(angleRobotToDesiredPosition <= 2*M_PI);

	// How far is the robot away from the desiredPosition? Clip at 1.0, because we'll
	// use this as a factor lateron.
	const double distanceToDesiredPosition = sqrt(pow(difference.x(), 2) + pow(difference.y(), 2));
	const double factorDistance = std::min(distanceToDesiredPosition, 1.0);

	// This value is 0 < x < 1, and higher values mean the point is "even more sideways"
	double factorAngle;// = fmod(angleRobotToDesiredPosition, M_PI/2.0) / M_PI / 2.0;

	// This is the resulting factor for angle and distance, so its value is
	// defined as: 0 < x < maximumSpeedCorrectionFactor. If its zero, the speeds
	// should not be changed.
	double factorCombined;

	// Where should the robot be? In his...
	if(angleRobotToDesiredPosition < M_PI/2.0)
	{
		// ...front left
		factorAngle = fmod(angleRobotToDesiredPosition, (M_PI/2)) / (M_PI/2);
		factorCombined = factorAngle * factorDistance;
		Logger::instance()->Conversion("Conversion::correctWheelSpeeds(): front left: factor angle %.3F, distance %.2F, combined %.4F.", factorAngle, factorDistance, factorCombined);

		// Accelerate the right wheel. If factorCombined is 0, don't change the speed,
		// if its 1, change it using maximumSpeedCorrectionFactor.
		speedR = speedR * (1.0 + ((maximumSpeedCorrectionFactor - 1.0) * factorCombined));
	}
	else if(angleRobotToDesiredPosition >= M_PI/2.0 && angleRobotToDesiredPosition < M_PI)
	{
		// ...rear left
		factorAngle = ((M_PI/2) - fmod(angleRobotToDesiredPosition, (M_PI/2))) / (M_PI/2);
		factorCombined = factorAngle * factorDistance;
		Logger::instance()->Conversion("Conversion::correctWheelSpeeds(): rear left: factor angle %.2F, distance %.2F, combined %.4F.", factorAngle, factorDistance, factorCombined);

		// Brake the left wheel. If factorCombined is 0, don't change the speed,
		// if its 1, change it using maximumSpeedCorrectionFactor.
		speedL = speedL / (1.0 + ((maximumSpeedCorrectionFactor - 1.0) * factorCombined));
	}
	else if(angleRobotToDesiredPosition >= M_PI && angleRobotToDesiredPosition < M_PI*3.0/2.0)
	{
		// ...rear right
		factorAngle = fmod(angleRobotToDesiredPosition, (M_PI/2)) / (M_PI/2);
		factorCombined = factorAngle * factorDistance;
		Logger::instance()->Conversion("Conversion::correctWheelSpeeds(): rear right: factor angle %.2F, distance %.2F, combined %.4F.", factorAngle, factorDistance, factorCombined);

		// Brake the right wheel. If factorCombined is 0, don't change the speed,
		// if its 1, change it using maximumSpeedCorrectionFactor.
		speedR = speedR / (1.0 + ((maximumSpeedCorrectionFactor - 1.0) * factorCombined));
	}
	else if(angleRobotToDesiredPosition >= M_PI*3.0/2.0)
	{
		// ...front right
		factorAngle = ((M_PI/2) - fmod(angleRobotToDesiredPosition, (M_PI/2))) / (M_PI/2);
		factorCombined = factorAngle * factorDistance;
		Logger::instance()->Conversion("Conversion::correctWheelSpeeds(): front right: factor angle %.2F, distance %.2F, combined %.4F.", factorAngle, factorDistance, factorCombined);

		// Accelerate the left wheel. If factorCombined is 0, don't change the speed,
		// if its 1, change it using maximumSpeedCorrectionFactor.
		speedL = speedL * (1.0 + ((maximumSpeedCorrectionFactor - 1.0) * factorCombined));
	}
	else
	{
		abort("Conversion::correctWheelSpeeds(): angle is %.10F, wasn't caught in any case!", rad2deg(angleRobotToDesiredPosition));
	}
}

double Conversion::distanceBetween(const QPointF &a, const QPointF &b)
{
	return sqrt(
		pow(a.x() - b.x(), 2)
		+
		pow(a.y() - b.y(), 2)
		);
}
