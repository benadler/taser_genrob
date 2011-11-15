#include "configuration.h"

// Initialize the singleton-pointer to NULL
QMutex Configuration::singletonMutex;
Configuration* Configuration::singleton = NULL;

Configuration* Configuration::instance()
{
	QMutexLocker locker(&singletonMutex);

	if(singleton == NULL)
		singleton = new Configuration();

	return singleton;
}

Configuration::Configuration()
{
	settings = new QSettings("TAMS", "mobiled");
	timerFlush = new QTimer(this);

	// flush settings to disk every minute
	timerFlush->setInterval(60000);
	connect(timerFlush, SIGNAL(timeout()), this, SLOT(slotSyncSettings()));
	timerFlush->start();
}

Configuration::~Configuration()
{
	settings->sync();
	delete settings;
}

void Configuration::slotSyncSettings()
{
	settings->sync();
}

double Configuration::getWheelCircumference(const RobotSide side)
{
	if(side == sideLeft)
		return settings->value("robot/wheelCircumference/left", 0.477).toDouble();
	else
		return settings->value("robot/wheelCircumference/right", 0.477).toDouble();
}
void Configuration::setWheelCircumference(const RobotSide side, const double circumference)
{
	if(side == sideLeft)
		settings->setValue("robot/wheelCircumference/left", circumference);
	else
		settings->setValue("robot/wheelCircumference/right", circumference);
}

double Configuration::getSmallestHighSpeedCurveRadius(void)
{
	return settings->value("motion/smallestHighSpeedCurveRadius", 1.0).toDouble();
}
void Configuration::setSmallestHighSpeedCurveRadius(const double radius)
{
	settings->setValue("motion/smallestHighSpeedCurveRadius", radius);
}



double Configuration::getSpeedFactorWhenOnlyRotating(void)
{
	return settings->value("motion/speedFactorWhenOnlyRotating", 0.8).toDouble();
}

void Configuration::setSpeedFactorWhenOnlyRotating(const double factor)
{
	settings->setValue("motion/speedFactorWhenOnlyRotating", factor);
}



double Configuration::getOverlaySpeedFactor(void)
{
	return settings->value("motion/overlaySpeedFactor", 0.1).toDouble();
}

void Configuration::setOverlaySpeedFactor(const double factor)
{
	settings->setValue("motion/overlaySpeedFactor", factor);
}



double Configuration::getMaximumWheelSpeed(void)
{
	return fabs(settings->value("motion/maximumWheelSpeed", 2.0).toDouble());
}
void Configuration::setMaximumWheelSpeed(const double speed)
{
	settings->setValue("motion/maximumWheelSpeed", speed);
}



double Configuration::getAxisLength(void)
{
	return settings->value("robot/axisLength", 0.595).toDouble();
}
void Configuration::setAxisLength(const double axislength)
{
	settings->setValue("robot/axisLength", axislength);
}


int Configuration::getCanServerIP(void)
{
	QHostAddress address(settings->value("server/can/ip", "127.0.0.1").toString());
	return address.toIPv4Address();
}
void Configuration::setCanServerIP(const QString ip)
{
	settings->setValue("server/can/ip", ip);
}


int Configuration::getCanServerPort(void)
{
	return settings->value("server/can/port", 4321).toInt();
}
void Configuration::setCanServerPort(const int port)
{
	settings->setValue("server/can/port", port);
}

int Configuration::getMobileDServerPort()
{
	return settings->value("server/mobiled/port", 9002).toInt();
}
void Configuration::setMobileDServerPort(const int port)
{
	settings->setValue("server/mobiled/port", port);
}


QString Configuration::getFeaturePositionFile()
{
	return QString("features.map");
}

QString Configuration::getKnownLaserMarksFilename(void)
{
	return settings->value("files/lasermarks", "/usr/src/politbuero/mobiled/data/map.marks").toString();
}
void Configuration::setKnownLaserMarksFilename(const QString filename)
{
	settings->setValue("files/lasermarks", filename);
}

QString	Configuration::getLinesFilename(void)
{
	return settings->value("files/lines", "/usr/src/politbuero/mobiled/data/F3xx.lines").toString();
}
void	Configuration::setLinesFilename(const QString filename)
{
	settings->setValue("files/lines", filename);
}


Pose Configuration::getLaserScannerPose(RobotSide side)
{
	if(side == sideFront)
	{
		return Pose
			(
				settings->value("laserscanners/frontpose/x", 0.00).toDouble(),
				settings->value("laserscanners/frontpose/y", 0.33).toDouble(),
				Conversion::deg2rad(settings->value("laserscanners/frontpose/angle", 0.0).toDouble())
			);
	}

	if(side == sideRear)
	{
		return Pose
			(
				settings->value("laserscanners/rearpose/x", 0.00).toDouble(),
				settings->value("laserscanners/rearpose/y", -0.33).toDouble(),
				Conversion::deg2rad(settings->value("laserscanners/rearpose/angle", 180.0).toDouble())
			);
	}

	Pose pose;
	pose.setValid(false);
	return pose;
}

void Configuration::setLaserScannerPose(RobotSide side, const Pose &pose)
{
	if(side == sideFront)
	{
		settings->setValue("laserscanners/frontpose/x", pose.getX());
		settings->setValue("laserscanners/frontpose/y", pose.getY());
		settings->setValue("laserscanners/frontpose/angle", pose.getAngleDeg());
	}

	if(side == sideRear)
	{
		settings->setValue("laserscanners/rearpose/x", pose.getX());
		settings->setValue("laserscanners/rearpose/y", pose.getY());
		settings->setValue("laserscanners/rearpose/angle", pose.getAngleDeg());
	}

}

int Configuration::getNumberOfLaserScanners(void)
{
	return settings->value("laserscanners/amount", 2).toInt();
}

RobotSide Configuration::mapIndexToScannerSide(int index)
{
	if(index == 0) return sideFront;
	if(index == 1) return sideRear;

	return sideInvalid;

}

float Configuration::getCollisionAvoidanceLookAheadDistance(void)
{
	return settings->value("collisionAvoidance/lookAhead/distance", 1.5).toDouble();
}
void Configuration::setCollisionAvoidanceLookAheadDistance(const float distance)
{
	settings->setValue("collisionAvoidance/lookAhead/distance", distance);
}


float Configuration::getCollisionAvoidanceLookAheadInterval(void)
{
	return settings->value("collisionAvoidance/lookAhead/interval", 0.1).toDouble();
}
void Configuration::setCollisionAvoidanceLookAheadInterval(const float interval)
{
	settings->setValue("collisionAvoidance/lookAhead/interval", interval);
}

int Configuration::getCollisionAvoidanceUpdateInterval(void)
{
	// milliseconds
	return settings->value("collisionAvoidance/updateInterval", 1000).toInt();
}
void Configuration::setCollisionAvoidanceUpdateInterval(const int interval)
{
	// milliseconds
	settings->setValue("collisionAvoidance/updateInterval", interval);
}

float Configuration::getRobotRadius(void)
{
	return settings->value("robot/width", 0.55).toDouble();
}
void Configuration::setRobotRadius(const float width)
{
	settings->setValue("robot/width", width);
}

// in meters/second^2
float Configuration::getAbortMotionDeceleration(void)
{
	return settings->value("motion/abortionBrakeDeceleration", 0.25).toDouble();
}
void Configuration::setAbortMotionDeceleration(const float speed)
{
	settings->setValue("motion/abortionBrakeDeceleration", speed);
}

int Configuration::getPathPlannerPollInterval(void)
{
	return settings->value("server/waypoint/pollInterval", 2).toInt();
}
void Configuration::setPathPlannerPollInterval(const int interval)
{
	settings->setValue("server/waypoint/pollInterval", interval);
}

QList<int> Configuration::getIoWarriorPortsLaserScanner(void)
{
	QList<int> ports;
	ports << 3;
	ports << 4;

	return ports;
}

int Configuration::getIoWarriorPortPanTiltUnit(void)
{
	return settings->value("iowarrior/port/panTiltUnit", 0).toInt();
}

int Configuration::getIoWarriorPortBarretHand(void)
{
	return settings->value("iowarrior/port/barretHand", 1).toInt();
}

int Configuration::getIoWarriorPortMotorFan(void)
{
	return settings->value("iowarrior/port/motorFan", 2).toInt();
}

float Configuration::getPathOptimizationInterval(void)
{
	return settings->value("motion/pathOptimizationInterval", 1.0).toDouble();
}

void Configuration::setPathOptimizationInterval(float value)
{
	settings->setValue("motion/pathOptimizationInterval", value);
}

float Configuration::getMaximumSpeedCorrectionFactor(void)
{
	return settings->value("motion/maximumSpeedCorrectionFactor", 2.5).toDouble();
}
void Configuration::setMaximumSpeedCorrectionFactor(float value)
{
	settings->setValue("motion/maximumSpeedCorrectionFactor", value);
}

int Configuration::getSplineMappingSampleCount() const
{
	return settings->value("motion/splineMappingSampleCount", 10000).toInt();
}

void Configuration::setSplineMappingSampleCount(int value)
{
	settings->setValue("motion/splineMappingSampleCount", value);
}


int Configuration::getObstacleTimeout(void)
{
	return settings->value("pathplanner/obstacletimeout", 120000).toInt();
}
void Configuration::setObstacleTimeout(const int value)
{
	settings->setValue("pathplanner/obstacletimeout", value);
}





QString Configuration::getRobotSide(const RobotSide side)
{
	if(side == sideFront)
		return "front";
	if(side == sideRear)
		return "rear";
	if(side == sideLeft)
		return "left";
	if(side == sideRight)
		return "right";

	return "invalid side!";
}
