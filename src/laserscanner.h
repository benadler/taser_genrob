#ifndef LASERSCANNER_H
#define LASERSCANNER_H

#include <QList>
#include <QTimer>
#include <QUdpSocket>
#include <QByteArray>
#include <QThread>

#include <net/if.h> // for if_nametoindex()
#include <errno.h> // for errno()
// #include <netinet/in.h>
 #include <arpa/inet.h>
// #include <assert.h>

#include "pose.h"
#include "java.h"
#include "robot.h"
#include "conversion.h"
#include "configuration.h"
#include "localization.h"
#include "collisionavoidance.h"
#include "laserscan.h"
#include "lasermark.h"
#include "logger.h"

/// @brief This class receives Laserscans via UDP packets and stores them in LaserScan objects
///
/// We use a small device that collects laserscan data from two Sick LMS 200 scanners via
/// serial port, processes the data and sends us each scan in a UDP multicast packet.
/// The data-format and all other required information can be obtained from Hannes Bistry's
/// diploma-thesis, available at tams-www.informatik.uni-hamburg.de, filename is DA_bistry.pdf.
///
/// After receiving the measurements, this class updates the robot's localization. Also used
/// for dynamic path-planning, i.e. for changing the robots path when obstacles are detected.
///
/// Packets from the forward scanner have an address of 0x80 (simple measurement data) or 0x90
/// (extracted marks and area violations). The rear scanner's packets use 0x81 and 0x91, resp.

#define SCANDATA_FRONT 0x80
#define SCANDATA_REAR 0x81
#define MARKDATA_FRONT 0x90
#define MARKDATA_REAR 0x91

enum AlarmLevel
{
	green,	// 0 good to go
	yellow,	// 1 someone's there
	red,	// 2 roadkill
 	invalid // 3 well, invalid.
};

class CollisionAvoidance;

class LaserScanner : public QThread
{
	Q_OBJECT

	private:
		static LaserScanner* singleton;	///< pointer to the only instance of LaserScanner (which is a singleton class).
		static QMutex singletonMutex;

		int collisionAvoidanceUpdateCounter; ///< this counter keeps track of incoming laserScan packets and causes a CA-update on every Nth packet.

		QMutex* mutex;		///< a mutex, used to synchronize access from multiple threads.

		QUdpSocket *udpSocket;	///< the socket used to receive laser scan data
		Logger* logger;
		Configuration* config;

		CollisionAvoidance *collisionAvoidance;	///< a pointer to the collisionavoidance, used for fedding updates to it.

		QByteArray datagram;			///< used for reading and processing incoming UDP packets / laserscans
		AlarmLevel alarmLevelFront, alarmLevelRear;	///< used to save the scanner's current alarm levels (red=2/yellow=1/green=0)

		QList<LaserMarkMeasured>	laserMarksFront, laserMarksRear;	///< these lists hold the measured/detected LaserMarks

		Robot* robot;			///< localization and collisionAvoidance are only called when the robot is not idle.

		LaserScan* laserScan;		///< this member-object holds the latest laserscanner-data.

		/// Private constructor, this is a singleton.
		LaserScanner(void);

		/// Private destruktor, as this is a singleton
		~LaserScanner(void);

		/// This method reads scandata from a QByteArray that was received from the rabbit
		/// 3000 via the udpSocket and writes it into the LaserScan member-object.
		/// @param datagram a QByteArray containing the scan-packet received from the rabbit3000
		/// @param side the side at which the receiving scanner was located
		/// @return true when the data was successfully added to the LaserScan object
		bool convertDatagramToScan(const QByteArray datagram, const RobotSide side);

		/// The rabbit 3000 sends us special packets containing distance/angle tuples of measured/
		/// detected LaserMarks. This method adds that data to the LaserMarkMeasured-List member.
		/// @param datagram a QByteArray containing the marks-packet received from the rabbit 3000
		/// @param side the RobotSide from which the scandata originates.
		/// @return true when the data was successfully added to the LaserMark-List.
		bool updateLaserMarks(const QByteArray datagram, const RobotSide side);

		/// This method takes the detected marks and sends them to the localization-class.
		void sendMarksToLocalization(void);

		/// This method is updates the CollisionAvoidance with a new LaserScan.
		void updateCollisionAvoidance(void) const;

	private slots:
		/// This method handles incoming UDP packets and creates corresponding LaserScan-Objects.
		void slotReadPendingDatagrams(void);

	public:
		/// This static method returns a pointer to the only LaserScanner-object in existence. If there is no
		/// LaserScanner-object yet, one will be created before returning a pointer to it.
		static LaserScanner* instance(void);

		/// Calling start() will call this method, which in turn starts this thread's eventloop.
		void run(void);

		void getCurrentLaserScan(LaserScan &scan) const;

		/// This method cleanly shuts down. It powers down the laserscanners using java/jni/iowarrior,
		/// closes the UDP socket, stops the collisionavoidance updateTimer etc.
		void shutdown(void);
		
		AlarmLevel getAlarmLevel(const RobotSide side) const;
};

#endif
