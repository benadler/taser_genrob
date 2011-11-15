#ifndef JAVA_H
#define JAVA_H


#include <signal.h>
#include <sys/signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>

#include <QPointF>
#include <QList>
#include <QMutex>
#include <QTimer>
#include <QString>
#include <QStringList>
#include <QMutexLocker>
#include <QThread>

#include <jni.h>


#include "pose.h"
#include "logger.h"
#include "lasermark.h"
#include "configuration.h"

/// @brief This class wraps the JNI, used to talk to genMap and genPath
///

class Java : public QThread
{
	Q_OBJECT

	private:
		static Java* singleton;	///< pointer to the only instance of Localization (which is a singleton class).
		static QMutex singletonMutex;

		Logger* logger;
		Configuration* config;
		QMutex* mutex;				///< to synchronize access from multiple threads.

		JavaVM* jvm;       // denotes a Java VM
		JNIEnv* env;       // pointer to native method interface
		JavaVMInitArgs vm_args; // JDK 1.1 VM initialization arguments
		JavaVMOption options[4];

		jclass robletObstacles;
		jmethodID robletObstaclesMethodAdd;
		jmethodID robletObstaclesMethodRemove;

		jclass robletPath;
		jmethodID robletPathMethodGetPath;

		jclass robletLaserMarks;
		jmethodID robletLaserMarksMethodGet;

		jclass robletIoWarrior;
		jmethodID robletIoWarriorMethodSetPort;

		/// Private constructor, as this is a singleton.
		Java(void);

		void initialize(void);

		void optimizeWayPoints(QList<QPointF> &points) const;

	public:
		///
		/// This static method returns a pointer to the only Java-object in existance. If there is no
		/// Java-object yet, one will be created before returning a pointer to it.
		///
		static Java* instance(void);

		/// Destructor. Used for shutting down.
		~Java(void);

		void run(void);





	public slots:
		void slotGetWayPoints(Pose start, Pose end);
		void slotRequestLaserMarksFromGenMap(void);
		void slotSetIoWarriorPort(const unsigned int port, const bool enable) const;
		void slotAddObstaclesToMap(const QList<QPointF> &obstacles) const;
		void slotRemoveObstaclesFromMap(const QList<QPointF> &obstacles) const;

	signals:
		///
		/// This signal contains a list of QPointFs with wayPoints. It is fired after this class was asked
		/// to find a route between start and end using sendWayPointRequest(). If the list is empty, then
		/// there is no route between start and end.
		/// @param wayPoints A list of wayPoints, including the two points "start" and "end" from the request.
		///
		void wayPointsReady(QList<QPointF> wayPoints);

		void laserMarksReady(QList<LaserMarkKnown> laserMarks);
};

#endif
