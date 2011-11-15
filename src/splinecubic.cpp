#include <splinecubic.h>

SplineCubic::SplineCubic(QList<QPointF> &points, const float orientationEnd) : Spline(points, orientationEnd)
{
	// Split up the list of 2D points into 2 lists of 1D points.
	logger->Spline("SplineCubic::SplineCubic(): building spline using %d points", points.size());

	QList<float> pointsX, pointsY;
	for(int i = 0;i<points.size(); ++i)
	{
		pointsX.append(points.at(i).x());
		pointsY.append(points.at(i).y());
	}

	// For n points, create n-1 Polynoms for a spline with n-1 segments
	//qDebug() << "creating" << points.size()-1 << "polynoms for" << points.size() << "points.";
	xf = new Polynom[points.size()-1];
	yf = new Polynom[points.size()-1];

	buildPolynoms(pointsX, xf);
	buildPolynoms(pointsY, yf);

	logger->Spline("SplineCubic::SplineCubic(): pointsX has %d points", pointsX.size());

	getLength();
}

SplineCubic::SplineCubic(const SplineCubic &other) : Spline(other)
{
	xf = new Polynom[points.size()-1];
	yf = new Polynom[points.size()-1];

	for(int i=0;i<points.size();i++)
	{
		xf[i] = other.xf[i];
		yf[i] = other.yf[i];
	}
}

SplineCubic::~SplineCubic(void)
{
	if(xf) delete [] xf;
	if(yf) delete [] yf;
}

void SplineCubic::buildPolynoms(QList<float> points, Polynom *f)
{
	int numberOfPoints = points.size();

	double s[numberOfPoints];
	double y[numberOfPoints];
	double v[numberOfPoints-1];
	double q[numberOfPoints];

	long i;

	y[0] = 3 * (points[1] - points[0]);

	for(i=1;i<numberOfPoints-1;i++)
		y[i] = 3 * (points[i+1] - points[i-1]);

	y[numberOfPoints-1] = 3*(points[numberOfPoints-1] - points[numberOfPoints-2]);

	v[0] = 0.5;

	q[0] = 0.5 + y[0];

	for(i = 1; i < numberOfPoints-1; i++)
	{
		v[i] = 1.0 / (4 - v[i-1]);
		q[i] = v[i] * (y[i] - q[i-1]);
	}

	q[numberOfPoints-1] = (1.0 / (2 - v[numberOfPoints-2])) * (y[numberOfPoints-1] - q[numberOfPoints-2]);

	s[numberOfPoints-1] = q[numberOfPoints-1];

	for(i=numberOfPoints-2;i>=0;i--)
		s[i] = q[i] - v[i]*s[i+1];

	for(i=0;i<numberOfPoints-1;i++)
	{
		f[i].a = points[i];
		f[i].b = s[i];
		f[i].c = 3*points[i+1] - 3*points[i] - 2*s[i] - s[i+1];
		f[i].d = 2*points[i] - 2*points[i+1] + s[i] + s[i+1];
	}
}

QPointF SplineCubic::evaluate0t(double t) const
{
	Q_ASSERT(t >= 0.0);
	Q_ASSERT(t <= 1.0);

	int polynomNumber; double polynomT;
	t2tn(t, polynomT, polynomNumber);

	return QPointF(xf[polynomNumber].evaluate0(polynomT), yf[polynomNumber].evaluate0(polynomT));
}

QPointF SplineCubic::evaluate1t(double t) const
{
	Q_ASSERT(t >= 0.0);
	Q_ASSERT(t <= 1.0);

	int polynomNumber; double polynomT;
	t2tn(t, polynomT, polynomNumber);

	return QPointF(xf[polynomNumber].evaluate1(polynomT), yf[polynomNumber].evaluate1(polynomT));
}

QPointF SplineCubic::evaluate2t(double t) const
{
	Q_ASSERT(t >= 0.0);
	Q_ASSERT(t <= 1.0);

	int polynomNumber; double polynomT;
	t2tn(t, polynomT, polynomNumber);

	return QPointF(xf[polynomNumber].evaluate2(polynomT), yf[polynomNumber].evaluate2(polynomT));
}

QPointF SplineCubic::evaluate0(float distance) const
{
	if(distance < 0.0) distance = getCursor();	// No parameter distance given, use cursor position
	double t = distanceToT(distance);

	int polynomNumber; double polynomT;
	t2tn(t, polynomT, polynomNumber);

	return QPointF(xf[polynomNumber].evaluate0(polynomT), yf[polynomNumber].evaluate0(polynomT));
}

QPointF SplineCubic::evaluate1(float distance) const
{
	if(distance < 0.0) distance = getCursor();	// No parameter distance given, use cursor position
	double t = distanceToT(distance);

	int polynomNumber; double polynomT;
	t2tn(t, polynomT, polynomNumber);

	QPointF point(xf[polynomNumber].evaluate1(polynomT), yf[polynomNumber].evaluate1(polynomT));

// 	printf("SplineCubic::evaluate1(): t is %.6F, polynom %d, polyT is %.6F.\n", t, polynomNumber, polynomT);
// 	printf("SplineCubic::evaluate1(): point at %d is %d / %d.\n", distance, point.x(), point.y());
	return point;
}

QPointF SplineCubic::evaluate2(float distance) const
{
	if(distance < 0.0) distance = getCursor();	// No parameter distance given, use cursor position
	double t = distanceToT(distance);

	int polynomNumber; double polynomT;
	t2tn(t, polynomT, polynomNumber);

	return QPointF(xf[polynomNumber].evaluate2(polynomT), yf[polynomNumber].evaluate2(polynomT));
}

double SplineCubic::getAngleRad(float distance) const
{
	if(distance < 0.0) distance = getCursor();	// No parameter distance given, use cursor position

	QPointF abl1 = evaluate1(distance);
	double angleRad = atan2(abl1.y(), abl1.x());
// 	logger->Spline("getAnglerad(): got atan2(%.2F, %.2F) = %.4F.", abl1.y(), abl1.x(), angleRad);
	if(angleRad < 0.0) angleRad *= -1.0;
	else if(angleRad > 0.0) angleRad = 2*M_PI - angleRad;

	// mirror the angle on the x axis...
	return 2*M_PI - angleRad;
}

double SplineCubic::getCurvature(float distance) const
{
	if(distance < 0.0) distance = getCursor();	// No parameter distance given, use cursor position
	double t = distanceToT(distance);

	// http://de.wikipedia.org/wiki/Krümmung
	// http://upload.wikimedia.org/math/a/6/3/a63b7d087733b448c4f798d16d65b421.png
	double val1X = evaluate1t(t).x();
	double val1Y = evaluate1t(t).y();
	double val2X = evaluate2t(t).x();
	double val2Y = evaluate2t(t).y();

// 	qDebug() << "SplineCubic::getCurvature(): val1X" << val1X << "val1Y" << val1Y << "val2X" << val2X << "val2Y" << val2Y;

	double curvature =
		(val1X * val2Y - val2X * val1Y)
		/
		pow(val1X*val1X + val1Y*val1Y, 3.0/2.0);

// 	qDebug("SplineCubic::getCurvature(): distance %.3F, t is %.6F, curvature is %.6F", distance, t, curvature);

	return curvature;
}

bool SplineCubic::step(const Pose &pose, float &speedL, float &speedR)
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
		//logger->Spline("SplineCubic::step(): moving.");

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

		logger->Spline("SplineCubic::step(): trying to advance spline by %.8F meters", lastDistanceDelta);

		// Try to advance the spline by the same distance that the robot has advanced in reality.
		if(advance(lastDistanceDelta))
		{
			static bool initialized = false;
			static QTextStream out;
			if(!initialized)
			{
				initialized = true;
				static QFile file("/tmp/data.dat");
     				if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
         				abort("couldn't open data file");

     				out.setDevice(&file);
			}
			out << "SHOULD: " << evaluate0().x() << "\t" << evaluate0().y() << "\n";
			out << "IS    : " << pose.toPoint().x() << "\t" << pose.toPoint().y() << "\n";
			
			// What to do about correcting for drift etc? Easy: We first ask the spline
			// for its curvature and set wheelSpeeds according to that.
			// Then we compare the robots position with the one from the spline's cursor,
			// and see how far and in which direction the robot is off. Taking these
			// error values, Conversion::correctWheelSpeeds() does exactly what its name
			// implies: It brakes/accelerates one wheel to get the robot back on track.

			double curvature = getCurvature();
			Conversion::curvature2wheelSpeeds(curvature, speedL, speedR);

// 			speedL = speedR = 0.4;

			logger->Spline
				(
				  "SplineCubic::step(): spline %.8F / %.3F, curvature %.4F, setting wheel speeds to %.2F %.2F.",
				  getCursor(),
				  getLength(),
				  curvature,
				  speedL,
				  speedR
				);

			QPointF positionShould = evaluate0();

			// save these values, so we can advance the spline correctly in the next iteration
			lastSpeedL = speedL;
			lastSpeedR = speedR;

			// this call is put AFTER saving the speeds for the next iteration. This way, corrections
			// to "follow the rabbit" will not be included in advancing the spline next time.
			Conversion::correctWheelSpeeds(pose, positionShould, speedL, speedR);


			logger->Spline("SplineCubic::step(): done, setting wheelspeeds to %.2F / %.2F", speedL, speedR);

			return true;
		}
		else
		{
			// currentPhase is "moving", but the spline cannot advance any more, so go to phase rotatingEnd
			logger->Spline("SplineCubic::step(): moving, spline is done, setting currentRobotStatus to rotatingEnd.");
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

	abort("SplineCubic::step(): we should never be here!");

	// just to make the compiler shut up...
	return true;
}
