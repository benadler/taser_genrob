#ifndef SPLINECUBIC_H
#define SPLINECUBIC_H

#include "polynom.h"
#include "spline.h"

class SplineCubic : public Spline
{
	private:
		Polynom *xf, *yf;	///< Two polynom-pointers for a 2-dimensional spline. We store two polygons for each spline-segment.

		/// This method constructs the polynoms based on the given set of points.
		/// @param points the 1-dimensional data points. Unit is micrometers.
		/// @param f the polynom to be constructed for these data points
		///
		void buildPolynoms(QList<float> points, Polynom *f);

		///
		/// This method returns a cubic-spline interpolated point.
		/// If a 0<t<1 was given, this is used for the position on the spline.
		/// If no t was given, the spline's current cursor-position is used instead.
		/// @param t the position on the spline, between 0 and 1
		/// @return the cubic-interpolated 2D spline-value.
		///
		QPointF evaluate0t(double t) const;

		///
		/// This method returns a cubic-spline interpolated point.
		/// If a 0<t<1 was given, this is used for the position on the spline.
		/// If no t was given, the spline's current cursor-position is used instead.
		/// @param t the position on the spline, between 0 and 1
		/// @return the cubic-interpolated 2D spline-value.
		///
		QPointF evaluate1t(double t) const;

		///
		/// This method returns a cubic-spline interpolated point.
		/// If a 0<t<1 was given, this is used for the position on the spline.
		/// If no t was given, the spline's current cursor-position is used instead.
		/// @param t the position on the spline, between 0 and 1
		/// @return the cubic-interpolated 2D spline-value.
		///
		QPointF evaluate2t(double t) const;

		///
		/// This method returns the spline's curvature at a given point.
		/// @param distance the position on the spline in micrometers, between 0 and getLength()
		/// @return the spline's curvature at that given point.
		///
		double getCurvature(float distance = -1.0) const;

	public:
		///
		/// A constructor.
		/// @param points all data points making up the spline. Unit is meters. These points can NOT be changed lateron.
		/// @param orientationEnd when used as a motion, what orientation (in degrees) should the robot have after completing the spline?
		///
		SplineCubic(QList<QPointF> &points, const float orientationEnd = 0.0);

		SplineCubic(const SplineCubic &spline);

		///
		/// The destructor
		///
		~SplineCubic(void);

		///
		/// This method returns a cubic-spline interpolated point.
		/// If a 0<t<1 was given, this is used for the position on the spline.
		/// If no t was given, the spline's current cursor-position is used instead.
		/// @param distance the position on the spline in micrometers, between 0 and getLength()
		/// @return the cubic-interpolated 2D spline-value.
		///
		QPointF evaluate0(float distance = -1.0) const;

		///
		/// This method returns the first derivation.
		/// If a 0<t<1 was given, this is used for the position on the spline.
		/// If no t was given, the spline's current cursor-position is used instead.
		/// @param distance the position on the spline in micrometers, between 0 and getLength()
		/// @return the first derivation of the cubic-interpolated 2D spline-value.
		///
		QPointF evaluate1(float distance = -1.0) const;

		///
		/// This method returns the second derivation.
		/// If a 0<t<1 was given, this is used for the position on the spline.
		/// If no t was given, the spline's current cursor-position is used instead.
		/// @param distance the position on the spline in micrometers, between 0 and getLength()
		/// @return the second derivation of the cubic-interpolated 2D spline-value.
		///
		QPointF evaluate2(float distance = -1.0) const;

		double getAngleRad(float distance = -1.0) const;

		// These following methods make SplineCubic also a Motion

		/// This method is called in subclasses. It gets a pose and is supposed to set the wheelSpeeds.
		bool step(const Pose &pose, float &speedL, float &speedR);

		/// See base class Motion for documentation
// 		QList<QPointF> getPathPoints(const float lookAheadDistance = -1.0, const float lookAheadInterval = -1.0) const;
};

#endif
