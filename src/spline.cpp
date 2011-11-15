#include <spline.h>

Spline::Spline(const QList<QPointF> &points, const float orientationEnd) : Motion()
{
	logger->Spline("Spline::Spline(): building spline using %d points", points.size());

	this->mappingPrecision = config->getSplineMappingSampleCount();
	this->points = points;
	this->orientationEnd = orientationEnd;

	// set an invalid length, so it gets rebuilt on first call to getLength().
	this->length = -1.0;
	this->cursor = 0.0;

	// Used in distanceToT, for caching the last successful map-position
	this->lastSearchIndex = 0;
	this->lastSearchDistance = 0.0;
	this->currentPhase = rotatingStart;

	this->timeStamp = QTime::currentTime();

	this->lastSpeedL = 0.0;
	this->lastSpeedR = 0.0;

// 	this->logger = Motion::logger;
// 	this->config = Motion::config;

	// cannot call getLength() here, as we first need to build the spline,
	// which is the duty of the derived classes.
	//getLength();
}

Spline::Spline(const Spline &other) : Motion()
{
	// copy the other spline to us
	this->length = other.length;
	this->cursor = other.cursor;
	this->points = other.points;
	this->mapLengthT = other.mapLengthT;
	this->orientationEnd = other.orientationEnd;
	this->currentPhase = other.currentPhase;

	this->lastSearchIndex = 0;
	this->lastSearchDistance = 0.0;
}

Spline::~Spline(void)
{
}

QList<QPointF> Spline::getWayPoints(void) const
{
	return points;
}

bool Spline::advance(float distance)
{
	////qDebug() << "Spline::advance(): trying to advance from" << cursor << "to" << cursor+distance;
	if(cursor + distance > length)
		return false;
	else
		cursor += distance;

	return true;
}

bool Spline::setCursor(float distance)
{
	if(distance > length)
		return false;
	else
		cursor = distance;

	return true;
}

float Spline::getCursor(void) const
{
	return cursor;
}

float Spline::getLength(void)
{
	if(length < 0)
	{
		logger->Spline("Spline::getLength(): building t <-> length-mapping using %d points", points.size());
		QPainterPath path(points.first());

		for(int i=0; i <= mappingPrecision; i++)
		{
			double t = (double)i/(double)mappingPrecision;
			path.lineTo(evaluate0t(t));

			// add the current i<->distance mapping
			mapLengthT << path.length();

			//qDebug("Spline::getLength(): t %.4F <=> %.6F micrometers", t, path.length());
		}

		// This is correct, but then the path's length might differ from the path's length up to t=1
		//path.lineTo(points.last().x(), points.last().y());

		length = path.length();
		logger->Spline("Spline::getLength(): successfully built t <-> length-mapping, saving spline length of %.2F.", length);
		//logger->Spline("Spline::getLength(): %d items in mapping, %d unique items", mappingPrecision, mapLengthT.toSet().size());
	}

	return length;
}

void Spline::t2tn(double t_in, double &t_out, int &polynomNumber_out) const
{
	// 8 points [0-7]
	// 7 Polies [0-6]
	// hack!
	if(t_in >= 1.0)
	{
		t_out = 0.9999;
		polynomNumber_out = points.size() - 2;
		//qDebug() << "Spline::t2tn(): t_in is" << t_in << "t_out is" << t_out << "polyOut is" << polynomNumber_out;
		return;
	}

	double intervalLength = 1.0 / (points.size() - 1);
	polynomNumber_out = (int)floor(t_in / intervalLength);
	t_out = (t_in - (polynomNumber_out * intervalLength)) / intervalLength;
	//qDebug() << "Spline::t2tn(): t_in is" << t_in << "t_out is" << t_out << "polyOut is" << polynomNumber_out << "intervalLength is" << intervalLength;
}

double Spline::getAngleDeg(float distance) const
{
	return Conversion::rad2deg(getAngleRad(distance));
}

double Spline::distanceToT(float distance) const
{
	// sanity checks (distance of 0.0 would otherwise return a negative value)
	if(distance >= length)
	{
// 		qDebug() << "Spline::distanceToT(): called with a distance of" << distance << "which is longer than the spline, returning 1.0";
		return 1.0;
	}

	if(distance <= 0.0)
	{
// 		qDebug() << "Spline::distanceToT(): called with a distance of" << distance << "which is <= 0, returning 0.0";
		return 0.0;
	}

// 	qDebug() << "Spline::distanceToT(): called with a distance of" << distance << "lastSearchIndex" << lastSearchIndex << "lastSearchDistance" << lastSearchDistance;

	// rewind lastSearchIndex so that we don't have off-by-one problems.
	lastSearchIndex--;

	// if lastSearchIndex was 0, it is now -1, which is an illegal index
	if(lastSearchIndex < 0) lastSearchIndex = 0;

	// Let our spline be 200m long. First call with 100m, we return a t of about 1/2.
	// Then we get called with 50m. We won't find a t past lastSearchIndex, so we
	// need to reset lastSearchIndex to 0 and search from the start again.
	if(distance < lastSearchDistance)
	{
		lastSearchIndex = 0;
	}

	// Save this value for the next iteration
	lastSearchDistance = distance;

	float currentDistance = 0.0;

	while(currentDistance < distance)
	{
		if(lastSearchIndex > mapLengthT.size() - 1)
		{
			//qDebug() << "Spline::distanceToT(): distance" << distance << "cannot be found in my mapping, (length" << length << " last index " << mapLengthT.last() << "), returning t=1.0";
			return 1.0;
		}

		currentDistance = mapLengthT.at(lastSearchIndex++);
	}

	// We use the lastSearchIndex from BEFORE currentDistance grew larger than
	// distance. Subtract 2 instead of 1 because we used lastSearchIndex*++* above.
	// We use abs() to make sure lastSearchIndex-2 can not be negative, e.g. when
	// lastSearchIndex was 0 or 1 before.
	double t = (double)(abs(lastSearchIndex-2)) / (double)mappingPrecision;

	// At index i in mapLengthT, we have passed the desired distance in the spline
// 	qDebug() << "Spline::distanceToT(): distance of" << distance << "means a t of" << t << "lastSearchIndex" << lastSearchIndex;
	return t;
}

MotionType Spline::getType(void) const
{
	return spline;
}

void Spline::stepRotatingStart(const Pose &pose, float &speedL, float &speedR)
{
	// use poseCurrent->getAngle() or rather spline.getAngle(spline.getLength())?
	double angleDiff = getAngleDeg(0.0) - pose.getAngleDeg();

	// these two lines make sure we use the shorter turn
	if(angleDiff < -180.0) angleDiff += 360.0;
	if(angleDiff > +180.0) angleDiff -= 360.0;

	logger->Spline("SplineCubic::step(): splineAngle at start is %.2F deg, we are now at %.2F, angleDiff is %.2F",
	                getAngleDeg(0.0),
	                pose.getAngleDeg(),
	                angleDiff);

	if(fabs(angleDiff) > 0.02)
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

		logger->Spline("SplineCubic::step(): rotatingStart done, setting wheelspeeds to %.2F / %.2F", speedL, speedR);
	}
	else
	{
		// We're done rotating, now travel along the spline!
		logger->Spline("SplineCubic::step(): rotatingStart, reached start orientation, now moving.");
		currentPhase = movingSpline;
	}
}

bool Spline::stepRotatingEnd(const Pose &pose, float &speedL, float &speedR)
{
	double angleDiff = orientationEnd - pose.getAngleDeg();

	// these two lines make sure we use the shorter turn
	if(angleDiff < -180.0) angleDiff += 360.0;
	if(angleDiff > +180.0) angleDiff -= 360.0;

	logger->Spline("SplineCubic::step(): targetAngle is %.2F deg, we are now at %.2F, angleDiff is %.2F", orientationEnd, pose.getAngleDeg(), angleDiff);

	if(fabs(angleDiff) > 0.02)
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

		logger->Spline("SplineCubic::step(): rotatingEnd done, setting wheelspeeds to %.2F / %.2F", speedL, speedR);

		return true;
	}
	else
	{
		// We're done!
		speedL = speedR = 0.0;
		logger->Spline("SplineCubic::step(): rotatingEnd, reached final orientation, returning false.");
		finished = true;
		return false;
	}
}

QList<QPointF> Spline::getPathPoints(const float lookAheadDistance, const float lookAheadInterval) const
{
	if(lookAheadInterval < 0.0)
	{
		// parameters not specified, just return the waypoints.
		return points;
	}
	else
	{
		QList<QPointF> splinePoints;
		const float splineCursor = getCursor();
		float splinePosition = splineCursor;

		// TODO: Optimization potential by removing the first n points and appending n
		// new points instead of rebuilding all the time.

		// build complete splinePoints list. These are distance/interval iterations.
		while(splinePosition < splineCursor + lookAheadDistance)
		{
			splinePoints << evaluate0(splinePosition);
			splinePosition += lookAheadInterval;
		}

		return splinePoints;
	}
}
