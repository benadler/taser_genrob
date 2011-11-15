#include "networkserver.h"

// Initialize the singleton-pointer to NULL
QMutex NetworkServer::singletonMutex;
NetworkServer* NetworkServer::singleton = NULL;

NetworkServer* NetworkServer::instance()
{
	QMutexLocker locker(&singletonMutex);

	if(singleton == NULL)
	{
		// First instantiation. Make sure to not only create the new object,
		// but also initialize the thread, so the clients don't have to care.
		singleton = new NetworkServer();
		singleton->moveToThread(singleton);
		singleton->start();
	}

	return singleton;
}

NetworkServer::NetworkServer(void) : QThread()
{
	logger = Logger::instance();
	config = Configuration::instance();
	logger->NetworkServer("NetworkServer::NetworkServer(): listening on port %d.", config->getMobileDServerPort());

	mutex = new QMutex(QMutex::NonRecursive);
	laserScanner = LaserScanner::instance();
	robot = Robot::instance();
	tcpServer = new QTcpServer(this);

	// when Robot emits signals with new battery/drive values, we wan to receive and save them into our cache.
	connect(robot, SIGNAL(newBatteryVoltageReady(float)), this, SLOT(slotNewBatteryVoltageReady(float)));
	connect(robot, SIGNAL(newDriveTempsReady(float, float)), this, SLOT(slotNewDriveTempsReady(float, float)));

	// unused
	//connect(robot, SIGNAL(statusChanged(RobotStatus)), this, SLOT(slotRobotStatusChanged(RobotStatus)));
}

NetworkServer::~NetworkServer(void)
{
	logger->NetworkServer("NetworkServer::~NetworkServer()");
	socket->close();
}

void NetworkServer::run()
{
	if(!tcpServer->listen(QHostAddress::Any, config->getMobileDServerPort()))
		abort("NetworkServer::NetworkServer(): Could not open server on port %d, exiting.", config->getMobileDServerPort());

	connect(tcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));

	logger->NetworkServer("NetworkServer::run(): starting event loop, thread-id is %d.", syscall(SYS_gettid));
	exec();
}

void NetworkServer::setMobileD(MobileD* mobileD)
{
	this->mobileD = mobileD;
}

void NetworkServer::slotNewConnection(void)
{
	QMutexLocker locker(mutex);

	logger->NetworkServer("NetworkServer::slotNewConnection(): new client connected.");
	socket = tcpServer->nextPendingConnection();
	connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(slotSocketStateChanged(QAbstractSocket::SocketState)));
	connect(socket, SIGNAL(readyRead()), this, SLOT(slotDataReceived()));

	// close the server, one client is enough
	tcpServer->close();
}

void NetworkServer::slotSocketStateChanged(QAbstractSocket::SocketState state)
{
	QMutexLocker locker(mutex);

	if(state == QTcpSocket::UnconnectedState)
	{
		// the client has disconnected. Reopen the server so we can get a new client
		logger->NetworkServer("NetworkServer::slotSocketStateChanged(): client disconnected, reopening server.");
		tcpServer->listen(QHostAddress::Any, config->getMobileDServerPort());
	}
}

void NetworkServer::slotDataReceived(void)
{
	QMutexLocker locker(mutex);

	unsigned int bytesAvailable = socket->bytesAvailable();

// 	logger->NetworkServer("NetworkServer::slotDataReceived(): %d bytes available.", bytesAvailable);

	// Commands have at least 4 words (=16 byte)
	if(bytesAvailable < 16)
		return;

	// There might be multiple packets waiting in the buffer. Do NOT copy the whole
	// buffer into ONE packet. We have to look for the packetLength first, which is
	// located at bytes 4,5,6,7 (see packet.h documentation for header layout).
	QByteArray packetHeader = socket->read(8);
	unsigned int packetLength = *(unsigned int*)(packetHeader.constData() + 4);

	//logger->NetworkServer("NetworkServer::slotDataReceived(): the packet being received is %d bytes long.", packetLength);

	// we can now read the whole packet into a buffer.
	QByteArray packetComplete(packetHeader);
	packetComplete.append(socket->read(packetLength - 8));

	Packet packet;
	packet.setData((const unsigned char*)packetComplete.data(), packetComplete.size());

	if(packet.isValid())
		handlePacket(&packet);
	else
		logger->NetworkServer("NetworkServer::slotDataReceived(): received an invalid packet.");

	// If there is more data waiting (for another packet), then make sure its fetched ASAP.
	if(socket->bytesAvailable() > 0)
	{
		logger->NetworkServer("NetworkServer::slotDataReceived(): There's %d more bytes waiting, re-scheduling myself.", socket->bytesAvailable());
		QTimer::singleShot(0, this, SLOT(slotDataReceived()));
	}
}

void NetworkServer::handlePacket(Packet *packet)
{
// 	logger->NetworkServer("NetworkServer::handlePacket()");

	switch(packet->getCommand())
	{
	case CMD_PING:
		//packet->popU32(); 	// deliberate error! (used for testing)
		ping();
		break;

	case CMD_CHECKPROTOCOL:
		checkProtocol();
		break;

	case CMD_GETROBOTSTATUS:
		getRobotStatus();
		break;

	case CMD_GETCURRENTPATH:
		getCurrentPath();
		break;

	case CMD_GETPOSE:
		getPose();
		break;

	case CMD_SETPOSE:
		setPose(packet);
		break;

	case CMD_UPDATEMARKS:
		updateLaserMarks();
		break;

	case CMD_STOP:
		stopMotion();
		break;

	case CMD_MOVESPLINE:
		moveSpline(packet);
		break;

	case CMD_MOVESPLINEALONG:
		moveSplineAlong(packet);
		break;

	case CMD_MOVEROTATE:
		moveRotate(packet);
		break;

	case CMD_MOVETRANSLATE:
		moveTranslate(packet);
		break;

	case CMD_GETSPEEDFACTOR:
		getSpeedFactor();
		break;

	case CMD_SETSPEEDFACTOR:
		setSpeedFactor(packet);
		break;

	case CMD_WAITFORCOMPLETED:
		waitForCompleted();
		break;

	case CMD_GETNUMSCANNERS:
		getNumScanners();
		break;

	case CMD_GETSCANNERPOSE:
		getScannerPose(packet);
		break;

	case CMD_GETSCANRADIALSCANNER:
		getScanRadialScanner(packet);
		break;

	case CMD_GETCOLLISIONAVOIDANCE:
		getCollisionAvoidance();
		break;

	case CMD_SETCOLLISIONAVOIDANCE:
		setCollisionAvoidance(packet);
		break;

	case CMD_GETBATTERYVOLTAGE:
		getBatteryVoltage();
		break;

	case CMD_GETDRIVETEMPERATURES:
		getDriveTemperatures();
		break;

	default:
		logger->NetworkServer("unknown command 0x%08x", packet->getCommand());
// 		abort("unknown command 0x%08x", cmd);
	}
}



void NetworkServer::ping(void) const
{
	logger->NetworkServer("NetworkServer::Ping()");

	Packet packetToSend(CMD_REPLY | CMD_PING);

	packetToSend.pushS32(0); 	// status "ok"

	if(!packetToSend.send(socket))
		logger->NetworkServer("NetworkServer: sending reply packet failed");
}

void NetworkServer::checkProtocol(void) const
{
	logger->NetworkServer("NetworkServer::checkProtocol()");

	Packet packetToSend(CMD_REPLY | CMD_CHECKPROTOCOL);

	packetToSend.pushS32(0); 	// status "ok"
	packetToSend.pushU32(1); 	// version?

	// We don't want to send the number of commands explicitly, we already do so implicitly via packetsize
// 	packetToSend.pushU32(99);	// bogus value, will be corrected at the end.

	packetToSend.pushU32(CMD_PING);
	packetToSend.pushU32(CMD_CHECKPROTOCOL);

	packetToSend.pushU32(CMD_GETPOSE);
	packetToSend.pushU32(CMD_SETPOSE);
	packetToSend.pushU32(CMD_GETROBOTSTATUS);
	packetToSend.pushU32(CMD_GETCURRENTPATH);

	packetToSend.pushU32(CMD_UPDATEMARKS);

	packetToSend.pushU32(CMD_STOP);
	packetToSend.pushU32(CMD_MOVESPLINE);
	packetToSend.pushU32(CMD_MOVESPLINEALONG);
	packetToSend.pushU32(CMD_MOVEROTATE);
	packetToSend.pushU32(CMD_MOVETRANSLATE);
	packetToSend.pushU32(CMD_WAITFORCOMPLETED);

	packetToSend.pushU32(CMD_GETNUMSCANNERS);
	packetToSend.pushU32(CMD_GETSCANNERPOSE);
	packetToSend.pushU32(CMD_GETSCANRADIALSCANNER);

	packetToSend.pushU32(CMD_GETBATTERYVOLTAGE);
	packetToSend.pushU32(CMD_GETDRIVETEMPERATURES);

	// Each stored data needs four bytes (int, float etc.), so we need to divide by four.
	// As we also push status, version and number of commands, we need to substract three.
	// This is not exactly portable code...
// 	unsigned int numberOfCommands = packetToSend.getDataLength() / 4 - 3;
// 	logger->NetworkServer("NetworkServer::checkProtocol(): number of supported commands: %d.", numberOfCommands);
// 	packetToSend.putU32(numberOfCommands, 2);

	if(!packetToSend.send(socket))
		logger->NetworkServer("NetworkServer::checkProtocol(): sending reply packet failed");
}

void NetworkServer::getPose(void) const
{
	logger->NetworkServer("NetworkServer::getPose()");

	Packet packetToSend(CMD_REPLY | CMD_GETPOSE);
	Pose pose = Localization::instance()->getPose();

	packetToSend.pushS32(0);	// status "ok"
	packetToSend.pushF32(pose.getX());
	packetToSend.pushF32(pose.getY());
	packetToSend.pushF32(pose.getAngleRad());

	if(!packetToSend.send(socket))
		logger->NetworkServer("NetworkServer: sending reply packet failed");
}

void NetworkServer::setPose(Packet* packet) const
{
	Pose pose;

	pose.setX(packet->popF32());
	pose.setY(packet->popF32());
	pose.setAngleRad(packet->popF32());

	logger->NetworkServer("NetworkServer::setPose(): setting Pose to %s", qPrintable(pose.toString()));

	Localization::instance()->setPose(pose);

	Packet packetToSend(CMD_REPLY | CMD_SETPOSE);

	packetToSend.pushS32(0); 	// status "ok"

	pose = Localization::instance()->getPose();

	packetToSend.pushF32(pose.getX());
	packetToSend.pushF32(pose.getY());
	packetToSend.pushF32(pose.getAngleRad());

	if(!packetToSend.send(socket))
		logger->NetworkServer("NetworkServer: sending reply packet failed");
}

void NetworkServer::updateLaserMarks(void) const
{
	logger->NetworkServer("NetworkServer::updateLaserMarks()");

	Packet packetToSend(CMD_REPLY | CMD_UPDATEMARKS);

	Correspondence::instance()->requestKnownLaserMarksFromGenMap();

	packetToSend.pushS32(0); 	// status "ok"

	if(!packetToSend.send(socket))
		logger->NetworkServer("NetworkServer: sending reply packet failed");
}

void NetworkServer::getNumScanners(void) const
{
	logger->NetworkServer("NetworkServer::getNumScanners(): sending number of laserscanners (%d) back to client.", config->getNumberOfLaserScanners());

	Packet packetToSend(CMD_REPLY | CMD_GETNUMSCANNERS);

	packetToSend.pushS32(0); 	// status "ok"
	packetToSend.pushU32(config->getNumberOfLaserScanners());

	if(!packetToSend.send(socket))
		logger->NetworkServer("NetworkServer: sending reply packet failed");
}

void NetworkServer::getScannerPose(Packet* packet) const
{
	logger->NetworkServer("NetworkServer::getScannerPosition()");

	Packet packetToSend(CMD_REPLY | CMD_GETSCANNERPOSE);

	const unsigned int index = packet->popU32();

	Pose scannerPose = config->getLaserScannerPose(config->mapIndexToScannerSide(index));

	// status is 0 if everything is ok.
	if(scannerPose.isValid())
		packetToSend.pushS32(0);
	else
		packetToSend.pushS32(-1);

	packetToSend.pushF32(scannerPose.getX());
	packetToSend.pushF32(scannerPose.getY());
	packetToSend.pushF32(scannerPose.getAngleRad());

	if(!packetToSend.send(socket))
		logger->NetworkServer("NetworkServer: sending reply packet failed");
}


void NetworkServer::getScanRadialScanner(Packet* packet) const
{
	Packet packetToSend(CMD_REPLY | CMD_GETSCANRADIALSCANNER);

	RobotSide side = config->mapIndexToScannerSide(packet->popU32());

	QList<float> scannerDistances;
	LaserScan laserScan;
	laserScanner->getCurrentLaserScan(laserScan);
	laserScan.getScannerDistances(side, scannerDistances);

	int size = scannerDistances.size();

	if(size == 0)
	{
		packetToSend.pushS32(-1); 	// status
	}
	else
	{
		logger->NetworkServer("NetworkServer::getScanRadialScanner(): pushing %d values from %s-scanner into reply packet", size, qPrintable(Configuration::getRobotSide(side)));
		packetToSend.pushS32(0); 	// status "ok"

		packetToSend.pushS32(size);	// number of scans

		for(int i=0; i < scannerDistances.size(); ++i)
		{
			logger->NetworkServer("NetworkServer::getScanRadialScanner(): pushing value %.2F", scannerDistances.at(i));
			packetToSend.pushF32(scannerDistances.at(i));
		}
	}

	if(!packetToSend.send(socket))
		logger->NetworkServer("NetworkServer: sending reply packet failed");
}


void NetworkServer::stopMotion(void) const
{
	logger->NetworkServer("NetworkServer::stopMotion()");

	// We cannot call Robot::slotSetStatus() directly, as that could release the brakes, which would
	// cause a network-packet to be sent: "SocketNotifier: socket notifiers cannot be enabled from
	// another thread". Thus, we call it indirectly so that it will be processed in its own thread.

// 	QMetaObject::invokeMethod
// 	(
// 		robot,
// 		"slotSetStatus",
// 		Qt::QueuedConnection,
// 	  	Q_ARG(RobotStatus, aborting)
// 	);

	// The above comment is false, as setting the status to "aborting" will never change the brakes'
	// status. For this, see documentation of callee.
	robot->slotSetStatus(aborting);
	
	// the protocol doesn't say that we should return 0 when the robot has stopped. The client can call
	// CMD_WAITUNTILIDLE himself if he's interested.
	//robot->waitUntilIdle();

	// WARNING: Could it be that the robot isn't really idle right now? If yes, we're sending the
	// CMD-STOP-reply-packet too early!

	Packet packetToSend(CMD_REPLY | CMD_STOP);
	packetToSend.pushS32(0);
	if(!packetToSend.send(socket))
		logger->NetworkServer("NetworkServer: sending reply packet failed");
}

void NetworkServer::waitForCompleted(void) const
{
	logger->NetworkServer("NetworkServer::waitForCompleted()");

	robot->waitUntilIdle();

	logger->NetworkServer("NetworkServer::WaitForCompleted(): is idle");

	Packet packetToSend(CMD_REPLY | CMD_WAITFORCOMPLETED);

	packetToSend.pushS32(0);

	if(!packetToSend.send(socket))
		logger->NetworkServer("NetworkServer: sending reply packet failed");
}

void NetworkServer::getBatteryVoltage(void) const
{
	// In this method, we request an update for the desired value from Robot.
	// When that update arrives, we send an answer packet. We cannot ask
	// Robot directly, as it lives in another thread and we can't use its
	// QTcpSocket directly.

	logger->NetworkServer("NetworkServer::getBatteryVoltage(): requesting batteryVoltage from Robot...");

	QMetaObject::invokeMethod
		(
		  robot,
		  "slotGetBatteryVoltage",
		  Qt::QueuedConnection
		);
}

void NetworkServer::getDriveTemperatures(void) const
{
	// In this method, we request an update for the desired value from Robot.
	// When that update arrives, we send an answer packet. We cannot ask
	// Robot directly, as it lives in another thread and we can't use its
	// QTcpSocket directly.

	logger->NetworkServer("NetworkServer::getDriveTemperatures(): requesting driveTemps from Robot...");

	QMetaObject::invokeMethod
	(
		robot,
		"slotGetDriveTemperatures",
		Qt::QueuedConnection
	);
}

void NetworkServer::getSpeedFactor() const
{
	logger->NetworkServer("NetworkServer::getSpeedFactor()");

	Packet packetToSend(CMD_REPLY | CMD_GETSPEEDFACTOR);

	packetToSend.pushS32(0);	// status
	packetToSend.pushF32(config->getOverlaySpeedFactor());

	if(!packetToSend.send(socket))
		logger->NetworkServer("NetworkServer: sending reply packet failed");
}

void NetworkServer::setSpeedFactor(Packet* packet) const
{
	logger->NetworkServer("NetworkServer::getSpeedFactor()");

	const float overlaySpeedFactor = packet->popF32();
	Packet packetToSend(CMD_REPLY | CMD_SETSPEEDFACTOR);

	if(overlaySpeedFactor >= 0.0 && overlaySpeedFactor <= 1.0)
	{
		config->setOverlaySpeedFactor(overlaySpeedFactor);
		packetToSend.pushS32(0);	// status
	}
	else
	{
		packetToSend.pushS32(-1);	// status
	}

	if(!packetToSend.send(socket))
		logger->NetworkServer("NetworkServer: sending reply packet failed");
}

void NetworkServer::getRobotStatus() const
{
	logger->NetworkServer("NetworkServer::getRobotStatus(): sending status %s back to client.", qPrintable(robot->getStatusText()));

	Packet packetToSend(CMD_REPLY | CMD_GETROBOTSTATUS);

	packetToSend.pushS32(0);	// status
	packetToSend.pushU32(robot->getStatus());

	if(!packetToSend.send(socket))
		logger->NetworkServer("NetworkServer: sending reply packet failed");
}

void NetworkServer::getCurrentPath() const
{
	logger->NetworkServer("NetworkServer::getCurrentPath()");

	Packet packetToSend(CMD_REPLY | CMD_GETCURRENTPATH);

	// The robot might currently not have a motion. Return 1 in that case.
	Motion* motion = mobileD->getCurrentMotion();

	if(
		motion != 0
		&&
		(
			robot->getStatus() == moving
			||
			robot->getStatus() == aborting
			||
			robot->getStatus() == stalled
		)
	)
	{
		packetToSend.pushS32(0);	// status

		QList<QPointF> wayPoints = motion->getPathPoints();

		// number of waypoints can be derived from packetsize
		//packetToSend.pushU32(wayPoints.size());	// number of waypoints

		foreach(const QPointF &wayPoint, wayPoints)
		{
			packetToSend.pushF32(wayPoint.x());
			packetToSend.pushF32(wayPoint.y());
		}
	}
	else
	{
		packetToSend.pushS32(-1);	// status
	}

	if(!packetToSend.send(socket))
		logger->NetworkServer("NetworkServer: sending reply packet failed");
}

void NetworkServer::getCollisionAvoidance(void) const
{
	Packet packetToSend(CMD_REPLY | CMD_GETCOLLISIONAVOIDANCE);
	packetToSend.pushS32(0);		// status

	if(CollisionAvoidance::instance()->isActive())
		packetToSend.pushU32(1);	// current ca-state
	else
		packetToSend.pushU32(0);	// current ca-state

	if(!packetToSend.send(socket))
		logger->NetworkServer("NetworkServer: sending reply packet failed");
}

void NetworkServer::setCollisionAvoidance(Packet* packet) const
{
	const bool active = (bool)packet->popS32();
	CollisionAvoidance::instance()->setActive(active);

	Packet packetToSend(CMD_REPLY | CMD_SETCOLLISIONAVOIDANCE);
	packetToSend.pushS32(0);		// status
	packetToSend.pushS32((int)active);	// new ca-state

	if(!packetToSend.send(socket))
		logger->NetworkServer("NetworkServer: sending reply packet failed");
}

void NetworkServer::moveSpline(Packet* packet) const
{
	logger->NetworkServer("NetworkServer::moveSpline() in thread %d.", syscall(SYS_gettid));

	Pose poseTarget;
	poseTarget.setX(packet->popF32());
	poseTarget.setY(packet->popF32());
	poseTarget.setAngleRad(packet->popF32());
	
	logger->NetworkServer("NetworkServer::move(): to %s", qPrintable(poseTarget.toString()));

	Packet packetToSend(CMD_REPLY | CMD_MOVESPLINE);

	if(mobileD->moveSpline(poseTarget))
		packetToSend.pushS32(0);	// status
	else
		packetToSend.pushS32(-1);	// status

	if(!packetToSend.send(socket))
		logger->NetworkServer("NetworkServer: sending reply packet failed");

	logger->NetworkServer("NetworkServer::move(): done.");
}

void NetworkServer::moveSplineAlong(Packet* packet) const
{
	logger->NetworkServer("NetworkServer::moveSplineAlong()");

	// substract one for the final orientation
	const unsigned int numberOfWayPoints = (packet->getDataLength()-1) / 4;

	Packet packetToSend(CMD_REPLY | CMD_MOVESPLINEALONG);

	if(numberOfWayPoints > 0)
	{
		QList<QPointF> wayPoints;

		for(unsigned int i=0;i<numberOfWayPoints;i++)
		{
			const float x = packet->popF32();
			const float y = packet->popF32();
			wayPoints.append(QPointF(x, y));
		}

		const float targetOrientationRad = packet->popF32();

		if(mobileD->moveSpline(wayPoints, targetOrientationRad))
			packetToSend.pushS32(0);	// status
		else
			packetToSend.pushS32(-1);	// status
	}
	else
	{
		packetToSend.pushS32(-1);	// status
	}

	if(!packetToSend.send(socket))
		logger->NetworkServer("NetworkServer: sending reply packet failed");
}

void NetworkServer::moveRotate(Packet* packet) const
{
	logger->NetworkServer("NetworkServer::moveRotate() in thread %d.", syscall(SYS_gettid));

	const float angleRad = packet->popF32();

	Packet packetToSend(CMD_REPLY | CMD_MOVEROTATE);

	if(mobileD->moveRotate(angleRad))
		packetToSend.pushS32(0);	// status
	else
		packetToSend.pushS32(-1);	// status

	if(!packetToSend.send(socket))
		logger->NetworkServer("NetworkServer: sending reply packet failed");

	logger->NetworkServer("NetworkServer::moveRotate(): done.");
}

void NetworkServer::moveTranslate(Packet* packet) const
{
	logger->NetworkServer("NetworkServer::moveTranslate() in thread %d.", syscall(SYS_gettid));

	const float distance = packet->popF32();

	Packet packetToSend(CMD_REPLY | CMD_MOVETRANSLATE);

	if(mobileD->moveTranslate(distance))
		packetToSend.pushS32(0);	// status
	else
		packetToSend.pushS32(-1);	// status

	if(!packetToSend.send(socket))
		logger->NetworkServer("NetworkServer: sending reply packet failed");

	logger->NetworkServer("NetworkServer::moveTranslate(): done.");
}

void NetworkServer::slotNewBatteryVoltageReady(float batteryVoltage)
{
	logger->NetworkServer("NetworkServer::slotNewBatteryVoltageReady(): sending voltage of %.2F back to client.", batteryVoltage);

	Packet packetToSend(CMD_REPLY | CMD_GETBATTERYVOLTAGE);
	packetToSend.pushS32(0);       	// status
	packetToSend.pushF32(batteryVoltage);

	if(!packetToSend.send(socket))
		logger->NetworkServer("NetworkServer: sending reply packet failed");
}

void NetworkServer::slotNewDriveTempsReady(float motorTempL, float motorTempR)
{
	logger->NetworkServer("NetworkServer::slotNewDriveTempsReady(): sending driveTemps of %.2F/%.2F back to client.", motorTempL, motorTempR);

	Packet packetToSend(CMD_REPLY | CMD_GETDRIVETEMPERATURES);
	packetToSend.pushS32(0);	// status
	packetToSend.pushF32(motorTempL);	// left drive
	packetToSend.pushF32(motorTempR);	// right drive

	if(!packetToSend.send(socket))
		logger->NetworkServer("NetworkServer: sending reply packet failed");
}
