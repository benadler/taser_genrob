#ifndef ROBOT_H
#define ROBOT_H

#include <QUdpSocket>

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "configuration.h"
#include "localization.h"
#include "packet.h"
#include "logger.h"
#include "protocol_can.h"

///
/// @brief This class represents the robot and uses TCP/IP to talk to the can-server
///
///

// fucked up formatting, used because of latex \lstinputlisting
enum RobotStatus
{
	idle,		// 0	robot is bored.

	stalled,	// 1	robot is blocked. Waiting for new
			//	path and/or obstacle to disappear.

	movementSetup,	// 2	robot is waiting for initial path
			//	or is constructing motion profile.

	moving,		// 3	robot is moving, please don't talk
			//	to the driver.

	aborting	// 4	robot is braking, will then go idle.
			//	Reason is an obstacle or user abort.
};

class Robot : public QObject
{
	Q_OBJECT

	private:
		static Robot* singleton;	///< pointer to the only instance of Robot (which is a singleton class).
		static QMutex singletonMutex;

		QMutex* mutexThread;		///< used for thread-synchronization in member methods.
		QMutex* mutexRobot;		///< used to make sure that no two threads manipulate the robot at the same time.

		QTcpSocket* canSocket;		///< the socket used to talk to the CAN-server

		RobotStatus currentRobotStatus;	///< what the robot is currently doing
		bool brakesApplied;		///< whether the robot's brakes are currently applied. For internal use only.

		Configuration* config;

		Logger* logger;
		QTime timeStamp;

		/// Constructor. Private, as this is a singleton
		Robot(void);

		///
		/// enables / disables the robot's brakes using setEmergencyStop, as thats faster
		/// than using enableBrakes. This method is private, as we set brakes depending
		/// on the current RobotStatus ourselves.
		///
		void setBrakes(const bool enable);

		/// Called by two member methods when the robot's state has changed, this method
		/// will enable/disable the brakes depending on the new robot-state.
		void setBrakesToCurrentRobotState(void);

	private slots:
		void slotSocketStateChanged(QAbstractSocket::SocketState state);
		void slotIncomingCanPacket(void);

	public slots:
		/// Calling this slot makes Robot ask for the batteryVoltage using the CAN bus.
		/// When the reply comes in, the newBatteryVoltageReady(float) signal is emitted.
		void slotGetBatteryVoltage(void) const;

		/// Calling this slot makes Robot ask for the drive temperatures using the CAN bus.
		/// When the reply comes in, the newMotorTempsReady(float, float) signal is emitted.
		void slotGetDriveTemperatures() const;

		///
		/// This method is called to change the robot's status. For available statii(?),
		/// have a look at the RobotStatus enum. If the given RobotStatus should have
		/// the robot's brakes applied, this will be done automatically. If not, the
		/// brakes will be released.
		/// Its a slot, so it can be called indirectly from another thread. Thats impor-
		/// ant, as calling this method could release the brakes, which requires a TCP-
		/// packet to be sent to the CAN-Server, but we cannot initiate the sending from
		/// another thread: "QSocketNotifier: socket notifiers cannot be enabled from
		/// another thread". Whew.
		/// @param status the robot's new status
		///
		void slotSetStatus(const RobotStatus status);
	signals:
		void newBatteryVoltageReady(float);
		void newDriveTempsReady(float, float);
		void statusChanged(RobotStatus);

	public:
		///
		/// This static method returns a pointer to the only Robot-object in existence. If there is no
		/// Robot-object yet, one will be created before returning a pointer to it.
		/// @return a pointer to the only instance of the Robot-object.
		///
		static Robot* instance(void);

		/// Destruktor.
		~Robot(void);

		///
		/// This method locks the robotmutex. If it is already locked, the calling
		/// thread will block until the lock is unlocked by the thread that locked it.
		///
		void lock(void);

		///
		/// This method unlocks the robotmutex. The mutex can only be unlocked by the
		/// thread that locked it.
		///
		void unlock(void);

		///
		/// This method TRIES to lock the robotmutex and returns true if that succeeded.
		/// @return true when the robotmutex was successfully locked, else false.
		///
		bool tryLock(void);

		///
		/// This method sets the robot's new status ONLY IF the currentStatus is the expected status
		/// using an atomic operation. Have a look at the callers' comments to see why this is necessary.
		/// @return true if the expected status was found (and thus changed to newStatus), else false.
		///
		bool testAndSetStatus(const RobotStatus statusExpected, const RobotStatus statusNew);

		///
		/// This method returns the robot's current status. For available statii(?),
		/// have a look at the RobotStatus enum.
		/// @return the robot's current status
		///
		RobotStatus getStatus(void) const;

		///
		/// This method returns the robot's current status as text. Useful for debugging.
		/// @return the robot's current status as text.
		///
		QString getStatusText(void) const;
		QString getStatusText(RobotStatus status) const;

		///
		/// This method blocks the calling thread until the robot is idle.
		///
		void waitUntilIdle(void);

		///
		/// This method sets the speeds of both of the robot's motors.
		/// @param speedL the speed of the left motor in meters/second
		/// @param speedR the speed of the right motor in meters/second
		///
		void setWheelSpeeds(const float speedL, const float speedR);
};

#endif
