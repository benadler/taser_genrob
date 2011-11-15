#ifndef LASERSCAN_H
#define LASERSCAN_H

#include <QPointF>
#include <QList>
#include <QMutex>
#include <QMutexLocker>
#include <QUdpSocket>
#include <QDateTime>

#include "pose.h"
#include "conversion.h"
#include "configuration.h"
#include "lasermark.h"
#include "logger.h"

/// @brief This class represents one complete set of data from BOTH Sick LMS200 laserscanners.
///
/// Each laserscanner scans counterclockwise from 0 to 180 degrees in 0.5 degree steps. This
/// means we get 361 distance-values and 361 angle-values. The angle-values are offset by -90
/// degrees, so they go from -90 to +90 degrees.
///
/// A QMutex is used to sync access from different threads. This way, we can ensure that the
/// LaserScanner-thread won't write new scan-distances while NetworkServer is reading them.
///
/// LaserMarks are a different thing. Those are not stored here, have a look at lasermark.h.

class LaserScan
{
	private:
		Logger* logger;
		Configuration* config;

		QList<float>	scannerAnglesFront;	///< The angles in scanner-polar-coordinates, unit is radians.
		QList<float>	scannerDistancesFront;	///< The distances in scanner-polar-coordinates, unit is meters.
		QList<float>	scannerAnglesRear;	///< The angles in scanner-polar-coordinates, unit is radians.
		QList<float>	scannerDistancesRear;	///< The distances in scanner-polar-coordinates, unit is meters.

		QList<float>	platformAngles;		///< The angles in platform-polar-coordinates, unit is radians.
		QList<float>	platformDistances;	///< The distances in platform-polar-coordinates, unit is meters.

		QDateTime timeStamp;			///< when the scan data was received.
		QMutex* mutex;				///< to synchronize access from multiple threads.

		void updatePlatformCoordinates();

	public:
		/// Constructors.
		LaserScan(void);
		LaserScan(const LaserScan &other);

		/// Destructor.
		~LaserScan(void);

		void getScannerAngles(RobotSide side, QList<float> &angles) const;
		void getScannerDistances(RobotSide side, QList<float> &distances) const;

		void setScanData(const QList<float> &angles, const QList<float> &distances, const RobotSide scanner);

		/// This method returns the distance of the closest obstacle to the robot's center.
		float getClosestObstacleDistance(void);

		/// returns age of the scan data in seconds.
		int getAge(void) const;
};

#endif
