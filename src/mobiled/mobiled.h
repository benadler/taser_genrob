#ifndef MOBILED_H
#define MOBILED_H

#include <QPointF>
#include <QTimer>
#include <QDebug>
#include <QMutex>
#include <QMutexLocker>
#include <QCoreApplication>

#include <signal.h>
#include <sys/signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>

#include "configuration.h"
#include "networkserver.h"
#include "laserscan.h"
#include "laserscanner.h"
#include "lasermark.h"
#include "localization.h"
#include "collisionavoidance.h"
#include "packet.h"
#include "robot.h"
#include "motion.h"
#include "splinecubic.h"
#include "splinehermite.h"
#include "translation.h"
#include "rotation.h"
#include "pose.h"
#include "java.h"

/// @class MobileD
/// @brief This is the main mobileDaemon class.

class NetworkServer;
class CollisionAvoidance;
class LaserScanner;

class MobileD : public QCoreApplication
{
	Q_OBJECT

	private:
		QMutex* mutexThread;
		Logger* logger;
		Configuration* config;
		Localization* localization;
		NetworkServer* networkServer;
		CollisionAvoidance* collisionAvoidance;
		LaserScanner *laserScanner;
		Motion* motion;
		Robot* robot;
		Java* java;
		
bool getSign(float value);

	bool timeInited;
	struct timeval start;

		Pose poseTarget;		///< only used when moving splines, as we need to keep the targetOrientation somewhere.
		QTimer* stepMotionTimer;
		bool pathIsBlocked;
		float speedL, speedR;		///< Speeds at which the robot SHOULD be moving. These are set by the Motion::step() method and sent to the robot.

	private slots:
		void slotSendWayPointRequest(void);
		void slotNewWayPointsArrived(QList<QPointF> wayPoints);
		void slotStepMotion(void);
		void slotShutDown();

		/// This method is called by CollisionAvoidance when an obstacle appears on our path.
		/// I might add a pathDistanceToObstacle-parameter, so we can determine deceleration speed or whatever.
		void slotAlertPathBlocked(void);

	public slots:
		/// Aborts the current motion, slows the robot down, applies
		/// the brakes and will then go to robotStatus idle.
		void slotAbortCurrentMotion(void);

	public:
		MobileD(int argc, char** argv);
		~MobileD(void);

		/// Makes the robot move from currrent Pose to target Pose. The path is not
		/// explicitly defined when this method is called, genPath will give us the
		/// path necessary to move from current Pose to target.
		/// @param target where the robot should move
		/// @return whether the robot accepted the command. True when robotStatus == idle, else false.
		bool moveSpline(const Pose target);

		// Makes the robot move from currrent Pose to each of the given wayPoints, ending in the last with targetOrientationRad
		bool moveSpline(const QList<QPointF> &wayPoints, const float targetOrientationRad);

		bool moveTranslate(const float distance);
		bool moveRotate(const float angle);

		Motion* getCurrentMotion() const;

	signals:
		void sendWayPointRequestToJava(Pose, Pose);
};

#endif
