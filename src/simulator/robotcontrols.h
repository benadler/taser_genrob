#ifndef ROBOTCONTROLS_H
#define ROBOTCONTROLS_H

#include <QDockWidget>
#include <QTextEdit>
#include <QErrorMessage>
#include <QDebug>
#include <QTime>
#include <QFont>
#include <QMap>

#include <math.h>	// pow()

#include "ui_robotcontrol.h"

/// @class RobotControls
/// @brief This class is both a UI to control the robot as well as a state machine trying to simulate its behavior.

// http://techbase.kde.org/Development/Tutorials/Using_Qt_Designer

class RobotControls : public QDockWidget, public Ui_RobotControl
{
	Q_OBJECT

	private:
		int maxSpeed, speedLeft, speedRight;
		QMap<int, QTime> timeStamps;


	private slots:
		void slotSpeedBoxValuesChanged(void);
		void slotUpdateCurves(void);

		// this slot is called when value in the UI change. Causes the newPose() signal to be emitted.
		void slotPoseValuesChanged(void);

	public:
		RobotControls();
		~RobotControls();

		// in micrometers per second
		void setMotorSpeeds(int left, int right);

		// in micrometers per second
		void getMotorSpeeds(int &left, int &right) const;

		///
		/// This method returns the advances in micrometers for both wheel
		/// since the last call of this method with the same callerID.
		///
		void getMotorAdvances(int callerID, int &left, int &right);

		// returns the motor temperatures in microdegrees celsius. 35°C = 35.000.000
		void getMotorTemperatures(float &left, float &right) const;

		// returns battery voltage in volts.
		float getBatteryVoltage(void) const;

		// returns the remote control values.
		void getRemoteControl(int &steering, int &speed) const;

		void setBrakes(const bool enable);

	signals:
		void newPose(double x, double y, double angle);
		void showSplineCubic(bool, QList<QPointF>);
		void showSplineHermite(bool, QList<QPointF>, float tension, float bias);

	public slots:
		// call this to set the values in the UI.
		void slotSetPose(double x, double y, double angle);
};

#endif
