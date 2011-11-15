#include "laserscanner.h"

// Initialize the singleton-pointer to NULL
QMutex LaserScanner::singletonMutex;
LaserScanner* LaserScanner::singleton = NULL;

LaserScanner* LaserScanner::instance()
{
	QMutexLocker locker(&singletonMutex);

	if(singleton == NULL)
	{
		// First instantiation. Make sure to not only create the new object,
		// but also initialize the thread, so the clients don't have to care.
		singleton = new LaserScanner();
		singleton->moveToThread(singleton);
		singleton->start();
	}

	return singleton;
}

LaserScanner::LaserScanner(void) : QThread()
{
	logger = Logger::instance();
	logger->LaserScanner("LaserScanner::LaserScanner()");

	mutex = new QMutex();

	config = Configuration::instance();

	collisionAvoidance = CollisionAvoidance::instance();

	robot = Robot::instance();

	laserScan = new LaserScan();

	collisionAvoidanceUpdateCounter = 0;
	
	// this level is invalid, but will be updated immediately.
	alarmLevelFront = alarmLevelRear = invalid;

	int fd, on = 1;

	if((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		abort("LaserScanner::LaserScanner(): socket() failed.");

	struct sockaddr_in sa;
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(2222);
	sa.sin_addr.s_addr = INADDR_ANY;

	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	if(bind(fd, (struct sockaddr*) &sa, sizeof(sa)) < 0)
		abort("LaserScanner::LaserScanner(): bind() failed.");

	struct ip_mreqn mreq;
	memset(&mreq, 0, sizeof(mreq));
	mreq.imr_multiaddr.s_addr = inet_addr("224.0.0.23");
	mreq.imr_address.s_addr = INADDR_ANY;
	mreq.imr_ifindex = if_nametoindex("eth0");

	if(setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
		abort("LaserScanner::LaserScanner(): setsockopt() failed.");

	udpSocket = new QUdpSocket(this);
	if(! udpSocket->setSocketDescriptor(fd))
		abort("LaserScanner::LaserScanner(): qudpsocket didn't accept the native socketDescriptor, exiting.");

	connect(udpSocket, SIGNAL(readyRead()), this, SLOT(slotReadPendingDatagrams()));

	Java* java = Java::instance();

	foreach(const int port, config->getIoWarriorPortsLaserScanner())
	{
		logger->LaserScanner("LaserScanner::LaserScanner(): powering up laserscanner on port %d.", port);
		QMetaObject::invokeMethod
		(
			java,
			"slotSetIoWarriorPort",
			Qt::AutoConnection,
			Q_ARG(unsigned int, port),
			Q_ARG(bool, true)
		);
	}
}

LaserScanner::~LaserScanner(void)
{
	logger->LaserScanner("LaserScanner::~LaserScanner()");
	delete laserScan;
	delete udpSocket;
}

void LaserScanner::shutdown(void)
{
	QMutexLocker locker(mutex);

	logger->LaserScanner("LaserScanner::shutdown(): powering down laserscanners...");
	foreach(int port, config->getIoWarriorPortsLaserScanner())
	{
		QMetaObject::invokeMethod
		(
			Java::instance(),
			"slotSetIoWarriorPort",
			Qt::QueuedConnection,
			Q_ARG(unsigned int, port),
			Q_ARG(bool, false)
		);
	}

	logger->LaserScanner("LaserScanner::shutdown(): closing UDP socket...");
	udpSocket->close();
}

void LaserScanner::run(void)
{
	// start this thread's event loop
	logger->LaserScanner("LaserScanner::run(): starting event loop.");
	exec();

	// start the timer that updates our collisionAvoidance.
// 	logger->LaserScanner("LaserScanner::run(): starting collisionAvoidanceUpdateTimer.");
// 	collisionAvoidanceUpdateTimer->start(config->getCollisionAvoidanceUpdateInterval());
}

void LaserScanner::slotReadPendingDatagrams(void)
{
	QMutexLocker locker(mutex);

	//logger->LaserScanner("LaserScanner::slotReadPendingDatagrams()");
	while(udpSocket->hasPendingDatagrams())
	{
		datagram.resize(udpSocket->pendingDatagramSize());
		udpSocket->readDatagram(datagram.data(), datagram.size());

		// set the last byte to zero. Don't know why.
		datagram.data()[datagram.size()] = 0;

		// Valid packets start with a StartOfText/STX-Byte
		if(datagram.at(0) != 0x02)
		{
			logger->LaserScanner("LaserScanner::slotReadPendingDatagrams(): STX not found");
			continue;
		}

		unsigned char scannerId = datagram.at(1);

		// increment the packet counter, as we want to update CA on every Nth packet.
		collisionAvoidanceUpdateCounter++;

		//logger->LaserScanner("LaserScanner::slotReadPendingDatagrams(): received a packet from scanner 0x%02x", scannerId);

		if((unsigned char)datagram.at(4) == 0xb0 && datagram.size() == 732)
		{
			// Telegram containing normal scandata.
			if(scannerId == SCANDATA_FRONT)
				convertDatagramToScan(datagram, sideFront);

			if(scannerId == SCANDATA_REAR)
				convertDatagramToScan(datagram, sideRear);
		}
		if((unsigned char)datagram.at(4) == 0xb3)
		{
			// Telegram containing extracted marks and alarm levels.
			if(scannerId == MARKDATA_FRONT)
			{
				updateLaserMarks(datagram, sideFront);
				AlarmLevel alarmLevelFrontNew = (AlarmLevel)datagram.at((datagram.at(2)|(datagram.at(3)<<8))+4-1);
				if(alarmLevelFront != alarmLevelFrontNew)
					logger->LaserScanner("LaserScanner::slotReadPendingDatagrams(): new alarm level front: %d", alarmLevelFrontNew);
				
				alarmLevelFront = alarmLevelFrontNew;
			}

			if(scannerId == MARKDATA_REAR)
			{
				updateLaserMarks(datagram, sideRear);
				AlarmLevel alarmLevelRearNew = (AlarmLevel)datagram.at((datagram.at(2)|(datagram.at(3)<<8))+4-1);
				
				if(alarmLevelRear != alarmLevelRearNew)
					logger->LaserScanner("LaserScanner::slotReadPendingDatagrams(): new alarm level rear: %d", alarmLevelRearNew);
				
				alarmLevelRear = alarmLevelRearNew;
			}
		}

		// We receive packets like this: 80 (scanfront), 90 (markfront), 81 (scanrear), 91 (markrear), ...
		// Update Localization when the rear marks have arrived.
		if(scannerId == MARKDATA_REAR)
		{
			// No need to update localization when the robot isn't moving.
			// We SHOULD ALWAYS update it, the robot should not lose track of its pose when someone moves it around.
			if(robot->getStatus() != idle || true)
			{
				//logger->LaserScanner("LaserScanner::slotReadPendingDatagrams(): sending marks to localization after rear mark data arrived");
				sendMarksToLocalization();
			}
		}

		if(collisionAvoidanceUpdateCounter == 50)
		{
			collisionAvoidanceUpdateCounter = 0;
			updateCollisionAvoidance();
		}
	}
}

void LaserScanner::updateCollisionAvoidance(void) const
{
// 	qDebug("LaserScanner::updateCollisionAvoidance()");
	// No need to update collisionAvoidance when the robot isn't moving.
	if(robot->getStatus() != idle)
		collisionAvoidance->updateObstacles(laserScan);
}

void LaserScanner::sendMarksToLocalization()
{
	// feed the results to the localization
	QList<LaserMarkMeasured> laserMarks;
	laserMarks << laserMarksFront;
	laserMarks << laserMarksRear;

	//logger->LaserScanner("LaserScanner::sendMarksToLocalization(): sending %d marks to localization.", laserMarks.size());

	Localization::instance()->updatePoseUsingLaserMarks(laserMarks);
}

bool LaserScanner::convertDatagramToScan(const QByteArray datagram, RobotSide side)
{
	if((datagram.at(5) != 105) || (datagram.at(6) != 65 && datagram.at(6) != 69))
	{
		logger->LaserScanner("LaserScanner::convertDatagramToScan(): scan data corrupt, ignoring packet.");
		return false;
	}

	QList<float> angles, distances;

	unsigned char *data = (unsigned char*)datagram.data() + 7;	// the header is 7 bytes long
	int count = 0;
	for(int i = -180; i < 181; i++)
	{
		unsigned short dist = *(data++);
		dist |= (*(data++) & 0x1f) << 8;

		if(dist <= 0x1ff7)
		{
			angles << Conversion::deg2rad(i * 0.5); // angles now go from -90 (to the right) to 90 (to the left)
			distances << dist / 1000.0; // was millimeters, now in meters.
			//logger->LaserScanner("LaserScanner::convertDatagramToScan(): %d / %d: now appending angle %.2F, distance %.2F.", i, count, angles[count], distances[count]);
			count++;
		}
	}

	//logger->LaserScanner("LaserScanner::convertDatagramToScan(): %d angles, %d distances for side %d.", angles.size(), distances.size(), side);

	laserScan->setScanData(angles, distances, side);
	return true;
}

bool LaserScanner::updateLaserMarks(const QByteArray datagram, RobotSide side)
{
	unsigned char *data = (unsigned char*)datagram.data();
	if (datagram.size() < 4 || (int)((data[2]|(data[3]<<8))+4) != datagram.size())
	{
		logger->LaserScanner("LaserScanner::updateLaserMarks(): mark data corrupt, ignoring packet.");
		return false;
	}

	int nummarks = ((data[2]|(data[3]<<8))-2)/4;
	data += 5;

	if(side == sideFront) laserMarksFront.clear();
	if(side == sideRear) laserMarksRear.clear();

	if(nummarks == 0)
		logger->LaserScanner("LaserScanner::updateLaserMarks(): %s: no marks in this packet.", qPrintable(Configuration::getRobotSide(side)));

	// whats the pose of the source scanners relative to the platform?
	Pose poseScanner = config->getLaserScannerPose(side);

	float distance, angleRad, scannerX, scannerY, platformX, platformY;

	for(int i=0; i < nummarks; i++)
	{
		unsigned short distanceMM = *(data++);
		distanceMM |= (*(data++)) << 8; // in millimeters
		distance = distanceMM / 1000.0;

		unsigned short angle = *(data++);
		angle |= (*(data++)) << 8; // in 120th of a degree

		// We need to convert the polar scanner coordinates to cartesic platform coordinates.
		// The angle data goes counterclockwise from 0 (to the right) to 180 (to the left) degrees,
		angleRad = Conversion::deg2rad(angle / 120.0);

		// Get cartesian scanner coordinates
		scannerX = cos(angleRad + poseScanner.getAngleRad()) * distance;
		scannerY = sin(angleRad + poseScanner.getAngleRad()) * distance;

		//logger->LaserScanner("LaserScanner::updateLaserMarks(): %s: scannerX %.2F, scannerY %.2F", qPrintable(Configuration::getRobotSide(side)), scannerX, scannerY);

		platformX = poseScanner.getX() + scannerX;
		platformY = poseScanner.getY() + scannerY;

		//logger->LaserScanner("LaserScanner::updateLaserMarks(): %s: platformX %.2F, platformY %.2F", qPrintable(Configuration::getRobotSide(side)), platformX, platformY);

		LaserMarkMeasured mark(QPointF(platformX, platformY));

		/*
		logger->LaserScanner("LaserScanner::updateLaserMarks(): %s: mark found: %s",
			qPrintable(Configuration::getRobotSide(side)),
			qPrintable(mark.toString())
		);
		*/

		if(side == sideFront)
			laserMarksFront.append(mark);

		if(side == sideRear)
			laserMarksRear.append(mark);
	}

	return true;
}

void LaserScanner::getCurrentLaserScan(LaserScan &scan) const
{
	QMutexLocker locker(mutex);

	scan = *laserScan;
}

AlarmLevel LaserScanner::getAlarmLevel(const RobotSide side) const
{
	if(side == sideFront)
		return alarmLevelFront;
	else if(side == sideRear)
		return alarmLevelRear;
	else
		abort("LaserScanner::getAlarmLevel(): i was asked for an alarm level on a side other than front or rear!");
}
