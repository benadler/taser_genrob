#ifndef COLLISIONAVOIDANCE_H
#define COLLISIONAVOIDANCE_H

#include <QList>
#include <QPoint>
#include <QTransform>

#include "logger.h"
#include "pose.h"
#include "robot.h"
#include "spline.h"
#include "mobiled.h"
#include "java.h"
#include "localization.h"
#include "laserscan.h"
#include "beeperthread.h"

class MobileD;

class CollisionAvoidance : public QObject
{
	Q_OBJECT

	public:
		/// This static method returns a pointer to the only CollisionAvoidance-object in existance. If there is no
		/// CollisionAvoidance-object yet, one will be created before returning a pointer to it.
		static CollisionAvoidance* instance(void);

		/// This method is the heart of collision avoidance. It asks Localization for our current Pose,
		/// knows the currently driven spline and gets LaserScan data. It calculates whether we'll have
		/// an obstacle on our path. If yes, it adds the obstacle to the map and alerts mobileD to get
		/// a new path from the current Pose to our target.
		void updateObstacles(const LaserScan *scan);

		void setActive(const bool active);
		bool isActive(void) const;

		void setMobileD(MobileD* mobileD);

	private:
		static CollisionAvoidance* singleton;	///< pointer to the only instance of Localization (which is a singleton class).
		static QMutex singletonMutex;

		QMutex* mutex;		///< used for thread-synchronization in member methods.

		Configuration* config;
		Localization* localization;
		MobileD* mobileD;	///< we need mobileD's address, so we can ask it for its current spline.
		Java* java;
		BeeperThread* beeperThread;
		Robot* robot;

		bool active;

		QList<float> scanAnglesFront, scanDistancesFront, scanAnglesRear, scanDistancesRear;

		QList<QPointF> pathPoints;	///< a list of points on the next getCollisionAvoidanceLookAheadDistance() meters on the robot's path
		QList<QPointF> obstacles;	///< a list of obstacles that are closer than getCollisionAvoidanceLookAheadDistance, extracted from the laserscan.

		/// Constructor.
		CollisionAvoidance(void);

		/// Destructor.
		~CollisionAvoidance(void);

		void checkCollisions(void);
		void updatePathPoints(void);
		float getSquaredDistance(const QPointF &a, const QPointF &b);

		Logger* logger;

	signals:
		void activationStatusChanged(bool);
		void alertPathBlocked(void);
};

#endif
