#include "laserscan.h"

LaserScan::LaserScan(void)
{
	logger = Logger::instance();
	config = Configuration::instance();
	mutex = new QMutex();

	logger->LaserScan("LaserScan::LaserScan()");

	timeStamp = QDateTime::currentDateTime();
}

LaserScan::LaserScan(const LaserScan &other)
{
	logger->LaserScan("LaserScan::LaserScan(RobotSide)");

	logger = Logger::instance();
	config = Configuration::instance();

	mutex = new QMutex();

	scannerAnglesFront = other.scannerAnglesFront;
	scannerDistancesFront = other.scannerDistancesFront;

	scannerAnglesRear = other.scannerAnglesRear;
	scannerDistancesRear = other.scannerDistancesRear;

	platformAngles = other.platformAngles;
	platformDistances = other.platformDistances;

	timeStamp = other.timeStamp;
}

LaserScan::~LaserScan(void)
{
	logger->LaserScan("LaserScan::~LaserScan()");
	delete mutex;
}

void LaserScan::getScannerAngles(RobotSide side, QList<float> &angles) const
{
	QMutexLocker locker(mutex);

	if(side == sideFront)
		angles = scannerAnglesFront;
	else if(side == sideRear)
		angles = scannerAnglesRear;
	else
		abort("LaserScan::getScannerAngles(): undefined side %d.", side);
}

void LaserScan::getScannerDistances(RobotSide side, QList<float> &distances) const
{
	QMutexLocker locker(mutex);

	if(side == sideFront)
		distances = scannerDistancesFront;
	else if(side == sideRear)
		distances = scannerDistancesRear;
	else
		abort("LaserScan::getScannerDistances(): undefined side %d.", side);
}

void LaserScan::setScanData(const QList<float> &angles, const QList<float> &distances, const RobotSide scanner)
{
	QMutexLocker locker(mutex);

	// Clear the data that depends on what we will now replace.
	platformAngles.clear();
	platformDistances.clear();

	//logger->LaserScan("LaserScan::setScanData(): got %d angles, %d distances for side %d.", angles.size(), distances.size(), scanner);

	if(scanner == sideFront)
	{
		scannerAnglesFront = angles;
		scannerDistancesFront = distances;
	}

	if(scanner == sideRear)
	{
		scannerAnglesRear = angles;
		scannerDistancesRear = distances;
	}

	timeStamp = QDateTime::currentDateTime();
}

void LaserScan::updatePlatformCoordinates(void)
{
	QMutexLocker locker(mutex);

	// Convert local scanner coordinates to platform coordinates
	// if they don't exist already.
	if(platformAngles.size() != 0 || platformDistances.size() != 0)
		return;

	// whats the pose of the source scanners relative to the platform?
	Pose scannerPoseFront = config->getLaserScannerPose(sideFront);
	Pose scannerPoseRear = config->getLaserScannerPose(sideRear);

	float angleRad, x, y;

	// convert the front scan data
	for(int i=0; i < scannerAnglesFront.size(); i++)
	{
		angleRad = scannerAnglesFront.at(i) + scannerPoseFront.getAngleRad();
		x = scannerPoseFront.getX() + cos(angleRad) * scannerDistancesFront.at(i);
		y = scannerPoseFront.getY() + sin(angleRad) * scannerDistancesFront.at(i);

		platformAngles.append(atan2(y, x));
		platformDistances.append(sqrt(x*x + y*y));
	}

	// convert the rear scan data
	for(int i=0; i < scannerAnglesRear.size(); i++)
	{
		angleRad = scannerAnglesRear.at(i) + scannerPoseRear.getAngleRad();
		x = scannerPoseRear.getX() + cos(angleRad) * scannerDistancesRear.at(i);
		y = scannerPoseRear.getY() + sin(angleRad) * scannerDistancesRear.at(i);

		platformAngles.append(atan2(y, x));
		platformDistances.append(sqrt(x*x + y*y));
	}
}

float LaserScan::getClosestObstacleDistance(void)
{
	QMutexLocker locker(mutex);

	float mindist = 100.0;

	updatePlatformCoordinates();

	for(int i=0; i < platformDistances.size(); i++)
		if (platformDistances.at(i) < mindist)
			mindist = platformDistances.at(i);

	return mindist;
}


int LaserScan::getAge(void) const
{
	QMutexLocker locker(mutex);

	return timeStamp.secsTo(QDateTime::currentDateTime());
}
