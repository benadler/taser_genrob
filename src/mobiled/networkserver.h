#ifndef NETWORKSERVER_H
#define NETWORKSERVER_H

#include <QPointF>
#include <QList>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>

#include <sys/types.h>


#include "conversion.h"
#include "logger.h"
#include "localization.h"
#include "laserscanner.h"
#include "collisionavoidance.h"
#include "packet.h"
#include "robot.h"
#include "mobiled.h"
#include "protocol_mobiled.h"

///
/// @brief This class manages mobileD's connections to network clients.
///
/// As soon as one client connects, the serverSocket is closed, so only one connection can exist
/// at any time. When the client disconnects, the serverSocket is reopened.
///
/// When data arrives from the client, slotDataReceived() is called. It creates a Packet, does
/// some verifications and then calls handlePacket(packet). The latter then checks for the
/// packet's command-code and calls the necessary member-method. Most member methods send a reply
/// to the client, some member methods need the packet passed as an argument to them, because it
/// contains data necessary for the given command (e.g. Packets with CMD_MOVE contain the target
/// pose).
///

class MobileD;
class LaserScanner;

class NetworkServer : public QThread
{
	Q_OBJECT

	private:
		static NetworkServer* singleton;	///< pointer to the only instance of NetworkServer (which is a singleton class).
		static QMutex singletonMutex;		///< used to ensure no two instances of this class are created.

		QMutex* mutex;				///< used for thread-synchronization in member methods.
		QTcpServer* tcpServer;
		QTcpSocket* socket;

		Robot* robot;
		MobileD* mobileD;
		Configuration* config;
		LaserScanner* laserScanner;
		Logger* logger;

		/// Constructor. Private, as this is a singleton
		NetworkServer(void);

		/// Used to start this thread's event-loop.
		void run();

		/// This method is called with a packet received from the client. It looks at the packet's command-code and calls the matching method.
		void handlePacket(Packet *packet);

		/// This method tells the client which protocol parts are supported by sending all supported command codes.
		void checkProtocol(void) const;

		/// This method sends a ping back to the client
		void ping(void) const;

		/// This method is called when the client wants to know the robot's current position
		void getPose(void) const;

		/// This method is called by the client to set the robot's position. The robot is NOT moved,
		/// but the localization is told where the robot currently is. Used for initializing the localization
		/// @param packet The packet sent by the client. Used to read some parameters/data.
		void setPose(Packet* packet) const;

		/// This method is called when the client wants to know the robot's current status.
		void getRobotStatus(void) const;

		/// This method is called when the client wants to know the robot's current path.
		void getCurrentPath(void) const;

		/// This method is called by the client and causes correspondence the reload the known laserMarks,
		/// either from a file or from genMap via Java/JNI.
		void updateLaserMarks(void) const;

		/// This method is called by the client to find out how many laserscanners are installed.
		void getNumScanners(void) const;

		/// This method is called by the client to determine a laserscanner's position relative to the robot's center.
		void getScannerPose(Packet* packet) const;

		/// This method is called by the client to request a given laserscanner's scandata.
		void getScanRadialScanner(Packet* packet) const;


		/// This method is called by the client to make the robot move along a spline through set of given points.
		void moveSplineAlong(Packet* packet) const;
		/// This method is called by the client to make the robot move to a given Pose. The WAY to arrive at the Pose
		/// is determined by mobiled, using Java/genPath.
		void moveSpline(Packet* packet) const;
		void moveRotate(Packet* packet) const;
		void moveTranslate(Packet* packet) const;

		void setCollisionAvoidance(Packet* packet) const;
		void getCollisionAvoidance(void) const;



		/// This method can be called by the client to stop the robot's motion immediately.
		void stopMotion(void) const;

		/// This method is called by the client to get the robot's current speedFactor.
		void getSpeedFactor() const;
		/// This method is called by the client to set the robot's current speedFactor.
		void setSpeedFactor(Packet* packet) const;

		/// This method is called by the client and replies with a packet as soon as the robot has finished moving.
		void waitForCompleted(void) const;

		/// This method is called by the client to get the robot's current battery voltage.
		void getBatteryVoltage(void) const;

		/// This method is called by the client to get the robot's current motor temperatures.
		void getDriveTemperatures(void) const;

	private slots:
		/// This member slot is called when a client connects to the serverSocket.
		void slotNewConnection(void);

		/// This member slot is called when the serverSocket has changed its state.
		void slotSocketStateChanged(QAbstractSocket::SocketState state);

		/// This member slot is called when the client has sent us some data.
		void slotDataReceived(void);

		/// This member slot is called when the robot has changed its status
		// unused, it seems
		//void slotRobotStatusChanged(robotStatus status);

	public slots:
		// these are called by Robot when it has received the new values from CAN
		void slotNewBatteryVoltageReady(float batteryVoltage);
		void slotNewDriveTempsReady(float motorTempL, float motorTempR);

	public:
		///
		/// This static method returns a pointer to the only NetworkServer-object in existence. If there is no
		/// NetworkServer-object yet, one will be created before returning a pointer to it.
		///
		static NetworkServer* instance(void);

		/// This method is used to give NetworkServer mobileD's address, so that callbacks can be made, e.g.
		/// to tell mobileD to CMD_MOVE to a given Pose.
		void setMobileD(MobileD* mobileD);

		/// Destruktor.
		~NetworkServer(void);
};

#endif
