#include "collisionavoidance.h"

// Initialize the singleton-pointer to NULL
QMutex CollisionAvoidance::singletonMutex;
CollisionAvoidance* CollisionAvoidance::singleton = NULL;

CollisionAvoidance* CollisionAvoidance::instance(void)
{
	QMutexLocker locker(&singletonMutex);

	if(singleton == NULL)
		singleton = new CollisionAvoidance();

	return singleton;
}

CollisionAvoidance::CollisionAvoidance(void) : QObject()
{
	logger = Logger::instance();
	logger->CollisionAvoidance("CollisionAvoidance::CollisionAvoidance() created in thread id %d.", syscall(SYS_gettid));

	mutex = new QMutex();

	config = Configuration::instance();
	localization = Localization::instance();
	java = Java::instance();
	robot = Robot::instance();

	beeperThread = new BeeperThread();

	connect
	(
		this,
		SIGNAL(activationStatusChanged(bool)),
		beeperThread,
		SLOT(slotCollisionAvoidanceStatusChanged(bool)),
		Qt::QueuedConnection
	);

	beeperThread->start();

	// http://chaos.troll.no/~ahanssen/devdays2007/DevDays2007-Threading.pdf
	// The slot above will only be called in a queued connection if the
	// beeperThread-QThread-object lives in the beeperThread-thread. For
	// this, we need the next line:
	beeperThread->moveToThread(beeperThread);

	active = true;

	mobileD = 0;
}

CollisionAvoidance::~CollisionAvoidance(void)
{
}

void CollisionAvoidance::setMobileD(MobileD* mobileD)
{
	QMutexLocker locker(mutex);

	this->mobileD = mobileD;
}

void CollisionAvoidance::setActive(const bool active)
{
	QMutexLocker locker(mutex);

	this->active = active;
	emit activationStatusChanged(active);
}

bool CollisionAvoidance::isActive(void) const
{
	QMutexLocker locker(mutex);

	return active;
}

void CollisionAvoidance::updateObstacles(const LaserScan *scan)
{
	QMutexLocker locker(mutex);

	if(!active || robot->getStatus() != moving)
		return;

	const float lookAheadDistance = config->getCollisionAvoidanceLookAheadDistance();
	const Pose poseScannerFront = config->getLaserScannerPose(sideFront);
	const Pose poseScannerRear = config->getLaserScannerPose(sideRear);
	Pose poseRobot = localization->getPose();

	// build a list of obstacles that are closer than getCollisionAvoidanceLookAheadDistance()
	obstacles.clear();

	// Record all obstacles that are closer than lookAheadDistance. Do that for both scanners.
	scan->getScannerAngles(sideFront, scanAnglesFront);
	scan->getScannerDistances(sideFront, scanDistancesFront);

	QTransform scannerFrontToWorld;
	scannerFrontToWorld = scannerFrontToWorld.translate(poseRobot.getX(), poseRobot.getY());
	scannerFrontToWorld = scannerFrontToWorld.rotateRadians(poseRobot.getAngleRad());
	// I have no idea why I need this offset...
	scannerFrontToWorld = scannerFrontToWorld.rotate(-90.0);
	scannerFrontToWorld = scannerFrontToWorld.translate(poseScannerFront.getX(), poseScannerFront.getY());
	scannerFrontToWorld = scannerFrontToWorld.rotateRadians(poseScannerFront.getAngleRad());

	for(int i=0; i<scanAnglesFront.size(); i++)
	{
// 		printf("%.2F ", scanDistances.at(i));
		if(scanDistancesFront.at(i) < lookAheadDistance)
		{
// 			printf(" BELOW THRESHOLD, angle is %.2F\n", Conversion::rad2deg(scanAngles.at(i)));
			// convert scanner to world coordinates
			const float obsScannerX = -sin(scanAngles.at(i)) * scanDistances.at(i);
			const float obsScannerY = cos(scanAngles.at(i)) * scanDistances.at(i);

			obstacles.append(scannerFrontToWorld.map(QPointF(obsScannerX, obsScannerY)));
// 			printf("updateObstacles(): adding obstacle at scanner %.2F %.2F, world %.2F %.2F\n", obsScannerX, obsScannerY, obstacles.last().x(), obstacles.last().y());
		}
	}

	scan->getScannerAngles(sideRear, scanAnglesRear);
	scan->getScannerDistances(sideRear, scanDistancesRear);

	QTransform scannerRearToWorld;
	scannerRearToWorld = scannerRearToWorld.translate(poseRobot.getX(), poseRobot.getY());
	scannerRearToWorld = scannerRearToWorld.rotateRadians(poseRobot.getAngleRad());
	// I have no idea why I need this offset...
	scannerRearToWorld = scannerRearToWorld.rotate(-90.0);
	scannerRearToWorld = scannerRearToWorld.translate(poseScannerRear.getX(), poseScannerRear.getY());
	scannerRearToWorld = scannerRearToWorld.rotateRadians(poseScannerRear.getAngleRad());

	for(int i=0; i<scanAnglesRear.size(); i++)
	{
// 		printf("%.2F ", scanDistances.at(i));
		if(scanDistancesRear.at(i) < lookAheadDistance)
		{
// 			printf(" BELOW THRESHOLD, angle is %.2F\n", Conversion::rad2deg(scanAngles.at(i)));
			// convert scanner to world coordinates
			const float obsScannerX = -sin(scanAngles.at(i)) * scanDistances.at(i);
			const float obsScannerY = cos(scanAngles.at(i)) * scanDistances.at(i);

			obstacles.append(scannerRearToWorld.map(QPointF(obsScannerX, obsScannerY)));
// 			printf("updateObstacles(): adding obstacle at scanner %.2F %.2F, world %.2F %.2F\n", obsScannerX, obsScannerY, obstacles.last().x(), obstacles.last().y());
		}
	}

	// We now have all obstacles closer than lookAheadDistance
	// in a list. Find out where the robot's motion wants to go.
	// TODO: We might do this first and then we'll know whether
	// we have to ask the front, rear, or both laserscanners for
	// obstacles. For that, we should get pathpoints in platform
	// coordinates, not world coordinates. Ugh.
	updatePathPoints();

	// Now we have the obstacles and the points where we're
	// moving to. Compare them and ring the alarm if they're close.
	checkCollisions();
}

void CollisionAvoidance::checkCollisions(void)
{
	// compare pathPoints and obstacles and check for upcoming disasters
	bool collisionAnticipated = false;

	// We use squared distances so that we don't have to claculate sqrt() in every iteration.
	// Add 5cm of safety around the robot.
	const float collisionDistanceSquared = pow(config->getRobotRadius() + 0.05, 2);

// 	qDebug("CollisionAvoidance::checkCollisions(): reporting obstacles closer than %.2F", collisionDistanceSquared);

	QList<QPointF> obstaclesCausingCollision;

	foreach(const QPointF &obstacle, obstacles)
	{
// 		qDebug("CollisionAvoidance::checkCollisions(): checking obstacle %.2F/%.2F", obstacle.x(), obstacle.y());
		foreach(QPointF pathPoint, pathPoints)
		{
			const float squaredDistance = getSquaredDistance(pathPoint, obstacle);
			if(squaredDistance < collisionDistanceSquared)
			{
				collisionAnticipated = true;
				obstaclesCausingCollision << obstacle;
				qDebug("CollisionAvoidance::checkCollisions(): reporting obstacle %.2F / %.2F, distance to pathpoint is %.2F", obstacle.x(), obstacle.y(), sqrt(squaredDistance));
			}
			else
			{
// 				qDebug("%.1F ", squaredDistance);
			}
		}
	}

	if(collisionAnticipated)
	{
		// tell mobileD to look for a new path.
		qDebug("CollisionAvoidance::checkCollisions(): \n\nPATH BLOCKED\n\n");
		emit alertPathBlocked();

		// only add obstacles if we're on a spline-move. Polluting genmap
		// with obstacles doesn't help us for rotations & translations.
		if(mobileD->getCurrentMotion()->getType() == spline)
		{
			QMetaObject::invokeMethod
			(
				java,
				"slotAddObstaclesToMap",
				Qt::QueuedConnection,
				Q_ARG(QList<QPointF>, obstaclesCausingCollision)
			);
		}
	}
}

void CollisionAvoidance::updatePathPoints(void)
{
	// Make sure pathPoints contains all points from the robot's current
	// position up to the next getCollisionAvoidanceLookAheadDistance() meters
	// in intervals of getCollisionAvoidanceLookAheadInterval(). Whew.

	Motion* motion = mobileD->getCurrentMotion();

	// Using mobileD's spline is dangerous, as it could change or be set to 0:
	// The robot is currently moving, else we wouldn't have been called. Thus,
	// a motion should be set in mobileD. If not, I'd like to know!
	// For a motion pointer to change, mobileD must go through (and leave) the
	// waitingForPath status. Thanks to Java, that takes enough time to realize
	// that the robot isn't moving anymore ;)
	Q_ASSERT(motion != 0);

	pathPoints = motion->getPathPoints(
		config->getCollisionAvoidanceLookAheadDistance(),
		config->getCollisionAvoidanceLookAheadInterval()
	);

	// Find out where we are going. In most cases, the robot will be driving
	// either front XOR back, but with splines, it could be both. Knowing
	// this, we can make sure to only check the necessary
	checkScannerFront = checkScannerBack = false;
	foreach(const QPointF &point, pathPoints)
	{
		if(checkScannerFront && checkScannerBack)
			break;

		if(point.y() > 0.0)
			checkScannerFront = true;
		else if(point.y() < 0.0)
			checkScannerBack = true;

// 		printf("updatePathPoints(): pathpoint at %.2F %.2F\n", point.x(), point.y());

	}
}

float CollisionAvoidance::getSquaredDistance(const QPointF &a, const QPointF &b)
{
	return pow(b.x() - a.x(), 2) + pow(b.y() - a.y(), 2);
}
