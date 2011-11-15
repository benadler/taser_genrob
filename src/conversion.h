#ifndef CONVERSION_H
#define CONVERSION_H

#include <QPointF>
#include <QLineF>
#include <QDebug>

#include <math.h>

#include "configuration.h"
#include "pose.h"
#include "logger.h"

/// @class Conversion
/// @brief This class offers conversion methods (rad2deg, etc.)

class Pose;

class Conversion
{
	public:
		/// This method reads the curvature parameter and writes the resulting wheelSpeeds into
		/// argument 2 and 3. It uses the configured values for slowing down in curves etc.
		/// @param curvature the (spline's current) curvature, negative for left curves.
		/// @param speedL the resulting speed of the left wheel.
		/// @param speedR the resulting speed of the right wheel.
		static void curvature2wheelSpeeds(const double curvature, float &speedL, float &speedR);
		static void correctWheelSpeeds(const Pose poseCurrent, const QPointF positionShould, float &speedL, float &speedR);

		static void diff2rotran(const int speedL, const int speedR, float &speedTranslation, float &speedRotation);

		static double distanceBetween(const QPointF &a, const QPointF &b);

		static double deg2rad(double degree);
		static double rad2deg(double radiant);
};

#endif
