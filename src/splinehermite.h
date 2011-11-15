#ifndef SPLINEHERMITE_H
#define SPLINEHERMITE_H

#include "spline.h"

class SplineHermite : public Spline
{
	private:
		double tension, bias;
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

		// Tension: 1 is high, 0 normal, -1 is low
		// Bias: 0 is even, positive towards first segment, negative towards the other
		double hermiteInterpolate0(double y0, double y1, double y2, double y3, double t) const;
		double hermiteInterpolate1(double y0, double y1, double y2, double y3, double t) const;

	public:
		///
		/// A constructor.
		/// @param points all data points making up the spline. Unit is meters. These points can NOT be changed lateron.
		/// @param orientationEnd when used as a motion, what orientation (in degrees) should the robot have after completing the spline?
		///
		SplineHermite(const QList<QPointF> &points, const float orientationEnd = 0.0);

		SplineHermite(const SplineHermite &spline);

		///
		/// The destructor
		///
		~SplineHermite(void);

		void setParameters(const double tension, const double bias);

		///
		/// This method returns a cubic-spline interpolated point.
		/// If a 0<t<1 was given, this is used for the position on the spline.
		/// If no t was given, the spline's current cursor-position is used instead.
		/// @param distance the position on the spline in micrometers, between 0 and getLength()
		/// @return the cubic-interpolated 2D spline-value.
		///
		QPointF evaluate0(float distance = -1.0) const;


		double getAngleRad(float distance = -1.0) const;

		// These following methods make SplineHermite also a Motion

		/// This method is called in subclasses. It gets a pose and is supposed to set the wheelSpeeds.
		bool step(const Pose &pose, float &speedL, float &speedR);

		/// See base class Motion for documentation
// 		QList<QPointF> getPathPoints(const float lookAheadDistance = -1.0, const float lookAheadInterval = -1.0) const;
};

#endif
