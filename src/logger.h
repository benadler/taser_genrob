#ifndef DEBUG_H
#define DEBUG_H

#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include <QMutex>
#include <QMutexLocker>

#define DOLOGGING {va_list ap;va_start(ap, fmt);print(fmt, ap);va_end(ap);}

class Logger
{
	private:
		static Logger* singleton;	///< pointer to the only instance of Logger (which is a singleton class).
		static QMutex singletonMutex;
		QMutex *mutex;			///< used for thread synchronization

		///
		/// The default constructor does not take any arguments and is private, since Logger is a singleton.
		///
		Logger(void);

		///
		/// The default destructor does not take any arguments and is private, since Logger is a singleton.
		///
		~Logger(void);

		static bool logBeeperThread;	///< whether to print BeeperThread log-messages. Will be inited by setupLogging();
		static bool logCan;		///< whether to print Can log-messages. Will be inited by setupLogging();
		static bool logCanDevice;	///< whether to print CanDevice log-messages. Will be inited by setupLogging();
		static bool logCollisionAvoidance;	///< whether to print CanDevice log-messages. Will be inited by setupLogging();
		static bool logConversion;	///< whether to print Conversion log-messages. Will be inited by setupLogging();
		static bool logCorrespondence;	///< whether to print CanDevice log-messages. Will be inited by setupLogging();
		static bool logBattery;		///< whether to print Battery log-messages. Will be inited by setupLogging();
		static bool logDrive;		///< whether to print Drive log-messages. Will be inited by setupLogging();
		static bool logJava;		///< whether to print Java log-messages. Will be inited by setupLogging();
		static bool logLaserScan;	///< whether to print LaserScan log-messages. Will be inited by setupLogging();
		static bool logLaserMark;	///< whether to print LaserMark log-messages. Will be inited by setupLogging();
		static bool logLaserScanner;	///< whether to print LaserScanner log-messages. Will be inited by setupLogging();
		static bool logLocalization;	///< whether to print Localization log-messages. Will be inited by setupLogging();
		static bool logMobileD;		///< whether to print MobileD log-messages. Will be inited by setupLogging();
		static bool logMobileDClient;	///< whether to print MobileDClient log-messages. Will be inited by setupLogging();
		static bool logMotion;	///< whether to print Motion log-messages. Will be inited by setupLogging();
		static bool logNetworkServer;	///< whether to print NetworkServer log-messages. Will be inited by setupLogging();
		static bool logPacket;		///< whether to print Packet log-messages. Will be inited by setupLogging();
		static bool logRobot;		///< whether to print Robot log-messages. Will be inited by setupLogging();
		static bool logSignalThread;	///< whether to print SignalThread log-messages. Will be inited by setupLogging();
		static bool logSpline;		///< whether to print Spline log-messages. Will be inited by setupLogging();
		static bool logUdpClient;	///< whether to print UdpClient log-messages. Will be inited by setupLogging();
		static bool logCanServer;	///< whether to print CanServer log-messages. Will be inited by setupLogging();
		static bool logSimulator;	///< whether to print Simulator log-messages. Will be inited by setupLogging();
		static bool logRemoteControl;	///< whether to print RemoteControl log-messages. Will be inited by setupLogging();
		static bool logWayPointServer;	///< whether to print WayPointServer log-messages. Will be inited by setupLogging();

		///
		/// Simply writes the given string to stdout like printf, just with timestamp prepended.
		///
		static void log(const char *fmt, ...);

		///
		/// Just like log(), but needs a va_list passed. Called by the public debug functions,
		/// because they need to forward all their parameters, and this is the way to do it.
		/// http://www.thescripts.com/forum/thread212668.html
		///
		static void print(const char *fmt, va_list args);

	public:
		///
		/// This static method returns a pointer to the only Logger-object in existence. If there is no
		/// Logger-object yet, one will be created before returning a pointer to it.
		///
		static Logger* instance(void);

		static void setupLogging(int argc, char* argv[]);

		// used for logging.
		void BeeperThread(const char *fmt, ...){QMutexLocker locker(mutex);if(logBeeperThread)DOLOGGING}
		void Can(const char *fmt, ...){QMutexLocker locker(mutex);if(logCan)DOLOGGING}
		void CanDevice(const char *fmt, ...){QMutexLocker locker(mutex);if(logCanDevice)DOLOGGING}
		void CollisionAvoidance(const char *fmt, ...){QMutexLocker locker(mutex);if(logCollisionAvoidance)DOLOGGING}
		void Conversion(const char *fmt, ...){QMutexLocker locker(mutex);if(logConversion)DOLOGGING}
		void Correspondence(const char *fmt, ...){QMutexLocker locker(mutex);if(logCorrespondence)DOLOGGING}
		void Battery(const char *fmt, ...){QMutexLocker locker(mutex);if(logBattery)DOLOGGING}
		void Drive(const char *fmt, ...){QMutexLocker locker(mutex);if(logDrive)DOLOGGING}
		void Java(const char *fmt, ...){QMutexLocker locker(mutex);if(logJava)DOLOGGING}
		void LaserMark(const char *fmt, ...){QMutexLocker locker(mutex);if(logLaserMark)DOLOGGING}
		void LaserScan(const char *fmt, ...){QMutexLocker locker(mutex);if(logLaserScan)DOLOGGING}
		void LaserScanner(const char *fmt, ...){QMutexLocker locker(mutex);if(logLaserScanner)DOLOGGING}
		void Localization(const char *fmt, ...){QMutexLocker locker(mutex);if(logLocalization)DOLOGGING}
		void MobileD(const char *fmt, ...){QMutexLocker locker(mutex);if(logMobileD)DOLOGGING}
		void MobileDClient(const char *fmt, ...){QMutexLocker locker(mutex);if(logMobileDClient)DOLOGGING}
		void Motion(const char *fmt, ...){QMutexLocker locker(mutex);if(logMotion)DOLOGGING}
		void NetworkServer(const char *fmt, ...){QMutexLocker locker(mutex);if(logNetworkServer)DOLOGGING}
		void Packet(const char *fmt, ...){QMutexLocker locker(mutex);if(logPacket)DOLOGGING}
		void Robot(const char *fmt, ...){QMutexLocker locker(mutex);if(logRobot)DOLOGGING}
		void Spline(const char *fmt, ...){QMutexLocker locker(mutex);if(logSpline)DOLOGGING}
		void SignalThread(const char *fmt, ...){QMutexLocker locker(mutex);if(logSignalThread)DOLOGGING}
		void UdpClient(const char *fmt, ...){QMutexLocker locker(mutex);if(logUdpClient)DOLOGGING}
		void CanServer(const char *fmt, ...){QMutexLocker locker(mutex);if(logCanServer)DOLOGGING}
		void Simulator(const char *fmt, ...){QMutexLocker locker(mutex);if(logSimulator)DOLOGGING}
		void RemoteControl(const char *fmt, ...){QMutexLocker locker(mutex);if(logRemoteControl)DOLOGGING}
		void WayPointServer(const char *fmt, ...){QMutexLocker locker(mutex);if(logWayPointServer)DOLOGGING}
};

void abort(const char *fmt, ...);

#endif
