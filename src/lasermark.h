#ifndef LASERMARK_H
#define LASERMARK_H

#include <QList>
#include <QDateTime>

#include <math.h>

#include "configuration.h"

/// @brief This class represents one lasermark
/// LaserMarks are pieces of reflective tape that are installed at known
/// places within the building. By finding marks (and mapping them to known marks), the robot
/// can localize itself.
/// Angle values are in radiants, distances are either float-meters or int-millimeters.

class LaserMark
{
	protected:
		Logger* logger;

	public:
		/// Constructors.
		LaserMark(void);

		/// Destructor.
		~LaserMark(void);
};

/// @brief This class represents one lasermark known to exist on the map
/// LaserMarks are pieces of reflective tape that are installed at known places within the
/// building. By finding marks (and mapping them to known marks), the robot can localize
/// itself.
///
/// The position of the known mark is stored in cartesic world coordinates using a QPointF,
/// which uses float values.
///
/// A LaserMarkKnown with x/y = 2.5/-1.3 means the mark is located at exactly that position
/// on the map.

class LaserMarkKnown : public LaserMark
{
	private:
// 		int x, y;
// 		float angleOrientation, angleOpening;
// 		QString comment;
		QPointF worldCoordinate;

	public:
		/// Constructors.
		LaserMarkKnown(void);
		LaserMarkKnown(const QPointF worldCoordinate);

		/// Destructor.
		~LaserMarkKnown(void);

		QPointF	getWorldCoordinate(void) const;
		void	setWorldCoordinate(const QPointF worldCoordinate);

		QString toString(void) const;
};

/// @brief This class represents one lasermark that was detected by a laserscanner
/// LaserMarks are pieces of reflective tape that are installed at known places within the
/// building. By finding marks (and mapping them to known marks), the robot can localize
/// itself.
///
/// The position of the measured mark is stored in cartesic platform coordinates using a
/// QPointF, which uses float values.
///
/// A LaserMarkMeasured with x/y = 2.5/-1.3 means the mark was detected 2.5m to the right
/// and 1.3 meters behind the robot.

class LaserMarkMeasured : public LaserMark
{
	private:
		QPointF platformCoordinate;

		QDateTime timeStamp;	/// when the mark was detected.

	public:
		/// Constructors.
		LaserMarkMeasured(const QPointF platformCoordinate);

		/// Destructor.
		~LaserMarkMeasured(void);

		QPointF getPlatformCoordinate(void) const;
		void setPlatformCoordinate(const QPointF platformCoordinate);

		QDateTime getTimeStamp(void) const;
		int getAge(void) const;

		QString toString(void) const;
};

#endif
