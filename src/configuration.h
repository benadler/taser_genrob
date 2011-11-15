#ifndef CONFIGURATION_H
#define CONFIGURATION_H

/// @class Configuration
/// @brief The Configuration-class offers all configurable values for the robot and its subsystems
/// This class curently serves values that were defined at compile-time. Lateron, we want
/// to load a configuration-file and use its values instead.

#include <QString>
#include <QMutex>
#include <QTimer>
#include <QMutexLocker>
#include <QHostAddress>
#include <QSettings>

#include "pose.h"

class Pose;

enum RobotSide
{
	sideLeft,
	sideRight,
	sideFront,
	sideRear,

	sideInvalid
};


class Configuration : public QObject
{
	Q_OBJECT

	private:
		/// Constructor. Private, as this is a singleton
		Configuration();
		static Configuration* singleton;	///< pointer to the only instance of Configuration (which is a singleton class).
		static QMutex singletonMutex;

		QMutex* mutex;		///< used for thread-synchronization in member methods.

		QSettings *settings;
		QTimer* timerFlush;

		float overlaySpeedFactor;

	private slots:
		void slotSyncSettings();

	public:


		///
		/// This static method returns a pointer to the only Localization-object in existence. If there is no
		/// Localization-object yet, one will be created before returning a pointer to it.
		///
		static Configuration* instance(void);

		~Configuration();

		double	getWheelCircumference(const RobotSide side);
		void	setWheelCircumference(const RobotSide side, const double circumference);

		double	getAxisLength(void);
		void	setAxisLength(const double axislength);

		double	getMaximumWheelSpeed(void);
		void	setMaximumWheelSpeed(const double wheelspeed);

		double	getSmallestHighSpeedCurveRadius(void);
		void	setSmallestHighSpeedCurveRadius(const double radius);

		double	getSpeedFactorWhenOnlyRotating(void);
		void	setSpeedFactorWhenOnlyRotating(double factor);

		double	getOverlaySpeedFactor(void);
		void	setOverlaySpeedFactor(const double factor);

		int	getCanServerIP(void);
		void	setCanServerIP(const QString ip);

		int	getCanServerPort();
		void	setCanServerPort(const int port);

		int	getMobileDServerPort();
		void	setMobileDServerPort(const int port);

		Pose	getLaserScannerPose(RobotSide side);
		void	setLaserScannerPose(RobotSide side, const Pose &pose);

		int	getNumberOfLaserScanners(void);
		RobotSide mapIndexToScannerSide(int index);

		QString	getFeaturePositionFile();

		QString	getKnownLaserMarksFilename(void);
		void	setKnownLaserMarksFilename(const QString filename);

		QString	getLinesFilename(void);
		void	setLinesFilename(const QString filename);

		float	getRobotRadius(void);
		void	setRobotRadius(const float width);

		float	getAbortMotionDeceleration(void);
		void	setAbortMotionDeceleration(const float speed);

		float	getCollisionAvoidanceLookAheadDistance(void);
		void	setCollisionAvoidanceLookAheadDistance(const float distance);

		float	getCollisionAvoidanceLookAheadInterval(void);
		void	setCollisionAvoidanceLookAheadInterval(const float interval);

		int	getCollisionAvoidanceUpdateInterval(void);
		void	setCollisionAvoidanceUpdateInterval(const int interval);

		int	getPathPlannerPollInterval(void);
		void	setPathPlannerPollInterval(const int interval);

		QList<int> getIoWarriorPortsLaserScanner(void);
		int	getIoWarriorPortPanTiltUnit(void);
		int	getIoWarriorPortBarretHand(void);
		int	getIoWarriorPortMotorFan(void);

		float	getPathOptimizationInterval(void);
		void	setPathOptimizationInterval(float);

		float	getMaximumSpeedCorrectionFactor(void);
		void	setMaximumSpeedCorrectionFactor(float);

		int	getSplineMappingSampleCount() const;
		void	setSplineMappingSampleCount(int);
		
		int	getObstacleTimeout(void);
		void	setObstacleTimeout(const int value);

		static QString getRobotSide(const RobotSide side);


};

#endif
