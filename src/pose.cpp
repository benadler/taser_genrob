#include "pose.h"

Pose::Pose(void)
{
	x = 0.0;
	y = 0.0;
	angleDeg = 0.0;
	this->valid = true;
	config = Configuration::instance();
}

Pose::Pose(const double x, const double y, const double angleRad)
{
	this->x = x;
	this->y = y;
	this->angleDeg = Conversion::rad2deg(angleRad);
	this->valid = true;
	config = Configuration::instance();
}

Pose::Pose(const QPointF point, const double angleRad)
{
	this->x = point.x();
	this->y = point.y();
	this->angleDeg = Conversion::rad2deg(angleRad);
	this->valid = true;
	config = Configuration::instance();
}

Pose::Pose(const Pose &other)
{
	this->x = other.getX();
	this->y = other.getY();
	this->angleDeg = other.getAngleDeg();
	this->valid = other.isValid();
	config = Configuration::instance();
}

// do I need this?
bool Pose::operator=(const Pose other)
{
	this->x = other.getX();
	this->y = other.getY();
	this->angleDeg = other.getAngleDeg();
	this->valid = other.isValid();
	return true;
}

void Pose::test(void)
{
	// these tests now fail due to comparing floats with each other.

	qDebug() << "Pose::test()";
	// Create a new pose
	Pose myPose;
	// declare helper variables ([leftWheel|rightWheel]delta[X|Y])
	int ldX, ldY, rdX, rdY;

	Q_ASSERT(myPose.getX() == 0);
	Q_ASSERT(myPose.getY() == 0);
	Q_ASSERT(myPose.getAngleDeg() == 0.0);

	Q_ASSERT(myPose.getWheelL().getX() == -config->getAxisLength() / 2);
	Q_ASSERT(myPose.getWheelL().getY() == 0);

	Q_ASSERT(myPose.getWheelR().getX() == +config->getAxisLength() / 2);
	Q_ASSERT(myPose.getWheelR().getY() == 0);

	// rotate 90 degrees clockwise
	myPose.rotateByDeg(90.0);
	Q_ASSERT(myPose.getX() == 0);
	Q_ASSERT(myPose.getY() == 0);
	Q_ASSERT(myPose.getAngleDeg() == 90.0);

	Q_ASSERT(myPose.getWheelL().getX() == 0);
	Q_ASSERT(myPose.getWheelL().getY() == -config->getAxisLength() / 2);

	Q_ASSERT(myPose.getWheelR().getX() == 0);
	Q_ASSERT(myPose.getWheelR().getY() == +config->getAxisLength() / 2);

	// rotate another 180 degrees clockwise
	myPose.rotateByDeg(180.0);
	Q_ASSERT(myPose.getX() == 0);
	Q_ASSERT(myPose.getY() == 0);
	Q_ASSERT(myPose.getAngleDeg() == 270.0);

	Q_ASSERT(myPose.getWheelL().getX() == 0);
	Q_ASSERT(myPose.getWheelL().getY() == +config->getAxisLength() / 2);

	Q_ASSERT(myPose.getWheelR().getX() == 0);
	Q_ASSERT(myPose.getWheelR().getY() == -config->getAxisLength() / 2);

	// move by 10000 / 20000
	myPose.moveBy(10000, 20000);
	Q_ASSERT(myPose.getX() == 10000);
	Q_ASSERT(myPose.getY() == 20000);
	Q_ASSERT(myPose.getAngleDeg() == 270.0);

	Q_ASSERT(myPose.getWheelL().getX() == 10000);
	Q_ASSERT(myPose.getWheelL().getY() == +config->getAxisLength() / 2 + 20000);

	Q_ASSERT(myPose.getWheelR().getX() == 10000);
	Q_ASSERT(myPose.getWheelR().getY() == -config->getAxisLength() / 2 + 20000);

	// rotate another 180 degrees clockwise
	myPose.rotateByDeg(180.0);
	Q_ASSERT(myPose.getX() == 10000);
	Q_ASSERT(myPose.getY() == 20000);
	Q_ASSERT(myPose.getAngleDeg() == 90.0);

	Q_ASSERT(myPose.getWheelL().getX() == 10000);
	Q_ASSERT(myPose.getWheelL().getY() == -config->getAxisLength() / 2 + 20000);

	Q_ASSERT(myPose.getWheelR().getX() == 10000);
	Q_ASSERT(myPose.getWheelR().getY() == +config->getAxisLength() / 2 + 20000);

	// rotate 90 degrees COUNTERclockwise
	myPose.rotateByDeg(-90.0);
	// move it TO -40000, +30000
	myPose.moveBy(-50000, +10000);

	Q_ASSERT(myPose.getX() == -40000);
	Q_ASSERT(myPose.getY() == 30000);
	Q_ASSERT(myPose.getAngleDeg() == 0.0);

	Q_ASSERT(myPose.getWheelL().getX() == -config->getAxisLength() / 2 - 40000);
	Q_ASSERT(myPose.getWheelL().getX() == -config->getAxisLength() / 2 + myPose.getX());
	Q_ASSERT(myPose.getWheelL().getY() == 30000);


	Q_ASSERT(myPose.getWheelR().getX() == config->getAxisLength() / 2 - 40000);
	Q_ASSERT(myPose.getWheelR().getX() == config->getAxisLength() / 2 + myPose.getX());
	Q_ASSERT(myPose.getWheelR().getY() == 30000);

	// set it to 45 degrees clockwise at 0/0
	myPose.rotateByDeg(45.0);
	myPose.setX(0);
	myPose.setY(0);
	Q_ASSERT(myPose.getX() == 0);
	Q_ASSERT(myPose.getY() == 0);
	Q_ASSERT(myPose.getAngleDeg() == 45.0);

	ldY = (int)-(sin(myPose.getAngleRad()) * config->getAxisLength() / 2);
	ldX = (int)-(cos(myPose.getAngleRad()) * config->getAxisLength() / 2);
	rdY = (int)+(sin(myPose.getAngleRad()) * config->getAxisLength() / 2);
	rdX = (int)+(cos(myPose.getAngleRad()) * config->getAxisLength() / 2);

// 	qDebug() << "myPose is" << myPose.toString(true) << "ld is" << ldX << ldY << "rd is" << rdX << rdY;

	Q_ASSERT(myPose.getWheelL().getX() == ldX);
	Q_ASSERT(myPose.getWheelL().getY() == ldY);

	Q_ASSERT(myPose.getWheelR().getX() == rdX);
	Q_ASSERT(myPose.getWheelR().getY() == rdY);





	// set it to 135 degrees clockwise at 0/0
	myPose.setPose(0.0, 0.0, Conversion::deg2rad(225.0));
	Q_ASSERT(myPose.getX() == 0);
	Q_ASSERT(myPose.getY() == 0);
	Q_ASSERT(myPose.getAngleDeg() == 225.0);

	ldY = (int)-(sin(myPose.getAngleRad()) * config->getAxisLength() / 2);
	ldX = (int)-(cos(myPose.getAngleRad()) * config->getAxisLength() / 2);
	rdY = (int)+(sin(myPose.getAngleRad()) * config->getAxisLength() / 2);
	rdX = (int)+(cos(myPose.getAngleRad()) * config->getAxisLength() / 2);

// 	qDebug() << "myPose is" << myPose.toString(true) << "ld is" << ldX << ldY << "rd is" << rdX << rdY;

	Q_ASSERT(myPose.getWheelL().getX() == ldX);
	Q_ASSERT(myPose.getWheelL().getY() == ldY);

	Q_ASSERT(myPose.getWheelR().getX() == rdX);
	Q_ASSERT(myPose.getWheelR().getY() == rdY);
}

Pose Pose::getWheelL(void) const
{
	// At zero angle, the robot faces east. The left wheel
	// should be half the axislength higher than the center.
	double dX = -sin(getAngleRad()) * config->getAxisLength() / 2.0;
	double dY = cos(getAngleRad()) * config->getAxisLength() / 2.0;

// 	qDebug() << "Pose::getWheelL(): angle is" << angleDeg << "wheel deltas are" << dX << dY;

	Pose wheelPose(x + dX, y + dY, getAngleRad());
	return wheelPose;
}

Pose Pose::getWheelR(void) const
{
	// At zero angle, the robot faces east. The right wheel
	// should be half the axislength lower than the center.
	double dX = sin(getAngleRad()) * config->getAxisLength() / 2.0;
	double dY = -cos(getAngleRad()) * config->getAxisLength() / 2.0;

// 	qDebug() << "Pose::getWheelR(): angle is" << angleDeg << "wheel deltas are" << dX << dY;

	Pose wheelPose(x + dX, y + dY, getAngleRad());
	return wheelPose;
}

void Pose::setX(const double x) { this->x = x; }
void Pose::setY(const double y) { this->y = y; }

void Pose::setAngleDeg(const double angleDeg)
{
	// This is the only method where the angle is actually set. All
	// other methods like setAngle* and rotateBy* use this method.

	// use modulo for floats = fmod()
	this->angleDeg = fmod(angleDeg, 360.0);

	// Make sure the angle never becomes negative, i.e. keep it
	// between 0.0 and 360.0.
	if(this->angleDeg < 0.0) this->angleDeg += 360.0;
}

void Pose::setAngleRad(const double angleRad)
{
	setAngleDeg(Conversion::rad2deg(angleRad));
}

void Pose::rotateByRad(const double delta)
{
	setAngleDeg(angleDeg + Conversion::rad2deg(delta));
}

void Pose::rotateByDeg(const double delta)
{
	setAngleDeg(angleDeg + delta);
}

void Pose::moveBy(const double x, const double y)
{
	this->x += x;
	this->y += y;
}

void Pose::setPose(const QPointF position, const double angleRad)
{
	this->x = position.x();
	this->y = position.y();
	setAngleRad(angleRad);
}

void Pose::setPose(const double x, const double y, const double angleRad)
{
	this->x = x;
	this->y = y;
	setAngleRad(angleRad);
}

void Pose::setPosition(const QPointF position)
{
	this->x = position.x();
	this->y = position.y();
}

double Pose::getX(void) const { return x; }
double Pose::getY(void) const { return y; }

double Pose::getAngleDeg(void) const { return angleDeg; }
double Pose::getAngleRad(void) const { return Conversion::deg2rad(angleDeg); }

double Pose::distanceTo(const Pose &other) const
{
	double distance =
			sqrt(
			pow(other.getX() - this->getX(), 2)
			+
			pow(other.getY() - this->getY(), 2)
			);
// 	qDebug() << "Pose::distanceTo(): distance between me" << toString() << "and other" << other.toString() << "is" << distance;
	return distance;
}

Pose Pose::differenceTo(const Pose &other) const
{
	return Pose
	(
		other.getX() - x,
		other.getY() - y,
		other.getAngleRad() - getAngleRad()
	);
}

double Pose::advance(int advanceL, int advanceR)
{
	// Make sure to skip the following code if the robot has not advanced.
	// We'd divide by zero otherwise and lose efficiency.
	if(advanceL == 0 && advanceR == 0)
	{
// 		qDebug() << "Pose::advance(): pose before is" << toString(true) << "advances are 0 0, returning.";
		return 0;
	}

	// Create a copy of this pose before moving. We'll check how far we have
	// moved against poseBefore at the end, so we can return the distance.
	Pose poseBefore(getX(), getY(), getAngleRad());

// 	qDebug() << "Pose::advance(): pose before is" << toString(true) << "advances are" << advanceL << advanceR;

	// create two virtual wheels from poseBefore: |-------#-------|
	QPointF wheelL
		(
		  getWheelL().getX(),
		  getWheelL().getY()
		);

	QPointF wheelR
		(
		  getWheelR().getX(),
		  getWheelR().getY()
		);

	// connect them using a line/axis
	QLineF axis(wheelL, wheelR);

	// #########################################################################
	// #### First part: move the wheels into the position
	// #### they WOULD have if they were NOT connected.
	// #########################################################################

	double wheelLAdvanceX = (cos(getAngleRad()) * (double)advanceL / 1000000.0);
	double wheelLAdvanceY = (sin(getAngleRad()) * (double)advanceL / 1000000.0);
	double wheelRAdvanceX = (cos(getAngleRad()) * (double)advanceR / 1000000.0);
	double wheelRAdvanceY = (sin(getAngleRad()) * (double)advanceR / 1000000.0);

	// move the two virtual wheels.
	wheelL.setX(wheelL.x() + wheelLAdvanceX);
	wheelL.setY(wheelL.y() + wheelLAdvanceY);
	wheelR.setX(wheelR.x() + wheelRAdvanceX);
	wheelR.setY(wheelR.y() + wheelRAdvanceY);

// 	qDebug("Pose::advance(): wheelL advanced %.6F / %.6F, moving to %.6F / %.6F", wheelLAdvanceX, wheelLAdvanceY, wheelL.x(), wheelL.y());
// 	qDebug("Pose::advance(): wheelR advanced %.6F / %.6F, moving to %.6F / %.6F", wheelRAdvanceX, wheelRAdvanceY, wheelR.x(), wheelR.y());

	// #########################################################################
	// #### Second part, and this is where the fun starts. Correct the
	// #### wheels' movement, because in reality, they ARE connected.
	// #########################################################################

	// Create two lines from the robot's new center to each wheel...
	QLineF lineLeft (QLineF(wheelL, wheelR).pointAt(0.5), wheelL);
	QLineF lineRight(QLineF(wheelL, wheelR).pointAt(0.5), wheelR);

// 	qDebug("Pose::advance(): lengths after moving wheels: lineLeft %.6F, lineRight %.6F, axis %.6F", lineLeft.length(), lineRight.length(), axis.length());
// 	qDebug("Pose::advance(): After moving the wheels individually, they have a distance of %.6F micrometers", QLineF(wheelL, wheelR).length());

	// how much longer is the new axis?
	double axisGrowth = lineLeft.length() + lineRight.length() - axis.length();
// 	qDebug("Pose::advance(): axisGrowth is %.6F micrometers", axisGrowth);

	// shorten both lines to the center, according to the ratio of wheelAdvances
	// THIS following line is the heart of it.
	lineLeft.setLength
		(
		  lineLeft.length()
		  -
		  (
		    axisGrowth
		    /
		    (fabs(advanceL / 1000000.0) + fabs(advanceR / 1000000.0))
		    *
		    fabs(advanceL / 1000000.0)
		  )
		);

	// use trackWidth instead of axis.length() to prevent rounding error accumulation.
	lineRight.setLength
		(
		  config->getAxisLength()
		  -
		  lineLeft.length()
		);

// 	qDebug("Pose::advance(): lengths after correcting wheels: lineLeft %.6F, lineRight %.6F, axis %.6F", lineLeft.length(), lineRight.length(), axis.length());

// 	qDebug("moving left  closer to center, from %f/%f to %f/%f.", wheelL.x(), wheelL.y(), lineLeft.p2().x(), lineLeft.p2().y());
// 	qDebug("moving right closer to center, from %f/%f to %f/%f.", wheelR.x(), wheelR.y(), lineRight.p2().x(), lineRight.p2().y());

	// ...then move both wheels to the line's new ends (towards the center)
	wheelL.setX(lineLeft.p2().x());
	wheelL.setY(lineLeft.p2().y());
	wheelR.setX(lineRight.p2().x());
	wheelR.setY(lineRight.p2().y());

	// how far has the robot's center advanced?
// 	qDebug("Pose::advance(): wheelL ist at %.6F / %.6F, wheelR is at %.6F / %.6F", wheelL.x(), wheelL.y(), wheelR.x(), wheelR.y());
	QPointF robotPositionNew = QLineF(wheelL, wheelR).pointAt(0.5);
// 	qDebug("Pose::advance(): center between wheels is at %.6F / %.6F", robotPositionNew.x(), robotPositionNew.y());
	setX(robotPositionNew.x());
	setY(robotPositionNew.y());
// 	qDebug() << "Pose::advance(): before changing rotation, pose is now" << toString(true);

	// how much has the robot been rotated?
	double rotation;
	if(abs(advanceL) < abs(advanceR))
	{
		// Create a new line from left to right, so we can compare angles.
		QLineF tempLine2(wheelL, wheelR);

		rotation = axis.angle(tempLine2);
		if(advanceR < 0) rotation *= -1.0;
	}
	else if(abs(advanceL) > abs(advanceR))
	{
		// Create a new line from right to left, so we can compare angles.
		QLineF tempLine2(wheelR, wheelL);

		rotation = axis.angle(tempLine2) - 180.0;
		if(advanceL < 0) rotation *= -1.0;
	}
	else if(advanceL == advanceR)
	{
		// both wheels have moved the same amount into the same direction.
		// no need to correct the wheel-positions, as the axis' length has not changed.
		// the robot's angle doesn't change.
		rotation = 0.0;
	}
	else if(advanceL == (-advanceR))
	{
		// Create a new line from left to right, so we can compare angles.
		QLineF tempLine2(wheelL, wheelR);

		if(advanceL > 0)
			rotation = -1.0 * axis.angle(tempLine2);
		if(advanceR > 0)
			rotation = +1.0 * axis.angle(tempLine2);
	}

// 	qDebug("Pose::advance(): rotation due to advances is %.6F", rotation);
	rotateByDeg(rotation);

// 	qDebug() << "Pose::advance(): pose after is" << toString(true);

	return distanceTo(poseBefore);
}

QPointF Pose::toPoint(void) const
{
	return QPointF(x, y);
}

bool Pose::isValid(void) const
{
	return valid;
}

void Pose::setValid(bool valid)
{
	this->valid = valid;
}

QString Pose::toString(const bool includingWheels, const unsigned int precision) const
{
	QString text;
	text.append("x");
	text.append(QString::number(x, 'F', precision));
	text.append(", y");
	text.append(QString::number(y, 'F', precision));
	text.append(", angle");
	text.append(QString::number(angleDeg, 'F', precision));

	if(includingWheels)
	{
		Pose poseWheelL = getWheelL();
		text.append(", wheelL x");
		text.append(QString::number(poseWheelL.getX(), 'F', precision));
		text.append(", y");
		text.append(QString::number(poseWheelL.getY(), 'F', precision));

		Pose poseWheelR = getWheelR();
		text.append(", wheelR x");
		text.append(QString::number(poseWheelR.getX(), 'F', precision));
		text.append(", y");
		text.append(QString::number(poseWheelR.getY(), 'F', precision));
	}

	return text;
}
