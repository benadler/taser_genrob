#ifndef SPLINE_H
#define SPLINE_H

#include <QList>
#include <QPoint>
#include <QPointF>
#include <QDebug>
#include <QPainterPath>

#include <math.h>	// atan(), M_PI, floor() etc.

#include "polynom.h"
#include "conversion.h"
#include "logger.h"
#include "motion.h"

enum MotionPhase
{
		rotatingStart,
		rotatingEnd,
		movingSpline
};

class Spline : public Motion
{
	protected:
		MotionPhase currentPhase;		///< in what phase the spline-motion currently is.
		float orientationEnd;			///< what orientation should the robot assume after having reached the target position?

		float lastSpeedL, lastSpeedR;		///< what were the last assigned wheelspeeds?
		QTime timeStamp;			///< when were the wheelSpeeds last assigned? Used to advance the spline (in phase "moving") by a correct length.

		QList<float> mapLengthT;		///< This list maps values of splinelengths to t, because t = 0.5 doesn't mean we're in the middle of the spline's length.
		int mappingPrecision;		///< This variable determines how precise (=numerous) the mapping Length <-> T is.
		mutable int lastSearchIndex;		///< This value holds the index of mapLengthT where the last desired distance was found. Used for caching/performance.
		mutable float lastSearchDistance;	///< This value holds the distance that was passed to distanceToT at the last call. Used for caching/performance.
		float cursor;				///< This variable holds the position of the robot on the spline, (e.g. 2.34531 meters into the spline)
		float length;				///< The length of the spline is cached here, as its calculation is quite expensive.

		QList<QPointF> points;			///< A list containing the 2D data points

		///
		/// This method takes a *global* t from 0...1 and splits it up into
		/// - an polynom/interval number
		/// - a *local* t for that polynom/interval.
		///
		/// The first parameter is read, the last two are then written to.
		///
		/// @param t_in a value between 0 and 1, where 0 means start of spline and 1 means end of spline.
		/// @param t_out a value between 0 and 1, where 0 means start of the polynom and 1 means end of the polynom
		/// @param polynomNumber_out the polynom/segment/interval number for which t_out is valid.
		///
		void t2tn(double t_in, double &t_out, int &polynomNumber_out) const;

		///
		/// This method takes a distance in meters and returns the corresponding t.
		/// Not const as it modifies a few variables while caching results.
		/// @param distance the position on the spline in meters
		/// @return the corresponding value of t
		///
		double distanceToT(float distance) const;

		///
		/// This method returns a cubic-spline interpolated point.
		/// If a 0<t<1 was given, this is used for the position on the spline.
		/// If no t was given, the spline's current cursor-position is used instead.
		/// @param t the position on the spline, between 0 and 1
		/// @return the cubic-interpolated 2D spline-value.
		///
		virtual QPointF evaluate0t(double t) const = 0;

		void stepRotatingStart(const Pose &pose, float &speedL, float &speedR);
		bool stepRotatingEnd  (const Pose &pose, float &speedL, float &speedR);

	public:
		///
		/// A constructor.
		/// @param points all data points making up the spline. Unit is meters. These points can NOT be changed lateron.
		/// @param orientationEnd when used as a motion, what orientation (in degrees) should the robot have after completing the spline?
		///
		Spline(const QList<QPointF> &points, const float orientationEnd = 0.0);
		Spline(const Spline &spline);

		///
		/// The destructor
		///
		virtual ~Spline(void);

		///
		/// This method returns a cubic-spline interpolated point.
		/// If a 0<t<1 was given, this is used for the position on the spline.
		/// If no t was given, the spline's current cursor-position is used instead.
		/// @param distance the position on the spline in meters, between 0 and getLength()
		/// @return the cubic-interpolated 2D spline-value.
		///
		virtual QPointF evaluate0(float distance = -1.0) const = 0;

		///
		/// This method returns the angle of the spline's tangent at a given point
		/// @param distance the position on the spline in meters, between 0 and getLength()
		/// @return the angle of the spline at the point t in degrees
		///
		double getAngleDeg(float distance = -1.0) const;

		///
		/// This method returns the angle of the spline's tangent at a given point
		/// @param distance the position on the spline in meters, between 0 and getLength()
		/// @return the angle of the spline at the point t in radians
		///
		virtual double getAngleRad(float distance = -1.0) const = 0;

		///
		/// This method returns the waypoints that this curve is built upon.
		/// @return A QList containing the QPoint-waypoints
		///
		QList<QPointF> getWayPoints(void) const;

		///
		/// This method advances the "spline cursor" by distance meters
		/// @param distance how much to advance the cursor/robot on the spline. Unit is meters.
		/// @return true if successful, false otherwise (i.e. when distance > splinelength)
		///
		bool advance(float distance);

		///
		/// This method sets the "spline cursor" to distance meters
		/// @param distance where to set the cursor/robot on the spline. Unit is meters.
		/// @return true if successful, false otherwise (e.g. when distance > splinelength)
		///
		bool setCursor(float distance);

		///
		/// This method returns the spline's cursor
		/// @return distance where the cursor/robot currently is on the spline. Unit is meters.
		///
		float getCursor(void) const;

		///
		/// This method returns the spline's pathlength. Its not const, as it caches the length,
		/// because calculating the length is computationally expensive.
		/// @return the spline's pathlength in meters
		///
		float getLength(void);

		/// These methods are actually needed in the Motion-inheritance, but it is common to all Splines.
		QList<QPointF> getPathPoints(const float lookAheadDistance = -1.0, const float lookAheadInterval = -1.0) const;

		MotionType getType(void) const;
};

#endif
