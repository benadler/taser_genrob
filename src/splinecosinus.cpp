#include <splinecosinus.h>

SplineCosinus::SplineCosinus(const QList<QPointF> &points, const float orientationEnd) : Spline(points, orientationEnd)
{
	getLength();
}

SplineCosinus::SplineCosinus(const SplineCosinus &other) : Spline(other)
{
}

SplineCosinus::~SplineCosinus(void)
{
}

QPointF SplineCosinus::evaluate0t(double t) const
{
	Q_ASSERT(t >= 0.0);
	Q_ASSERT(t <= 1.0);

	int polynomNumber; double polynomT;
	t2tn(t, polynomT, polynomNumber);

	QPointF p0, p1, p2, p3;

	if(polynomNumber - 1 >= 0)
		p0 = points.at(polynomNumber - 1);
	else
		p0 = points.at(polynomNumber + 0);

	p1 = points.at(polynomNumber + 0);
	p2 = points.at(polynomNumber + 1);

	if(polynomNumber + 2 <= points.size() + -1)
		p3 = points.at(polynomNumber + 2);
	else
		p3 = points.at(polynomNumber + 1);

	//cosine interpolation
// 	const double t2 = (1 - cos(polynomT * M_PI)) / 2;
// 	return(QPointF
// 		(
// 			p1.x() * (1 - t2) + p2.x() * t2,
// 			p1.y() * (1 - t2) + p2.y() * t2
// 		));

	double tension = 1.0;

	//catmull-rom-interpolation
	double h00 = 2*polynomT*polynomT*polynomT - 3*polynomT*polynomT + 1;
	double h10 = polynomT*polynomT*polynomT - 2*polynomT*polynomT + polynomT;
	double h01 = -2*polynomT*polynomT*polynomT + 3*polynomT*polynomT;
	double h11 = polynomT*polynomT*polynomT - polynomT*polynomT;

	double hx = p2.x() - p1.x();
	double hy = p2.y() - p1.y();

	// tangents:
	double m0x = (1 - tension) * p1.x() - p0.x() / 2;
	double m1x = (1 - tension) * p3.x() - p2.x() / 2;
	double m0y = (1 - tension) * p1.y() - p0.y() / 2;
	double m1y = (1 - tension) * p3.y() - p2.y() / 2;

	double x = h00 * p1.x() + h10*hx*m0x + h01 * p2.x() + h11 * hx * m1x;
	x = h00 * p1.x() + h10*m0x + h01 * p2.x() + h11 * m1x;
	double y = h00 * p1.y() + h10*hy*m0y + h01 * p2.y() + h11 * hy * m1y;
	y = h00 * p1.y() + h10*m0y + h01 * p2.y() + h11 * m1y;
	return QPointF(x, y);
}

QPointF SplineCosinus::evaluate0(float distance) const
{
	if(distance < 0.0) distance = getCursor();	// No parameter distance given, use cursor position
	return evaluate0t(distanceToT(distance));
}

double SplineCosinus::getAngleRad(float distance) const
{
	if(distance < 0.0) distance = getCursor();	// No parameter distance given, use cursor position

	Q_ASSERT(distance <= length);

	QPointF a,b;
	if(distance + 0.015 > length)
	{
		a = evaluate0t(distanceToT(distance - 0.01));
		b = evaluate0t(distanceToT(distance));
	}
	else
	{
		a = evaluate0t(distanceToT(distance));
		b = evaluate0t(distanceToT(distance + 0.01));
	}

	QPointF diff = b - a;
	double angleRad = atan2(diff.y(), diff.x());
	if(angleRad < 0.0) angleRad *= -1.0;
	else if(angleRad > 0.0) angleRad = 2*M_PI - angleRad;

	// mirror the angle on the x axis...
	return 2*M_PI - angleRad;
}

bool SplineCosinus::step(const Pose &pose, float &speedL, float &speedR)
{
	// Its important to NOT use "else if" statements, but rather just "if". Thats because when e.g. rotatingStart
	// ends, we want to immediately go to moving in the same iteration. That wouldn't work with "else if".

	if(currentPhase == rotatingStart)
	{
		// This method is defined in the base-class "Spline", because rotating
		// into the spline's start-direction is the same for all splines.
		stepRotatingStart(pose, speedL, speedR);
		return true;
	}

	if(currentPhase == movingSpline)
	{
		//logger->Spline("SplineCosinus::step(): moving.");

		// When were the wheelspeeds set last time? Clip at 40 msecs, at least the first
		// iteration in phase "moving" would yield problematic results, as timeStamp is initialized
		// in the c'tor, but only updated i this phase (after rotatingStart has completed).
		const float timeDiff = std::min(timeStamp.msecsTo(QTime::currentTime()), 40) / 1000.0;
		timeStamp = QTime::currentTime();

		// How far has the robot advanced? advance() expects parameters in micrometers.
		double lastDistanceDelta = Pose().advance(
							(int)(lastSpeedL * timeDiff * 1000000.0),
							(int)(lastSpeedR * timeDiff * 1000000.0)
		                                         );

		logger->Spline("SplineCosinus::step(): trying to advance spline by %.8F meters", lastDistanceDelta);

		// Try to advance the spline by the same distance that the robot has advanced in reality.
		if(advance(lastDistanceDelta))
		{
			// What to do about correcting for drift etc? Easy: We first ask the spline
			// for its curvature and set wheelSpeeds according to that.
			// Then we compare the robots position with the one from the spline's cursor,
			// and see how far and in which direction the robot is off. Taking these
			// error values, Conversion::correctWheelSpeeds() does exactly what its name
			// implies: It brakes/accelerates one wheel to get the robot back on track.

// 			double curvature = getCurvature();
// 			Conversion::curvature2wheelSpeeds(curvature, speedL, speedR);

			speedL = speedR = 0.4;

			logger->Spline
				(
				  "SplineCosinus::step(): spline %.8F / %.3F, setting wheel speeds to %.2F %.2F.",
				  getCursor(),
				  getLength(),
				  speedL,
				  speedR
				);

			QPointF positionShould = evaluate0();

			// save these values, so we can advance the spline correctly in the next iteration
			lastSpeedL = speedL;
			lastSpeedR = speedR;

			// TODO: does this call modify our parent parameters speedL and speedR, or do I need to prepend "&"?
			Conversion::correctWheelSpeeds(pose, positionShould, speedL, speedR);

			logger->Spline("SplineCosinus::step(): rotatingStart done, setting wheelspeeds to %.2F / %.2F", speedL, speedR);

			return true;
		}
		else
		{
			// currentPhase is "moving", but the spline cannot advance any more, so go to phase rotatingEnd
			logger->Spline("SplineCosinus::step(): moving, spline is done, setting currentRobotStatus to rotatingEnd.");
			currentPhase = rotatingEnd;
		}
	}

	if(currentPhase == rotatingEnd)
	{
		// This method is defined in the base-class "Spline", because rotating
		// from the spline's end-direction to the desired orientation is the
		// same for all splines.
		return stepRotatingEnd(pose, speedL, speedR);
	}

	abort("SplineCosinus::step(): we should never be here!");

	// just to make the compiler shut up...
	return true;
}
