#include "lasermark.h"

LaserMark::LaserMark(void)
{
	logger = Logger::instance();
// 	logger->LaserMark("LaserMark::LaserMark(void)");
}

LaserMarkMeasured::LaserMarkMeasured(const QPointF platformCoordinate)
{
	logger = Logger::instance();
// 	logger->LaserMark("LaserMarkMeasured::LaserMarkMeasured(QPointF)");

	this->platformCoordinate = platformCoordinate;
	this->timeStamp = QDateTime::currentDateTime();
}

LaserMarkKnown::LaserMarkKnown(const QPointF worldCoordinate)
{
	logger = Logger::instance();
	this->worldCoordinate = worldCoordinate;
}

LaserMarkKnown::LaserMarkKnown(void)
{
	logger = Logger::instance();
// 	logger->LaserMark("LaserMarkKnown::LaserMarkKnown(void)");
}

LaserMark::~LaserMark(void)
{
// 	logger->LaserMark("LaserMark::~LaserMark()");
}

LaserMarkKnown::~LaserMarkKnown(void)
{
// 	logger->LaserMark("LaserMarkKnown::~LaserMarkKnown()");
}

LaserMarkMeasured::~LaserMarkMeasured(void)
{
// 	logger->LaserMark("LaserMarkMeasured::~LaserMarkMeasured()");
}

// LaserMarkKnown
QPointF LaserMarkKnown::getWorldCoordinate(void) const
{
	return worldCoordinate;
}

void LaserMarkKnown::setWorldCoordinate(const QPointF worldCoordinate)
{
	this->worldCoordinate = worldCoordinate;
}

QString	LaserMarkKnown::toString(void) const
{
	return QString("worldX %1, worldY %2").arg(worldCoordinate.x()).arg(worldCoordinate.y());
}

// LaserMarkMeasured
QPointF LaserMarkMeasured::getPlatformCoordinate(void) const
{
	return platformCoordinate;
}

void LaserMarkMeasured::setPlatformCoordinate(const QPointF platformCoordinate)
{
	this->platformCoordinate = platformCoordinate;
}

QDateTime LaserMarkMeasured::getTimeStamp(void) const
{
	return timeStamp;
}

int LaserMarkMeasured::getAge(void) const
{
	return timeStamp.secsTo(QDateTime::currentDateTime());
}

QString LaserMarkMeasured::toString(void) const
{
	return QString("platformX %1, platformY %2").arg(platformCoordinate.x()).arg(platformCoordinate.y());
}
