#include <splinehermite.h>

SplineHermite::SplineHermite(const QList<QPointF> &points, const float orientationEnd) : Spline(points, orientationEnd)
{
	tension = 0.0;
	bias = 0.0;

	getLength();
}

SplineHermite::SplineHermite(const SplineHermite &other) : Spline(other)
{
}

SplineHermite::~SplineHermite(void)
{

}


QPointF SplineHermite::evaluate0t(double t) const
{
	Q_ASSERT(t >= 0.0);
	Q_ASSERT(t <= 1.0);
	Q_ASSERT(points.size() >= 2);

	int segmentNumber; double segmentT;
	t2tn(t, segmentT, segmentNumber);

	QPointF p0, p1, p2, p3;

	if(segmentNumber == 0)
	{
		// First segment. We need a non-existing point in front of the first one
		// Use the values from the first for now.
		p0 = points.at(segmentNumber+0);
		p1 = points.at(segmentNumber+0);
		p2 = points.at(segmentNumber+1);

		if(points.size() >= segmentNumber + 3)
		{
			// Great, at least we have a fourth point we can use
			p3 = points.at(segmentNumber+2);
		}
		else
		{
			// As with the first, use the second point twice
			p3 = points.at(segmentNumber+1);
		}
	}
	else if(segmentNumber == points.size()-2)
	{
		// Last segment. We need a non-existing point after the last one
		// Use the values from the last for now.
		p0 = points.at(segmentNumber-1);
		p1 = points.at(segmentNumber+0);
		p2 = points.at(segmentNumber+1);

		if(points.size() >= segmentNumber + 3)
		{
			// Great, at least we have a fourth point we can use
			p3 = points.at(segmentNumber+2);
		}
		else
		{
			// As with the first, use the second point twice
			p3 = points.at(segmentNumber+1);
		}
	}
	else
	{
		// A segment within a sufficient number of points. Oh joy.
		p0 = points.at(segmentNumber-1);
		p1 = points.at(segmentNumber+0);
		p2 = points.at(segmentNumber+1);
		p3 = points.at(segmentNumber+2);

	}

	QPointF point(
		hermiteInterpolate0(
			p0.x(),
			p1.x(),
			p2.x(),
			p3.x(),
			segmentT),
		hermiteInterpolate0(
			p0.y(),
			p1.y(),
			p2.y(),
			p3.y(),
			segmentT)
		);

	return point;
}

void SplineHermite::setParameters(const double tension, const double bias)
{
	this->tension = tension;
	this->bias = bias;
}

double SplineHermite::hermiteInterpolate0(
   double y0,double y1,
   double y2,double y3,
   double t) const
{
	double m0,m1,t2,t3;
	double a0,a1,a2,a3;

	t2 = t * t;
	t3 = t2 * t;
	m0  = (y1-y0)*(1+bias)*(1-tension)/2;
	m0 += (y2-y1)*(1-bias)*(1-tension)/2;
	m1  = (y2-y1)*(1+bias)*(1-tension)/2;
	m1 += (y3-y2)*(1-bias)*(1-tension)/2;
	a0 =  2*t3 - 3*t2 + 1;
	a1 =    t3 - 2*t2 + t;
	a2 =    t3 -   t2;
	a3 = -2*t3 + 3*t2;

	return(a0*y1+a1*m0+a2*m1+a3*y2);
}

QPointF SplineHermite::evaluate0(float distance) const
{
	if(distance < 0.0) distance = getCursor();	// No parameter distance given, use cursor position
	return evaluate0t(distanceToT(distance));
}

double SplineHermite::getAngleRad(float distance) const
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

bool SplineHermite::step(const Pose &pose, float &speedL, float &speedR)
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
		//logger->Spline("SplineHermite::step(): moving.");

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

		logger->Spline("SplineHermite::step(): trying to advance spline by %.8F meters", lastDistanceDelta);

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
				  "SplineHermite::step(): spline %.8F / %.3F, setting wheel speeds to %.2F %.2F.",
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

			logger->Spline("SplineHermite::step(): rotatingStart done, setting wheelspeeds to %.2F / %.2F", speedL, speedR);

			return true;
		}
		else
		{
			// currentPhase is "moving", but the spline cannot advance any more, so go to phase rotatingEnd
			logger->Spline("SplineHermite::step(): moving, spline is done, setting currentRobotStatus to rotatingEnd.");
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

	abort("SplineHermite::step(): we should never be here!");

	// just to make the compiler shut up...
	return true;
}
